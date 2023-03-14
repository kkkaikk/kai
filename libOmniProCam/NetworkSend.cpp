/*********************************************************
	ネットワーク with Point Grey Research社製カメラ
	2014/06/19 by 佐藤俊樹@UECISMS
	注意: ソースコードの外部への配布は厳禁とする。
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
using namespace cv;

/* カメラクラス */
#include "./Camera/CaptureDevice_PGRCamera2_FireflyMV.h"

#include "./ImageProcessing/Binarizer.h"
#include "./ImageProcessing/RegionLabeler.h"
#include "./ImageProcessing/ObjectTracker.h"
#include "./Network/SocketUDP.h"

/* 画像 */
Mat *srcImageGRAY;
Mat *binarizedImageGRAY;
Mat *backgroundImageGRAY;

double thresholdValue = 100;

/* カメラ */
FireflyMV *fireflyMV;

/* カメラ設定 */
static const int CAMERA_INDEX = 0;
static const int CAMERA_ROI_X = 0;
static const int CAMERA_ROI_Y = 0;
static const int CAMERA_ROI_WIDTH = 752;
static const int CAMERA_ROI_HEIGHT = 480;
static const PixelFormat pixelFormat = PIXEL_FORMAT_MONO8;
static const float CAMERA_SHUTTER_SPEED = 10;
static const float CAMERA_GAIN = 5.0f;

/* 状態 */
typedef enum {
	STATE_CAPTURE_BACKGROUND,
	STATE_CALC_BACKGROUND_SUBTRACTION,
} BACKGROUND_SUBTRACTION_STATE;
BACKGROUND_SUBTRACTION_STATE state = STATE_CAPTURE_BACKGROUND;

/* 2値化クラス */
Binarizer binarizer;

/* ラベリングクラス */
RegionLabeler regionLabeler;

/* ラベリング結果 */
Vector<Region> regions;

/* ラベリング設定 */
static const int MAX_REGIONS = 20;
static const double MAX_REGION_SIZE_THRESHOLD = 30000.0;
static const double MIN_REGION_SIZE_THRESHOLD = 100.0;

/* トラッキング対象のデータ構造 */
static const int MAX_TRACKING_OBJECTS = 10;
ObjectTracker objectTracker;
Vector<TrackingObject> trackingObjectArray;

/* ネットワークで送るデータ */
typedef struct{
	unsigned int id;
	unsigned int x;
	unsigned int y;
} NetworkData;
NetworkData sendBuffer[MAX_TRACKING_OBJECTS];		// 送信バッファ
UDPClient *udpClient = NULL;

/* 初期化 */
int initialize(){

	/* カメラの初期化 */
	fireflyMV = FireflyMV::getInstance();
	fireflyMV->setCameraIndex(CAMERA_INDEX);
	if ( fireflyMV->initialize() < 0 ){
		printf("カメラの初期化に失敗しました。\n");
		return -1;
	}
	fireflyMV->setCustomImageParameter(CAMERA_ROI_X, CAMERA_ROI_Y, CAMERA_ROI_WIDTH, CAMERA_ROI_HEIGHT, pixelFormat);
	fireflyMV->setShutterSpeed(CAMERA_SHUTTER_SPEED);
	fireflyMV->setGain(CAMERA_GAIN);

	/* 画像初期化 */
	srcImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);
	binarizedImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);
	backgroundImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);

	/* 2値化クラス */
	binarizer.initialize(CAMERA_ROI_WIDTH, CAMERA_ROI_HEIGHT);

	/* ラベリング結果初期化 */
	regions.resize(MAX_REGIONS);

	/* ウインドウ作成 */
	namedWindow("srcImage");
	namedWindow("labelingResult");

	/* トラッキング初期化 */
	trackingObjectArray.resize(MAX_TRACKING_OBJECTS);
	for (int i = 0; i < MAX_TRACKING_OBJECTS; i++){
		trackingObjectArray[i].id = 0;
	}

	/* ネットワーク初期化 */
	SocketUDP::initializeSocket();
	udpClient = SocketUDP::createClient("localhost", 31416);
	for (int i = 0; i < MAX_TRACKING_OBJECTS; i++){
		sendBuffer[i].id = 0;
	}

	/* 撮影開始 */
	if ( fireflyMV->startCamera() < 0){
		printf("カメラが開始できませんでした。\n");
		return -1;
	}

	return 0;
} // initialize

