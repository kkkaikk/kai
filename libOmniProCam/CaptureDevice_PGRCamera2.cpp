#include "CaptureDevice_PGRCamera2.h"

/* �R���X�g���N�^ */
PGRCamera::PGRCamera() : CaptureDevice(){
	cameraIndex = 0;
	isCameraStarted = 0;
	isCustomParameterInitialized = 0;
	tmpImageHeader = NULL;
	triggerMode.onOff = false;
} // �R���X�g���N�^

/* �f�X�g���N�^ */
PGRCamera::~PGRCamera(){
} // �f�X�g���N�^

/* �O���u�����t���[���f�[�^�̃|�C���^�����̂܂�IPL�C���[�W�w�b�_�ɂ������ēn�� */
cv::Mat *PGRCamera::grabImage(){
  if (camera.RetrieveBuffer( &rawImage ) != FlyCapture2::PGRERROR_OK){
		printf("�J��������摜���擾�ł��܂���ł����B\n");
		return NULL;
  }
	tmpImageHeader->data = (uchar*)rawImage.GetData();
	return tmpImageHeader;
} // grabImage

/* �^����ꂽ�C���[�W�f�[�^��imageData�|�C���^�Ƀt���[���̃|�C���^���������� */
void PGRCamera::grabImage(cv::Mat &imageHeader){
  if (camera.RetrieveBuffer( &rawImage ) != FlyCapture2::PGRERROR_OK){
		printf("�J��������摜���擾�ł��܂���ł����B\n");
		return;
  }
	imageHeader.data = (uchar*)rawImage.GetData();
} // grabImage

/************************************
						SET
 ************************************/

/* �J�����C���f�b�N�X���Z�b�g���� */
void PGRCamera::setCameraIndex(int index){
	cameraIndex = index;
} // setCameraIndex

/* �J�X�^���C���[�W�p�����[�^��S���ݒ肷��B
	 ROI�̃T�C�Y�̓J�������ɐ���������̂ŃJ�����̃}�j���A�����Q�� */
int PGRCamera::setCustomImageParameter( int x, int y, int width, int height, FlyCapture2::PixelFormat format, FlyCapture2::Mode mode ){		// ��1�����A��2�����̏��Ԃɒ���

	static int depth = IPL_DEPTH_8U, channel = 1;
	customImageParam.format = format;
	pixelFormat = format;
	format7Mode = mode;

	if ( format == FlyCapture2::PIXEL_FORMAT_RAW8 ){
		channel = 1;
		depth = IPL_DEPTH_8U;

	}else if ( format == FlyCapture2::PIXEL_FORMAT_MONO8 ){
		channel = 1;
		depth = IPL_DEPTH_8U;

	}else if ( format == FlyCapture2::PIXEL_FORMAT_MONO16 ){
		channel = 1;
		depth = IPL_DEPTH_16U;

	}else if ( format == FlyCapture2::PIXEL_FORMAT_RAW16 ){
		channel = 1;
		depth = IPL_DEPTH_16U;

	}else if ( format == FlyCapture2::PIXEL_FORMAT_BGR || format == FlyCapture2::PIXEL_FORMAT_RGB || format == FlyCapture2::PIXEL_FORMAT_RGB8 ){
		channel = 3;
		depth = IPL_DEPTH_8U;
	}

	/* �K�v�ł���Έꎞ�C���[�W�w�b�_���č쐬 */
	if ( tmpImageHeader ){
		if ( tmpImageHeader->cols != width || tmpImageHeader->rows != height ){
			delete tmpImageHeader;
			tmpImageHeader = new Mat(height, width, CV_8UC1);
		}
	}else{
		tmpImageHeader = new Mat(height, width, CV_8UC1);
	}

	/* �J�����̃X�^�[�g��Ȃ�ăX�^�[�g������ */
	if ( isCameraStarted ){		restartCamera();		}

	isCustomParameterInitialized = 1;

	FlyCapture2::Error error;

  /* Custom Image Format7 Capability���𓾂� */
  format7Info.mode = mode;		// mode 0
  error = camera.GetFormat7Info( &format7Info, &supported );
  if (error != FlyCapture2::PGRERROR_OK){
		return -1;
  }
	    
	/* �t�H�[�}�b�g�EROI�̐ݒ� */
  format7ImageSettings.mode = format7Mode;					// ���[�h0
  format7ImageSettings.offsetX = x;									// ROI X
  format7ImageSettings.offsetY = y;									// ROI Y
  format7ImageSettings.width = width;								// ROI Width
  format7ImageSettings.height = height;							// ROI Height
  format7ImageSettings.pixelFormat = pixelFormat;		// �s�N�Z���t�H�[�}�b�g

  /* Custom Image�ݒ���`�F�b�N */
	bool valid;
  error = camera.ValidateFormat7Settings( &format7ImageSettings, &valid, &format7PacketInfo );
  if (error != FlyCapture2::PGRERROR_OK){
		return -1;
  }
  if ( !valid ){
		printf("Format7�ݒ肪�L���ł͂���܂���B\n");
    return -1;
  }

  /* Custom Image�ݒ���J�����ɓ]�� */
  error = camera.SetFormat7Configuration(&format7ImageSettings, format7PacketInfo.recommendedBytesPerPacket);
  if (error != FlyCapture2::PGRERROR_OK){
     return -1;
  }

	roiRect.x = x;
	roiRect.y = y;
	roiRect.width = width;
	roiRect.height = height;

	return 0;
} // setCustomImageParameter

