#pragma once

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opencv2/opencv.hpp>

#include "CaptureDevice_PGRCamera2.h"
#include "CaptureDevice_BlackflyS.h"
#include "./Binarizer.h"
#include "./RegionLabeler.h"
#include "./ObjectTracker.h"

#include "Network/SocketUDP.h"

using namespace std;

/* オブジェクトオブジェクトのデータ構造 */
typedef struct {
	unsigned int id;						// 0: ID無し
	unsigned int lastUpdatedTime;
	Rect boundingRect;
	Point2f centroid;
	float size;
} LightObject;

ObjectTracker objectTracker;
vector<TrackingObject> *trackedObjectArrayPtr;

/* カメラの種類 or カメラなし */
typedef enum CAMERA_SDK_TYPE {
	CAMERA_SDK_TYPE_NO_CAMERA = 0,			// カメラを使わない(デバッグ用も兼ねる)
	CAMERA_SDK_TYPE_FLYCAPTURE2 = 1,		// FlyCaptureSDK2(Flea3等)
	CAMERA_SDK_TYPE_SPINNAKER = 2,			// Spinnaker SDK(BlackFlyS等)
};
static CAMERA_SDK_TYPE cameraSDKType = CAMERA_SDK_TYPE_FLYCAPTURE2;

/* 送信データフォーマット */
typedef struct {
	int channel;
	int index;
	int value;
} NetworkData;

NetworkData sendBuffer;

static int maxObjects;

/* サイズ閾値 */
static int maxSizeThreshold;
static int minSizeThreshold;

static int slowUpdateIntervalMSec = 1000;

static Timer *timer;

CaptureDevice *currentCaptureDevicePtr;
PGRCamera *pgrCamera;
BlackflyS *blackflyS;

/* 画像処理補助クラス */
Binarizer binarizer;
RegionLabeler regionLabeler;
vector<Region> regions;
int regionCounter;
float totalSize = 0.0f;

/* 2値化閾値 */
int binarizingThresholdBlockSize;
int adaptiveThresholdBlockSize;
double adaptiveThresholdCValue;

/* 最大オブジェクト領域数 */
int maxRegions;

/* カメラフォーマット */
int cameraIndex;
FlyCapture2::PixelFormat format;

/* 中間画像 */
cv::Mat binarizedImageCopyGRAY;

/* その他 */
int binarizingThreshold;

/* カメラのROI */
cv::Rect roi;

/* UDP */
UDPClient *udpClient;


vector<Region> resultRegionArray;

/* オブジェクト画像データ */
cv::Mat* srcImageGRAY;								// カメラ生画像(Gray)
cv::Mat backgroundImageGRAY;					// 背景画像(Gray)
cv::Mat srcImageRGBAForUnity;					// カメラ生画像(RGBAForUnity for Unity)

cv::Mat binarizedImageGRAY;						// カメラ2値画像(Gray)
cv::Mat binarizedImageRGBAForUnity;		// カメラ2値画像(RGBAForUnity for Unity)

cv::Mat maskImageGRAY;								// 輝度平均計算用マスク画像
cv::Mat maskImageInvGRAY;							// 輝度平均計算用マスク画像(白黒反転)
cv::Mat maskImageWithHandShapeGRAY;		// 輝度平均計算用マスク画像(白黒反転)

cv::Mat maskedSrcImageGRAY;						// 領域マスク後のカメラ画像
cv::Mat maskedSrcImageRGBAForUnity;		// 領域マスク後のカメラ画像

/* FPSの更新 */
unsigned int fps = 0;
unsigned int frameCounter = 0;
unsigned int fpsTick = 0;

void calcFPS() {
	frameCounter++;
	if (fpsTick + 1000 < timer->systemTime) {
		fpsTick = timer->systemTime;
		fps = frameCounter;
		frameCounter = 0;
	}
} // calcFPS