/* 解放 */
void finalize(){

	delete udpClient;
	SocketUDP::finalizeSocket();

	trackingObjectArray.clear();

	delete backgroundImageGRAY;
	delete binarizedImageGRAY;
	delete srcImageGRAY;
	fireflyMV->stopCamera();
	fireflyMV->finalize();
	FireflyMV::releaseInstance();
} // finalize

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
int main(){

	/* 初期化 */
	if ( initialize() < 0 ){
		printf("初期化に失敗しました。\n");
		return -1;
	}

	/* ループ */
	while (1){

		char key = waitKey(1);

		/* qで終了 */
		if ( key == 'q' ){
			break;
		}
		
		switch ( state ){

			case STATE_CAPTURE_BACKGROUND:

				/* 画像取得 */
				fireflyMV->grabImage(*srcImageGRAY);

				/* bで背景取得 */
				if ( key == 'c' ){
					state = STATE_CALC_BACKGROUND_SUBTRACTION;
					srcImageGRAY->copyTo(*backgroundImageGRAY);
					printf("Background Captured!\n");
				}

				/* 結果表示 */
				imshow("srcImage", *srcImageGRAY);
				break;

			case STATE_CALC_BACKGROUND_SUBTRACTION:

				/* 画像取得 */
				fireflyMV->grabImage(*srcImageGRAY);

				/* 2値化 */
				binarizer.calcBackgroundSubtraction(*srcImageGRAY, *binarizedImageGRAY, *backgroundImageGRAY, thresholdValue);

				/* ラベリング */
				int regionCounter;
				regionCounter = regionLabeler.detectRegion(*binarizedImageGRAY, MAX_REGION_SIZE_THRESHOLD, MIN_REGION_SIZE_THRESHOLD, regions);

				/* トラッキング */
				objectTracker.trackObjects(regions, regionCounter, trackingObjectArray);

				/* 送信 */
				for ( int i = 0; i < MAX_TRACKING_OBJECTS; i++ ){
					sendBuffer[i].id = trackingObjectArray[i].id;
					sendBuffer[i].x = trackingObjectArray[i].x;
					sendBuffer[i].y = trackingObjectArray[i].y;
				}
				udpClient->send(sendBuffer, sizeof(NetworkData) * MAX_TRACKING_OBJECTS);		// 送信

				/* 結果の表示 */
				for ( int i = 0; i < regionCounter; i++ ){

					/* 重心位置を描画 */
					circle(*srcImageGRAY, Point((int)regions[i].centroid.x, (int)regions[i].centroid.y), 3, Scalar(128, 128, 128));

					/* 包含矩形を描画 */
					rectangle(*srcImageGRAY, 
						Point(regions[i].boundingRect.x, regions[i].boundingRect.y), 
						Point(regions[i].boundingRect.x + regions[i].boundingRect.width, regions[i].boundingRect.y + regions[i].boundingRect.height),
						Scalar(128, 128, 128), 3);

					/* 慣性主軸を描画 */
					line(*srcImageGRAY, Point(regions[i].centroid.x + cosf(regions[i].angle) * 100.0f, 
						regions[i].centroid.y + sinf(regions[i].angle) * 100.0f), 
						Point(regions[i].centroid.x - cosf(regions[i].angle) * 100.0f, 
						regions[i].centroid.y - sinf(regions[i].angle) * 100.0f), Scalar(128, 128, 128), 2);
				}

				imshow("labelingResult", *srcImageGRAY);
				break;
		} // switch

	} // mainLoop
	
	/* 解放 */
	finalize();

	return 0;
}
