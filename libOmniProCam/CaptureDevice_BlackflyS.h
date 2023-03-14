/****************************************************************
		CaptureDevice_BlackflyS.h		BlackflySカメラクラス
		概要: BlackflyS制御用
		2017/10/16 by 佐藤俊樹@TokyoTech/Taiwagata Media Research Institute inc.
 ****************************************************************/
#ifndef __CAPTUREDEVICE_BLACKFLYS_H__
#define __CAPTUREDEVICE_BLACKFLYS_H__

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "CaptureDevice.h"

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

/************************************************
					FlyCaptureカメラクラス
 ************************************************/
class BlackflyS : public CaptureDevice{

public:

	/* コンストラクタ */
	BlackflyS();
	
	/* デストラクタ */
	virtual ~BlackflyS();
	
	/**********************************
						Grab(Copyあり)
	 **********************************/
	virtual cv::Mat *grabImage();
	virtual void grabImage(cv::Mat &imageHeader);		// 手動バッファ解放が必要

	/* 手動バッファ解放() */
	virtual void releaseImageBuffer(){		
		pResultImage->Release();	// Release image
	} // releaseImageBuffer

	/**********************************
						初期化 / 終了
	 **********************************/

	/* 初期化 */
	virtual int initialize();

	/* 終了 */
	virtual void finalize();

	/* カメラの再スタート */
	virtual void restartCamera();

	/* カメラインデックスをセットする */
	virtual void setCameraIndex(int index);

	/***********************************
						開始 / 停止
	 ***********************************/

	/* 開始 */
	virtual int startCamera();

	/* 停止 */
	virtual int stopCamera();

	/************************************************
												設定項目
	 ************************************************/

	/* シャッタスピード */
	void setShutter(int shutterSpeedUSec){
		if ( !isInitialized ){
			printf("カメラが初期化されていません。\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();
		CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
		exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());
		CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
		exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());
		CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
		exposureTime->SetValue(shutterSpeedUSec);
	} // setShutterSpeed

