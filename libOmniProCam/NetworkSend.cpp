/*********************************************************
	�l�b�g���[�N with Point Grey Research�А��J����
	2014/06/19 by �����r��@UECISMS
	����: �\�[�X�R�[�h�̊O���ւ̔z�z�͌��ւƂ���B
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
using namespace cv;

/* �J�����N���X */
#include "./Camera/CaptureDevice_PGRCamera2_FireflyMV.h"

#include "./ImageProcessing/Binarizer.h"
#include "./ImageProcessing/RegionLabeler.h"
#include "./ImageProcessing/ObjectTracker.h"
#include "./Network/SocketUDP.h"

/* �摜 */
Mat *srcImageGRAY;
Mat *binarizedImageGRAY;
Mat *backgroundImageGRAY;

double thresholdValue = 100;

/* �J���� */
FireflyMV *fireflyMV;

/* �J�����ݒ� */
static const int CAMERA_INDEX = 0;
static const int CAMERA_ROI_X = 0;
static const int CAMERA_ROI_Y = 0;
static const int CAMERA_ROI_WIDTH = 752;
static const int CAMERA_ROI_HEIGHT = 480;
static const PixelFormat pixelFormat = PIXEL_FORMAT_MONO8;
static const float CAMERA_SHUTTER_SPEED = 10;
static const float CAMERA_GAIN = 5.0f;

/* ��� */
typedef enum {
	STATE_CAPTURE_BACKGROUND,
	STATE_CALC_BACKGROUND_SUBTRACTION,
} BACKGROUND_SUBTRACTION_STATE;
BACKGROUND_SUBTRACTION_STATE state = STATE_CAPTURE_BACKGROUND;

/* 2�l���N���X */
Binarizer binarizer;

/* ���x�����O�N���X */
RegionLabeler regionLabeler;

/* ���x�����O���� */
Vector<Region> regions;

/* ���x�����O�ݒ� */
static const int MAX_REGIONS = 20;
static const double MAX_REGION_SIZE_THRESHOLD = 30000.0;
static const double MIN_REGION_SIZE_THRESHOLD = 100.0;

/* �g���b�L���O�Ώۂ̃f�[�^�\�� */
static const int MAX_TRACKING_OBJECTS = 10;
ObjectTracker objectTracker;
Vector<TrackingObject> trackingObjectArray;

/* �l�b�g���[�N�ő���f�[�^ */
typedef struct{
	unsigned int id;
	unsigned int x;
	unsigned int y;
} NetworkData;
NetworkData sendBuffer[MAX_TRACKING_OBJECTS];		// ���M�o�b�t�@
UDPClient *udpClient = NULL;

/* ������ */
int initialize(){

	/* �J�����̏����� */
	fireflyMV = FireflyMV::getInstance();
	fireflyMV->setCameraIndex(CAMERA_INDEX);
	if ( fireflyMV->initialize() < 0 ){
		printf("�J�����̏������Ɏ��s���܂����B\n");
		return -1;
	}
	fireflyMV->setCustomImageParameter(CAMERA_ROI_X, CAMERA_ROI_Y, CAMERA_ROI_WIDTH, CAMERA_ROI_HEIGHT, pixelFormat);
	fireflyMV->setShutterSpeed(CAMERA_SHUTTER_SPEED);
	fireflyMV->setGain(CAMERA_GAIN);

	/* �摜������ */
	srcImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);
	binarizedImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);
	backgroundImageGRAY = new Mat(CAMERA_ROI_HEIGHT, CAMERA_ROI_WIDTH, CV_8UC1);

	/* 2�l���N���X */
	binarizer.initialize(CAMERA_ROI_WIDTH, CAMERA_ROI_HEIGHT);

	/* ���x�����O���ʏ����� */
	regions.resize(MAX_REGIONS);

	/* �E�C���h�E�쐬 */
	namedWindow("srcImage");
	namedWindow("labelingResult");

	/* �g���b�L���O������ */
	trackingObjectArray.resize(MAX_TRACKING_OBJECTS);
	for (int i = 0; i < MAX_TRACKING_OBJECTS; i++){
		trackingObjectArray[i].id = 0;
	}

	/* �l�b�g���[�N������ */
	SocketUDP::initializeSocket();
	udpClient = SocketUDP::createClient("localhost", 31416);
	for (int i = 0; i < MAX_TRACKING_OBJECTS; i++){
		sendBuffer[i].id = 0;
	}

	/* �B�e�J�n */
	if ( fireflyMV->startCamera() < 0){
		printf("�J�������J�n�ł��܂���ł����B\n");
		return -1;
	}

	return 0;
} // initialize

