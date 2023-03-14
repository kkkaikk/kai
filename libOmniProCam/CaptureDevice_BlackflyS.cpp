#include "CaptureDevice_BlackflyS.h"

/* �R���X�g���N�^ */
BlackflyS::BlackflyS() : CaptureDevice(){
	cameraIndex = 0;
	isCameraStarted = 0;
	isCustomParameterInitialized = 0;
} // �R���X�g���N�^

/* �f�X�g���N�^ */
BlackflyS::~BlackflyS(){
} // �f�X�g���N�^

/* �O���u�����t���[���f�[�^�̃|�C���^���R�s�[�������̂�n�� */
cv::Mat *BlackflyS::grabImage(){

	if (!isCameraStarted){
		return NULL;
	}

	try {

		// Retrieve next received image and ensure image completion
		pResultImage = pCam->GetNextImage();

		if (pResultImage->IsIncomplete()) {

			printf("Image incomplete with image status %d\n", pResultImage->GetImageStatus());
			
		} else {			
			//ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);	// Convert image to mono 8
			tmpImage2.data = (uchar*)pResultImage->GetData();
			tmpImage2.copyTo(tmpImage);		// �R�s�[����
		}

		pResultImage->Release();	// Release image

		return &tmpImage;

	} catch (Spinnaker::Exception &e) {
		printf("Error: %s\n", e.what());
		return NULL;
	}
} // grabImage

/* �^����ꂽ�C���[�W�f�[�^��data���o�b�t�@�f�[�^�̃|�C���^�ɒu��������(Copy�Ȃ��B������Release���ĂԂ���) */
void BlackflyS::grabImage(cv::Mat &imageHeader){

	INodeMap &nodeMap = pCam->GetNodeMap();								// Retrieve GenICam nodemap

	if (!isCameraStarted) {
		return;
	}

	try {
				
		pResultImage = pCam->GetNextImage();	// Retrieve next received image and ensure image completion

		if (pResultImage->IsIncomplete()) {

			printf("Image incomplete with image status %d\n", pResultImage->GetImageStatus());

		} else {
			imageHeader.data = (uchar*)pResultImage->GetData();		// ���̌�ARelease()���K�v�I�I
		}
		
	} catch (Spinnaker::Exception &e) {
		printf("Error: %s\n", e.what());
	}

} // grabImage

/************************************
								SET
 ************************************/

/* �J�����C���f�b�N�X���Z�b�g���� */
void BlackflyS::setCameraIndex(int index){
	cameraIndex = index;
} // setCameraIndex

/**********************************
					������ / �I��
 **********************************/
/* ������ */
int BlackflyS::initialize(){

	if ( !isInitialized ){
		
		system = System::GetInstance();		// Retrieve singleton reference to system object		
		camList = system->GetCameras();		// Retrieve list of cameras from the system
		numCameras = camList.GetSize();

		if (numCameras == 0) {
			camList.Clear();			// Clear camera list before releasing system
			system->ReleaseInstance();// Release system
			printf("Not enough cameras!");
			printf("Done! Press Enter to exit...");
			return -1;
		}

		pCam = camList.GetByIndex(cameraIndex);	// Select camera
		pCam->Init();		// Initialize camera		

		// Retrieve device serial number for filename
		gcstring deviceSerialNumber("");
		INodeMap &nodeMapTLDevice = pCam->GetTLDeviceNodeMap();	// Retrieve TL device nodemap and print device information
		CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
		if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial)) {
			deviceSerialNumber = ptrStringSerial->GetValue();
			printf("Device serial number retrieved as %u\n", deviceSerialNumber);
		}

		printf("�J�����̏��������������܂����B\n");

		isInitialized = 1;
		return 0;
	}

	return -1;
} // initialize

/* �I�� */
void BlackflyS::finalize(){
	if ( isInitialized ){			

		pCam->DeInit();								// Deinitialize camera

		// Shared pointers can be terminated manually by assigning them to NULL.
		// This keeps releasing the system from throwing an exception.
		pCam = NULL;

		camList.Clear();							// Clear camera list before releasing system
		system->ReleaseInstance();		// Release system
		isInitialized = 0;
	}
} // finalize

/* �J�����̍ăX�^�[�g */
void BlackflyS::restartCamera(){
	stopCamera();
	startCamera();
} // restartCamera

/***********************************
					�J�n / ��~
 ***********************************/

/* �J�n */
int BlackflyS::startCamera(){
	if ( isInitialized ){

		INodeMap &nodeMap = pCam->GetNodeMap();								// Retrieve GenICam nodemap

		// Set acquisition mode to continuous
		CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
		if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
			printf("Unable to set acquisition mode to continuous (node retrieval). Aborting...\n");
			return -1;
		}

		CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
		if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous)) {
			printf("Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting...\n");
			return -2;
		}

		ptrAcquisitionMode->SetIntValue( ptrAcquisitionModeContinuous->GetValue() );

		printf("Acquisition mode set to continuous...");
		
		pCam->BeginAcquisition();	// Begin acquiring images

		reCreateBuffer();

		printf("Acquiring images...");

		isCameraStarted = 1;
	}
	return 0;
} // startCamera

/* ��~ */
int BlackflyS::stopCamera(){
	if ( isInitialized ){
		pCam->EndAcquisition();		// End acquisition
		isCameraStarted = 0;
	}
	return 0;
} // stopCamera