	/* Gain */
	void setGain(float gain){
		if (!isInitialized) {
			printf("カメラが初期化されていません。\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();
		CEnumerationPtr gainAuto = nodeMap.GetNode("GainAuto");
		gainAuto->SetIntValue(gainAuto->GetEntryByName("Off")->GetValue());
		CFloatPtr gainValue = nodeMap.GetNode("Gain");
		gainValue->SetValue(gain);
	} // setGain

	/* Trigger Mode */
	void enableTriggerMode(){
		if (!isInitialized) {
			printf("カメラが初期化されていません。\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();
		CEnumerationPtr triggerMode = nodeMap.GetNode("TriggerMode");
		triggerMode->SetIntValue(triggerMode->GetEntryByName("On")->GetValue());

		CEnumerationPtr triggerSource = nodeMap.GetNode("TriggerSource");
		triggerSource->SetIntValue(triggerSource->GetEntryByName("Line0")->GetValue());

		CEnumerationPtr triggerSelector = nodeMap.GetNode("TriggerSelector");
		triggerSelector->SetIntValue(triggerSelector->GetEntryByName("FrameStart")->GetValue());

		CEnumerationPtr triggerActivation = nodeMap.GetNode("TriggerActivation");
		triggerActivation->SetIntValue(triggerActivation->GetEntryByName("RisingEdge")->GetValue());
	} // enableTriggerMode

	/* Image Bufferの数 */
	void setNumImageBuffers(int numBuffers){
		Spinnaker::GenApi::INodeMap & sNodeMap = pCam->GetTLStreamNodeMap();
		CIntegerPtr StreamNode = sNodeMap.GetNode("StreamDefaultBufferCount");
		if ( numBuffers > StreamNode->GetMax() ){
			numBuffers = StreamNode->GetMax();
		}
		StreamNode->SetValue(numBuffers);	// Default: 10, Max: 11
		INT64 bufferCount = StreamNode->GetValue();
	} // setNumImageBuffers

	/* ROIの設定 */
	void setROI(int offsetX, int offsetY, int width, int height) {

		if (!isInitialized) {
			printf("カメラが初期化されていません。\n");
			return;
		}
		
		/* ここで設定したroiRectは最終的にカメラに設定された値に書き換わる */
		roiRect.x = offsetX;			roiRect.y = offsetY;
		roiRect.width = width;		roiRect.height = height;

		INodeMap &nodeMap = pCam->GetNodeMap();
		CIntegerPtr ptrOffsetX = nodeMap.GetNode("OffsetX");
		if (IsAvailable(ptrOffsetX) && IsWritable(ptrOffsetX)) {
			roiRect.x -= roiRect.x % ptrOffsetX->GetInc();
			ptrOffsetX->SetValue(roiRect.x);
			roiRect.x = ptrOffsetX->GetValue();
			printf("Offset X set to %d\n", roiRect.x);
		} else {
			printf("Offset X not available...\n");
		}

		CIntegerPtr ptrOffsetY = nodeMap.GetNode("OffsetY");
		if (IsAvailable(ptrOffsetY) && IsWritable(ptrOffsetY)) {
			roiRect.y -= roiRect.y % ptrOffsetY->GetInc();
			ptrOffsetY->SetValue(roiRect.y);
			roiRect.y = ptrOffsetY->GetValue();
			printf("Offset Y set to %d\n", roiRect.y);
		} else {
			printf("Offset Y not available...\n");
		}

		CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
		if (IsAvailable(ptrWidth) && IsWritable(ptrWidth)) {
			roiRect.width -= roiRect.width % ptrWidth->GetInc();
			ptrWidth->SetValue(roiRect.width);
			roiRect.width = ptrWidth->GetValue();
			printf("Width set to %d\n", roiRect.width);
		} else {
			printf("Width not available...");
		}

		CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
		if (IsAvailable(ptrHeight) && IsWritable(ptrHeight)) {
			roiRect.height -= roiRect.height % ptrHeight->GetInc();
			ptrHeight->SetValue(roiRect.height);
			roiRect.height = ptrHeight->GetValue();
			printf("Height set to %d...\n", roiRect.height);
		} else {
			printf("Height not available...\n");
		}
	} // setROI

	/* ROIの指定 */
	cv::Rect getROI(){
		return roiRect;
	} // getROI

	/* ROIを最大設定する */
	void setMaxROI() {

		if (!isInitialized) {
			printf("カメラが初期化されていません。\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();

		// Apply minimum to offset X
		CIntegerPtr ptrOffsetX = nodeMap.GetNode("OffsetX");
		if (IsAvailable(ptrOffsetX) && IsWritable(ptrOffsetX)) {
			ptrOffsetX->SetValue(ptrOffsetX->GetMin());
			roiRect.x = ptrOffsetX->GetMin();
			printf("Offset X set to %d\n", roiRect.x);
		} else {
			printf("Offset X not available...\n");
		}

		// Apply minimum to offset Y
		CIntegerPtr ptrOffsetY = nodeMap.GetNode("OffsetY");
		if (IsAvailable(ptrOffsetY) && IsWritable(ptrOffsetY)) {
			ptrOffsetY->SetValue(ptrOffsetY->GetMin());
			roiRect.y = ptrOffsetY->GetValue();
			printf("Offset Y set to %d\n", roiRect.y);
		} else {
			printf("Offset Y not available...\n");
		}

		// Set maximum width
		CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
		if (IsAvailable(ptrWidth) && IsWritable(ptrWidth)) {
			ptrWidth->SetValue(ptrWidth->GetMax());
			roiRect.width = ptrWidth->GetValue();
			printf("Width set to %d\n", roiRect.width);
		} else {
			printf("Width not available...\n");
		}

		// Set maximum height
		CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
		if (IsAvailable(ptrHeight) && IsWritable(ptrHeight)) {
			ptrHeight->SetValue(ptrHeight->GetMax());
			roiRect.height = ptrHeight->GetValue();
			printf("Height set to %d\n", roiRect.height);
		} else {
			printf("Height not available...\n");
		}
	} // setMaxROI

		/* PixelFormatの設定 */
	void setPixelFormat() {

		if (!isInitialized) {
			printf("カメラがStartされていません。\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();

		/* PixelFormatが存在するか確認 */
		CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");// Retrieve the enumeration node from the nodemap
		if (IsAvailable(ptrPixelFormat) && IsWritable(ptrPixelFormat)) {

			/* PixelFormatに指定したい設定(pixelFormat)が指定できるかどうか確認 */
			CEnumEntryPtr pixelFormatPtr = ptrPixelFormat->GetEntryByName(pixelFormat);	// Retrieve the desired entry node from the enumeration node
			if (IsAvailable(pixelFormatPtr) && IsReadable(pixelFormatPtr)) {

				/* 指定したい項目の値を得て設定する */
				int64_t pixelFormatValue = pixelFormatPtr->GetValue();	// Retrieve the integer value from the entry node						
				ptrPixelFormat->SetIntValue(pixelFormatValue);					// Set integer as new value for enumeration node
				printf("Pixel format set to %d\n", ptrPixelFormat->GetCurrentEntry()->GetSymbolic());

			} else {
				printf("Pixel format %s not available...\n", pixelFormat);
			}

		} else {
			printf("Pixel format not available...\n");
		}
	} // setPixelFormat
	
protected:

	unsigned int numCameras;
	unsigned int cameraIndex;

	SystemPtr system;

	// *** NOTES ***
	// The CameraPtr object is a shared pointer, and will generally clean itself
	// up upon exiting its scope. However, if a shared pointer is created in the
	// same scope that a system object is explicitly released (i.e. this scope),
	// the reference to the shared point must be broken manually.
	CameraPtr pCam;
	
	CameraList camList;	

	ImagePtr pResultImage;
	cv::Mat tmpImage;
	cv::Mat tmpImage2;

	int	isCameraStarted;
	int isCustomParameterInitialized;

	const char *pixelFormat = "Mono8";

	/* ROIの設定 */
	cv::Rect roiRect;

	/* 画像バッファの再作成 */
	void reCreateBuffer(){
		if (tmpImage.cols != roiRect.width || tmpImage.rows != roiRect.height) {
			tmpImage = cv::Mat::zeros(roiRect.height, roiRect.width, CV_8UC1);
			tmpImage2 = cv::Mat::zeros(roiRect.height, roiRect.width, CV_8UC1);
			printf("画像バッファが作成されました。\n");
		}
	} // reCreateBuffer

};
#endif