/* ��� */
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

	/* ������ */
	if ( initialize() < 0 ){
		printf("�������Ɏ��s���܂����B\n");
		return -1;
	}

	/* ���[�v */
	while (1){

		char key = waitKey(1);

		/* q�ŏI�� */
		if ( key == 'q' ){
			break;
		}
		
		switch ( state ){

			case STATE_CAPTURE_BACKGROUND:

				/* �摜�擾 */
				fireflyMV->grabImage(*srcImageGRAY);

				/* b�Ŕw�i�擾 */
				if ( key == 'c' ){
					state = STATE_CALC_BACKGROUND_SUBTRACTION;
					srcImageGRAY->copyTo(*backgroundImageGRAY);
					printf("Background Captured!\n");
				}

				/* ���ʕ\�� */
				imshow("srcImage", *srcImageGRAY);
				break;

			case STATE_CALC_BACKGROUND_SUBTRACTION:

				/* �摜�擾 */
				fireflyMV->grabImage(*srcImageGRAY);

				/* 2�l�� */
				binarizer.calcBackgroundSubtraction(*srcImageGRAY, *binarizedImageGRAY, *backgroundImageGRAY, thresholdValue);

				/* ���x�����O */
				int regionCounter;
				regionCounter = regionLabeler.detectRegion(*binarizedImageGRAY, MAX_REGION_SIZE_THRESHOLD, MIN_REGION_SIZE_THRESHOLD, regions);

				/* �g���b�L���O */
				objectTracker.trackObjects(regions, regionCounter, trackingObjectArray);

				/* ���M */
				for ( int i = 0; i < MAX_TRACKING_OBJECTS; i++ ){
					sendBuffer[i].id = trackingObjectArray[i].id;
					sendBuffer[i].x = trackingObjectArray[i].x;
					sendBuffer[i].y = trackingObjectArray[i].y;
				}
				udpClient->send(sendBuffer, sizeof(NetworkData) * MAX_TRACKING_OBJECTS);		// ���M

				/* ���ʂ̕\�� */
				for ( int i = 0; i < regionCounter; i++ ){

					/* �d�S�ʒu��`�� */
					circle(*srcImageGRAY, Point((int)regions[i].centroid.x, (int)regions[i].centroid.y), 3, Scalar(128, 128, 128));

					/* ��܋�`��`�� */
					rectangle(*srcImageGRAY, 
						Point(regions[i].boundingRect.x, regions[i].boundingRect.y), 
						Point(regions[i].boundingRect.x + regions[i].boundingRect.width, regions[i].boundingRect.y + regions[i].boundingRect.height),
						Scalar(128, 128, 128), 3);

					/* �����厲��`�� */
					line(*srcImageGRAY, Point(regions[i].centroid.x + cosf(regions[i].angle) * 100.0f, 
						regions[i].centroid.y + sinf(regions[i].angle) * 100.0f), 
						Point(regions[i].centroid.x - cosf(regions[i].angle) * 100.0f, 
						regions[i].centroid.y - sinf(regions[i].angle) * 100.0f), Scalar(128, 128, 128), 2);
				}

				imshow("labelingResult", *srcImageGRAY);
				break;
		} // switch

	} // mainLoop
	
	/* ��� */
	finalize();

	return 0;
}
