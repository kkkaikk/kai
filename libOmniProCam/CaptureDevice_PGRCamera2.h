/****************************************************************
		CaptureDevice_PGRCamera.h.h		FlyCaptureカメラクラス(Flycapture SDK Ver.2対応版)
		概要:
			FlyCaptureカメラを制御するクラス。
			基本的にカスタムイメージモードで動かす。
		ToDo: カラー現像処理はまだ未完成なので入れる
		2014/06/11 by 佐藤俊樹@UECISMS
		2015/11/02 外部トリガモード追加		by 佐藤俊樹
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

/* カスタムイメージのパラメータ */
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

/* デフォルトカスタムイメージパラメータ */
static const int FLY_DEFAULT_VEHICLE = 0;
static const int FLY_DEFAULT_LEFT = 0;
static const int FLY_DEFAULT_WIDTH = 640;
static const int FLY_DEFAULT_HEIGHT = 480;
static const float FLY_DEFAULT_SPEED = 100.0f;

/************************************************
					FlyCaptureカメラクラス
 ************************************************/
class PGRCamera : public CaptureDevice{

public:

	/* コンストラクタ */
	PGRCamera();
	
	/* デストラクタ */
	virtual ~PGRCamera();
	
	/* グラブしたフレームデータのポインタをそのままIPLイメージヘッダにくっつけて渡す */
	virtual cv::Mat *grabImage();

	/* 与えられたイメージデータのimageDataポインタにフレームのポインタを書き込む */
	virtual void grabImage(cv::Mat &imageHeader);
		
	/************************************
		Custom Image (Region of Interest)
	 ************************************/

	/* カスタムイメージパラメータを全部設定する。
		 ROIのサイズは設定できる値にカメラ毎に制限があるのでFlyCap.exeのCustomMode(Format7)の項目かマニュアルを参照 */
	virtual int setCustomImageParameter( int x, int y, int width, int height, FlyCapture2::PixelFormat format, FlyCapture2::Mode mode = FlyCapture2::MODE_0 );
		
	/************************************
		External GPIO Trigger Mode
		(この関数はinitialize()およびsetCustomImageParameter()後に呼ぶこと!!)
	 ************************************/

	/* トリガモード(mode = 0: StandardExternalTrigger, 1: BulbShutterTrigger, 14: OverlappedExposureReadoutTrigger, 15: MultiShotTrigger) */
	virtual int setTriggerMode(unsigned int mode, unsigned int source, bool isRisingEdge = true, int timeOut = 5000, unsigned int parameter = 0);		// この関数はinitialize()前に呼ぶこと!!

	/* トリガモード解除 */
	virtual int resetTriggerMode();

	/**********************************
						初期化 / 終了
	 **********************************/

	/* 初期化 */
	virtual int initialize();

	/* 初期化 */
	virtual int initialize(int x, int y, int width, int height, FlyCapture2::PixelFormat format);

	/* 終了 */
	virtual void finalize();

	/* カメラの再スタート */
	virtual void restartCamera();

	/***********************************
						開始 / 停止
	 ***********************************/

	/* 開始 */
	virtual int startCamera();

	/* 停止 */
	virtual int stopCamera();

	/****************************************
										設定
	 ****************************************/

	/* カメラインデックスをセットする */
	virtual void setCameraIndex(int index);

	/****************************************
						カメラパラメータ設定
		(Flycap.exeでAuto設定をオフにしておく必要がある)
	 ****************************************/

	/* ゲイン */
	void setGain(float gainValue);

	/* ブオブジェクトネス */
	void setBrightness(float brightnessValue);

	/* エクスポージャ */
	void setExposure(float exposureValue);

	/* ガンマ */
	void setGamma(float gammaValue);

	/* シャッタースピード */
	void setShutterSpeed(float shutterValue);

	/* フレームレート */
	void setFrameRate(float frameRateValue);

	/* ROIを取得 */
	virtual CvRect getROI();

	/* Image Size/Offset Units */
	virtual int getImageSizeUnit();
	virtual int getImageOffsetUnit();

	/* width/height */
	virtual int getMaxCameraWidth();
	virtual int getMaxCameraHeight();

	/* ソフトウェアトリガ */
	bool fireSoftwareTrigger(FlyCapture2::Camera* pCam ){
		const unsigned int k_softwareTrigger = 0x62C;
		const unsigned int k_fireVal = 0x80000000;
		FlyCapture2::Error error;
		error = pCam->WriteRegister( k_softwareTrigger, k_fireVal );
		if (error != FlyCapture2::PGRERROR_OK){
			printf("SoftwareTriggerに失敗しました。\n");
			return false;
		}
		return true;
	} // fireSoftwareTrigger

	/****************************************
						カラープロセス
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
  int isCameraStarted;											// キャプチャが開始されているか
	int cameraIndex;

	CUSTOM_IMAGE_PARAMETER customImageParam;	// Custom Imageの設定

};
#endif