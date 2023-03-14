/****************************************************************
		CaptureDevice_BlackflyS.h		BlackflyS�J�����N���X
		�T�v: BlackflyS����p
		2017/10/16 by �����r��@TokyoTech/Taiwagata Media Research Institute inc.
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
					FlyCapture�J�����N���X
 ************************************************/
class BlackflyS : public CaptureDevice{

public:

	/* �R���X�g���N�^ */
	BlackflyS();
	
	/* �f�X�g���N�^ */
	virtual ~BlackflyS();
	
	/**********************************
						Grab(Copy����)
	 **********************************/
	virtual cv::Mat *grabImage();
	virtual void grabImage(cv::Mat &imageHeader);		// �蓮�o�b�t�@������K�v

	/* �蓮�o�b�t�@���() */
	virtual void releaseImageBuffer(){		
		pResultImage->Release();	// Release image
	} // releaseImageBuffer

	/**********************************
						������ / �I��
	 **********************************/

	/* ������ */
	virtual int initialize();

	/* �I�� */
	virtual void finalize();

	/* �J�����̍ăX�^�[�g */
	virtual void restartCamera();

	/* �J�����C���f�b�N�X���Z�b�g���� */
	virtual void setCameraIndex(int index);

	/***********************************
						�J�n / ��~
	 ***********************************/

	/* �J�n */
	virtual int startCamera();

	/* ��~ */
	virtual int stopCamera();

	/************************************************
												�ݒ荀��
	 ************************************************/

	/* �V���b�^�X�s�[�h */
	void setShutter(int shutterSpeedUSec){
		if ( !isInitialized ){
			printf("�J����������������Ă��܂���B\n");
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
			printf("�J����������������Ă��܂���B\n");
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
			printf("�J����������������Ă��܂���B\n");
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

	/* Image Buffer�̐� */
	void setNumImageBuffers(int numBuffers){
		Spinnaker::GenApi::INodeMap & sNodeMap = pCam->GetTLStreamNodeMap();
		CIntegerPtr StreamNode = sNodeMap.GetNode("StreamDefaultBufferCount");
		if ( numBuffers > StreamNode->GetMax() ){
			numBuffers = StreamNode->GetMax();
		}
		StreamNode->SetValue(numBuffers);	// Default: 10, Max: 11
		INT64 bufferCount = StreamNode->GetValue();
	} // setNumImageBuffers

	/* ROI�̐ݒ� */
	void setROI(int offsetX, int offsetY, int width, int height) {

		if (!isInitialized) {
			printf("�J����������������Ă��܂���B\n");
			return;
		}
		
		/* �����Őݒ肵��roiRect�͍ŏI�I�ɃJ�����ɐݒ肳�ꂽ�l�ɏ�������� */
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

	/* ROI�̎w�� */
	cv::Rect getROI(){
		return roiRect;
	} // getROI

	/* ROI���ő�ݒ肷�� */
	void setMaxROI() {

		if (!isInitialized) {
			printf("�J����������������Ă��܂���B\n");
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

		/* PixelFormat�̐ݒ� */
	void setPixelFormat() {

		if (!isInitialized) {
			printf("�J������Start����Ă��܂���B\n");
			return;
		}
		INodeMap &nodeMap = pCam->GetNodeMap();

		/* PixelFormat�����݂��邩�m�F */
		CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");// Retrieve the enumeration node from the nodemap
		if (IsAvailable(ptrPixelFormat) && IsWritable(ptrPixelFormat)) {

			/* PixelFormat�Ɏw�肵�����ݒ�(pixelFormat)���w��ł��邩�ǂ����m�F */
			CEnumEntryPtr pixelFormatPtr = ptrPixelFormat->GetEntryByName(pixelFormat);	// Retrieve the desired entry node from the enumeration node
			if (IsAvailable(pixelFormatPtr) && IsReadable(pixelFormatPtr)) {

				/* �w�肵�������ڂ̒l�𓾂Đݒ肷�� */
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

	/* ROI�̐ݒ� */
	cv::Rect roiRect;

	/* �摜�o�b�t�@�̍č쐬 */
	void reCreateBuffer(){
		if (tmpImage.cols != roiRect.width || tmpImage.rows != roiRect.height) {
			tmpImage = cv::Mat::zeros(roiRect.height, roiRect.width, CV_8UC1);
			tmpImage2 = cv::Mat::zeros(roiRect.height, roiRect.width, CV_8UC1);
			printf("�摜�o�b�t�@���쐬����܂����B\n");
		}
	} // reCreateBuffer

};
#endif