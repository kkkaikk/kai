/****************************************************************
		CaptureDevice_PGRCamera.h.h		FlyCapture�J�����N���X(Flycapture SDK Ver.2�Ή���)
		�T�v:
			FlyCapture�J�����𐧌䂷��N���X�B
			��{�I�ɃJ�X�^���C���[�W���[�h�œ������B
		ToDo: �J���[���������͂܂��������Ȃ̂œ����
		2014/06/11 by �����r��@UECISMS
		2015/11/02 �O���g���K���[�h�ǉ�		by �����r��
 ****************************************************************/
#ifndef __CAPTUREDEVICE_FLYCAPTURECAM2_H__
#define __CAPTUREDEVICE_FLYCAPTURECAM2_H__

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <Windows.h>

#include "CaptureDevice.h"

#include <FlyCapture2.h>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

/* �J�X�^���C���[�W�̃p�����[�^ */
typedef struct{
	unsigned int uiMode;
	unsigned int uiImagePosLeft;		// left
	unsigned int uiImagePosTop;			// top
	unsigned int uiWidth;						// width
	unsigned int uiHeight;					// height
	float fBandWidth;								// speed
	FlyCapture2::PixelFormat format;							// format
} CUSTOM_IMAGE_PARAMETER;

static const int FLY_DEFAULT_CAMERA_INDEX = 0;

/* �f�t�H���g�J�X�^���C���[�W�p�����[�^ */
static const int FLY_DEFAULT_VEHICLE = 0;
static const int FLY_DEFAULT_LEFT = 0;
static const int FLY_DEFAULT_WIDTH = 640;
static const int FLY_DEFAULT_HEIGHT = 480;
static const float FLY_DEFAULT_SPEED = 100.0f;

/************************************************
					FlyCapture�J�����N���X
 ************************************************/
class PGRCamera : public CaptureDevice{

public:

	/* �R���X�g���N�^ */
	PGRCamera();
	
	/* �f�X�g���N�^ */
	virtual ~PGRCamera();
	
	/* �O���u�����t���[���f�[�^�̃|�C���^�����̂܂�IPL�C���[�W�w�b�_�ɂ������ēn�� */
	virtual cv::Mat *grabImage();

	/* �^����ꂽ�C���[�W�f�[�^��imageData�|�C���^�Ƀt���[���̃|�C���^���������� */
	virtual void grabImage(cv::Mat &imageHeader);
		
	/************************************
		Custom Image (Region of Interest)
	 ************************************/

	/* �J�X�^���C���[�W�p�����[�^��S���ݒ肷��B
		 ROI�̃T�C�Y�͐ݒ�ł���l�ɃJ�������ɐ���������̂�FlyCap.exe��CustomMode(Format7)�̍��ڂ��}�j���A�����Q�� */
	virtual int setCustomImageParameter( int x, int y, int width, int height, FlyCapture2::PixelFormat format, FlyCapture2::Mode mode = FlyCapture2::MODE_0 );
		
	/************************************
		External GPIO Trigger Mode
		(���̊֐���initialize()�����setCustomImageParameter()��ɌĂԂ���!!)
	 ************************************/

	/* �g���K���[�h(mode = 0: StandardExternalTrigger, 1: BulbShutterTrigger, 14: OverlappedExposureReadoutTrigger, 15: MultiShotTrigger) */
	virtual int setTriggerMode(unsigned int mode, unsigned int source, bool isRisingEdge = true, int timeOut = 5000, unsigned int parameter = 0);		// ���̊֐���initialize()�O�ɌĂԂ���!!

	/* �g���K���[�h���� */
	virtual int resetTriggerMode();

	/**********************************
						������ / �I��
	 **********************************/

	/* ������ */
	virtual int initialize();

	/* ������ */
	virtual int initialize(int x, int y, int width, int height, FlyCapture2::PixelFormat format);

	/* �I�� */
	virtual void finalize();

	/* �J�����̍ăX�^�[�g */
	virtual void restartCamera();

	/***********************************
						�J�n / ��~
	 ***********************************/

	/* �J�n */
	virtual int startCamera();

	/* ��~ */
	virtual int stopCamera();

	/****************************************
										�ݒ�
	 ****************************************/

	/* �J�����C���f�b�N�X���Z�b�g���� */
	virtual void setCameraIndex(int index);

	/****************************************
						�J�����p�����[�^�ݒ�
		(Flycap.exe��Auto�ݒ���I�t�ɂ��Ă����K�v������)
	 ****************************************/

	/* �Q�C�� */
	void setGain(float gainValue);

	/* �u�I�u�W�F�N�g�l�X */
	void setBrightness(float brightnessValue);

	/* �G�N�X�|�[�W�� */
	void setExposure(float exposureValue);

	/* �K���} */
	void setGamma(float gammaValue);

	/* �V���b�^�[�X�s�[�h */
	void setShutterSpeed(float shutterValue);

	/* �t���[�����[�g */
	void setFrameRate(float frameRateValue);

	/* ROI���擾 */
	virtual CvRect getROI();

	/* Image Size/Offset Units */
	virtual int getImageSizeUnit();
	virtual int getImageOffsetUnit();

	/* width/height */
	virtual int getMaxCameraWidth();
	virtual int getMaxCameraHeight();

	/* �\�t�g�E�F�A�g���K */
	bool fireSoftwareTrigger(FlyCapture2::Camera* pCam ){
		const unsigned int k_softwareTrigger = 0x62C;
		const unsigned int k_fireVal = 0x80000000;
		FlyCapture2::Error error;
		error = pCam->WriteRegister( k_softwareTrigger, k_fireVal );
		if (error != FlyCapture2::PGRERROR_OK){
			printf("SoftwareTrigger�Ɏ��s���܂����B\n");
			return false;
		}
		return true;
	} // fireSoftwareTrigger

	/****************************************
						�J���[�v���Z�X
	 ****************************************/
	virtual void setColorProcessing();

protected:

	FlyCapture2::Error error;

	FlyCapture2::PGRGuid guid;

	FlyCapture2::BusManager busManager;

	FlyCapture2::Camera camera;
	FlyCapture2::CameraInfo camInfo;

	FlyCapture2::Image rawImage;
	FlyCapture2::Image convertedImage;

	FlyCapture2::Property frmRate;

	unsigned int numCameras;

	CvRect roiRect;

	FlyCapture2::Mode format7Mode;
	FlyCapture2::Format7Info format7Info;
	FlyCapture2::Format7ImageSettings format7ImageSettings;
	FlyCapture2::Format7PacketInfo format7PacketInfo;
	FlyCapture2::PixelFormat pixelFormat;
	FlyCapture2::TriggerMode triggerMode;

	bool supported;

 	cv::Mat *tmpImageHeader;

	int enableColorProcessing;
	int isCustomParameterInitialized;
  int isCameraStarted;											// �L���v�`�����J�n����Ă��邩
	int cameraIndex;

	CUSTOM_IMAGE_PARAMETER customImageParam;	// Custom Image�̐ݒ�

};
#endif