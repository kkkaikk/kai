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

/* �I�u�W�F�N�g�I�u�W�F�N�g�̃f�[�^�\�� */
typedef struct {
	unsigned int id;						// 0: ID����
	unsigned int lastUpdatedTime;
	Rect boundingRect;
	Point2f centroid;
	float size;
} LightObject;

ObjectTracker objectTracker;
vector<TrackingObject> *trackedObjectArrayPtr;

/* �J�����̎�� or �J�����Ȃ� */
typedef enum CAMERA_SDK_TYPE {
	CAMERA_SDK_TYPE_NO_CAMERA = 0,			// �J�������g��Ȃ�(�f�o�b�O�p�����˂�)
	CAMERA_SDK_TYPE_FLYCAPTURE2 = 1,		// FlyCaptureSDK2(Flea3��)
	CAMERA_SDK_TYPE_SPINNAKER = 2,			// Spinnaker SDK(BlackFlyS��)
};
static CAMERA_SDK_TYPE cameraSDKType = CAMERA_SDK_TYPE_FLYCAPTURE2;

/* ���M�f�[�^�t�H�[�}�b�g */
typedef struct {
	int channel;
	int index;
	int value;
} NetworkData;

NetworkData sendBuffer;

static int maxObjects;

/* �T�C�Y臒l */
static int maxSizeThreshold;
static int minSizeThreshold;

static int slowUpdateIntervalMSec = 1000;

static Timer *timer;

CaptureDevice *currentCaptureDevicePtr;
PGRCamera *pgrCamera;
BlackflyS *blackflyS;

/* �摜�����⏕�N���X */
Binarizer binarizer;
RegionLabeler regionLabeler;
vector<Region> regions;
int regionCounter;
float totalSize = 0.0f;

/* 2�l��臒l */
int binarizingThresholdBlockSize;
int adaptiveThresholdBlockSize;
double adaptiveThresholdCValue;

/* �ő�I�u�W�F�N�g�̈搔 */
int maxRegions;

/* �J�����t�H�[�}�b�g */
int cameraIndex;
FlyCapture2::PixelFormat format;

/* ���ԉ摜 */
cv::Mat binarizedImageCopyGRAY;

/* ���̑� */
int binarizingThreshold;

/* �J������ROI */
cv::Rect roi;

/* UDP */
UDPClient *udpClient;


vector<Region> resultRegionArray;

/* �I�u�W�F�N�g�摜�f�[�^ */
cv::Mat* srcImageGRAY;								// �J�������摜(Gray)
cv::Mat backgroundImageGRAY;					// �w�i�摜(Gray)
cv::Mat srcImageRGBAForUnity;					// �J�������摜(RGBAForUnity for Unity)

cv::Mat binarizedImageGRAY;						// �J����2�l�摜(Gray)
cv::Mat binarizedImageRGBAForUnity;		// �J����2�l�摜(RGBAForUnity for Unity)

cv::Mat maskImageGRAY;								// �P�x���όv�Z�p�}�X�N�摜
cv::Mat maskImageInvGRAY;							// �P�x���όv�Z�p�}�X�N�摜(�������])
cv::Mat maskImageWithHandShapeGRAY;		// �P�x���όv�Z�p�}�X�N�摜(�������])

cv::Mat maskedSrcImageGRAY;						// �̈�}�X�N��̃J�����摜
cv::Mat maskedSrcImageRGBAForUnity;		// �̈�}�X�N��̃J�����摜

/* FPS�̍X�V */
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