/* �g���K���[�h */
int PGRCamera::setTriggerMode(unsigned int mode, unsigned int source, bool isRisingEdge, int timeOut, unsigned int parameter){
  camera.GetTriggerMode( &triggerMode );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("���݂�TriggerMode���擾�ł��܂���ł����B\n");
	  return -1;
  }

	/* TriggerMode�L���� */
	triggerMode.onOff = true;
	triggerMode.mode = mode;
	triggerMode.source = source;
	triggerMode.polarity = (unsigned int)isRisingEdge;
	triggerMode.parameter = parameter;

	/* �ݒ� */
	error = camera.SetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("TriggerMode���ݒ�ł��܂���ł����B\n");
		return -1;
	}

	printf("TriggerMode��ݒ肵�܂���: Mode: %u, Source: %u\n", mode, source);

  /* �^�C���A�E�g�̐ݒ� */
	FlyCapture2::FC2Config config;
  error = camera.GetConfiguration( &config );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("�J�����ݒ肪�擾�ł��܂���ł����B\n");
		return -1;
  }
  config.grabTimeout = timeOut;
	error = camera.SetConfiguration( &config );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("�J�����ݒ肪�K�p�ł��܂���ł����B\n");
		return -1;
  }

	return 0;
} // setTriggerMode

/* �g���K���[�h���� */
int PGRCamera::resetTriggerMode(){

	camera.GetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("���݂�TriggerMode���擾�ł��܂���ł����B\n");
		return -1;
	}

	/* TriggerMode�L���� */
	triggerMode.onOff = false;

	/* �ݒ� */
	error = camera.SetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("TriggerMode���ݒ�ł��܂���ł����B\n");
		return -1;
	}

	return 0;
} // resetTriggerMode

/****************************************
					�J���[�v���Z�X
	****************************************/
void PGRCamera::setColorProcessing(){
} // setColorProcessing

/**********************************
					������ / �I��
 **********************************/
/* ������ */
int PGRCamera::initialize(){

	if ( !isInitialized ){

		/* �J�����ڑ����m�F */
		if ( busManager.GetNumOfCameras( &numCameras ) != FlyCapture2::PGRERROR_OK ){
			return -1;
		}
		printf( "%d��̃J�������ڑ�����Ă��܂��B\n", numCameras );

		if ( numCameras < 1 ){
			printf( "�J�������ڑ�����Ă��܂���B\n" );
			return -1;
		}

		/* GUID�擾 */
		error = busManager.GetCameraFromIndex(cameraIndex, &guid);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* �J�����ڑ� */
		error = camera.Connect(&guid);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* �J�������擾 */
		error = camera.GetCameraInfo(&camInfo);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* Custom Image Format7 Capability���𓾂� */
		error = camera.GetFormat7Info( &format7Info, &supported );
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		roiRect.x = roiRect.y = 0;
		roiRect.width = format7Info.maxWidth;
		roiRect.height = format7Info.maxHeight;

		/* �O���g���K���[�h���� */
		resetTriggerMode();

		printf("�J�����̏��������������܂����B\n");

		isInitialized = 1;
		return 0;
	}

	return -1;
} // capIinitialize

/* ������ */
int PGRCamera::initialize(int x, int y, int width, int height, FlyCapture2::PixelFormat format){
	setCustomImageParameter(y, x, width, height, format);		// ��1�����A��2�����̏��Ԃɒ���
	return initialize();
} // capIinitialize

/* �I�� */
void PGRCamera::finalize(){
	if ( isInitialized ){

		if ( isCameraStarted ){
			stopCamera();
		}

		if ( tmpImageHeader ){
			delete tmpImageHeader;
		}

		/* �R���e�N�X�g�̔j�� */
		error = camera.Disconnect();
		if (error != FlyCapture2::PGRERROR_OK){
			return;
		}
		printf("�J�����̉�����������܂����B\n");

		isInitialized = 0;
	}
} // finalize

/* �J�����̍ăX�^�[�g */
void PGRCamera::restartCamera(){
	stopCamera();
	startCamera();
} // restartCamera

/***********************************
					�J�n / ��~
 ***********************************/

/* �J�n */
int PGRCamera::startCamera(){
	if ( isInitialized ){
		if ( camera.StartCapture() != FlyCapture2::PGRERROR_OK ){
			return -1;
		}
		isCameraStarted = 1;
	}
	return 0;
} // startCamera

/* ��~ */
int PGRCamera::stopCamera(){
	if ( isInitialized ){
		if (camera.StopCapture() != FlyCapture2::PGRERROR_OK){
			return -1;
		}   
		isCameraStarted = 0;
	}
	return 0;
} // stopCamera

/* �Q�C�� */
void PGRCamera::setGain(float gainValue){
	if ( gainValue > 30.0f ){
		gainValue = 30.0f;
	}else if ( gainValue < -6.0f ){
		gainValue = -6.0f;
	}
	FlyCapture2::Property gainProperty;
	gainProperty.type = FlyCapture2::GAIN;
	gainProperty.absValue = gainValue;
	gainProperty.autoManualMode = false;
	gainProperty.absControl = true;
	gainProperty.onOff = true;
	camera.SetProperty(&gainProperty, false );
} // setGain

/* �u�I�u�W�F�N�g�l�X */
void PGRCamera::setBrightness(float brightnessValue){
	if ( brightnessValue > 6.23f ){
		brightnessValue = 6.23f;
	}else if ( brightnessValue < 0.0f ){
		brightnessValue = 0.0f;
	}
	FlyCapture2::Property brightnessProperty;
	brightnessProperty.type = FlyCapture2::BRIGHTNESS;
	brightnessProperty.absValue = brightnessValue;
	brightnessProperty.autoManualMode = false;
	brightnessProperty.absControl = true;
	brightnessProperty.onOff = true;
	camera.SetProperty(&brightnessProperty, false );
} // setBrightness

/* �G�N�X�|�[�W�� */
void PGRCamera::setExposure(float exposureValue){
	if ( exposureValue > 2.41f ){
		exposureValue = 2.41f;
	}else if ( exposureValue < -7.58f ){
		exposureValue = -7.58f;
	}
	FlyCapture2::Property exposureProperty;
	exposureProperty.type = FlyCapture2::AUTO_EXPOSURE;
	exposureProperty.absValue = exposureValue;
	exposureProperty.autoManualMode = false;
	exposureProperty.absControl = true;
	exposureProperty.onOff = true;
	camera.SetProperty(&exposureProperty, false );
} // setExposure

/* �K���} */
void PGRCamera::setGamma(float gammaValue){
	if ( gammaValue > 4.0f ){
		gammaValue = 4.0f;
	}else if ( gammaValue < 0.5f ){
		gammaValue = 0.5f;
	}
	FlyCapture2::Property gammaProperty;
	gammaProperty.type = FlyCapture2::GAMMA;
	gammaProperty.absValue = gammaValue;
	gammaProperty.autoManualMode = false;
	gammaProperty.absControl = true;
	gammaProperty.onOff = true;
	camera.SetProperty(&gammaProperty, false );
} // setGamma

/* �V���b�^�X�s�[�h */
void PGRCamera::setShutterSpeed(float shutterValue){
	if ( shutterValue > 66.64f ){
		shutterValue = 66.64f;
	}else if ( shutterValue < 0.02f ){
		shutterValue = 0.02f;
	}
	FlyCapture2::Property shutterSpeedProperty;
	shutterSpeedProperty.type = FlyCapture2::SHUTTER;
	shutterSpeedProperty.absValue = shutterValue;
	shutterSpeedProperty.autoManualMode = false;
	shutterSpeedProperty.absControl = true;
	shutterSpeedProperty.onOff = true;
	camera.SetProperty(&shutterSpeedProperty, false );
} // setShutterSpeed

/* �t���[�����[�g */
void PGRCamera::setFrameRate(float frameRateValue){
	if ( frameRateValue > 500.0f ){
		frameRateValue = 500.0f;
	}else if ( frameRateValue < 1.0f ){
		frameRateValue = 1.0f;
	}
	FlyCapture2::Property frameRateProperty;
	frameRateProperty.type = FlyCapture2::FRAME_RATE;
	frameRateProperty.absValue = frameRateValue;
	frameRateProperty.autoManualMode = false;
	frameRateProperty.absControl = true;
	frameRateProperty.onOff = true;
	camera.SetProperty(&frameRateProperty, false );
} // setFrameRate

/* ROI���擾 */
CvRect PGRCamera::getROI(){
	return roiRect;
} // getROI

/* Image Size/Offset Units */
int PGRCamera::getImageSizeUnit(){
	return 4;
} // getImageSizeUnit

int PGRCamera::getImageOffsetUnit(){
	return 4;
} // getImageOffsetUnit

int PGRCamera::getMaxCameraWidth(){
	return 640;
} // getMaxCameraWidth

int PGRCamera::getMaxCameraHeight(){
	return 480;
} // getMaxCameraHeight
