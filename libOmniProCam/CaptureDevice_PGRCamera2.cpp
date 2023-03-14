#include "CaptureDevice_PGRCamera2.h"

/* コンストラクタ */
PGRCamera::PGRCamera() : CaptureDevice(){
	cameraIndex = 0;
	isCameraStarted = 0;
	isCustomParameterInitialized = 0;
	tmpImageHeader = NULL;
	triggerMode.onOff = false;
} // コンストラクタ

/* デストラクタ */
PGRCamera::~PGRCamera(){
} // デストラクタ

/* グラブしたフレームデータのポインタをそのままIPLイメージヘッダにくっつけて渡す */
cv::Mat *PGRCamera::grabImage(){
  if (camera.RetrieveBuffer( &rawImage ) != FlyCapture2::PGRERROR_OK){
		printf("カメラから画像を取得できませんでした。\n");
		return NULL;
  }
	tmpImageHeader->data = (uchar*)rawImage.GetData();
	return tmpImageHeader;
} // grabImage

/* 与えられたイメージデータのimageDataポインタにフレームのポインタを書き込む */
void PGRCamera::grabImage(cv::Mat &imageHeader){
  if (camera.RetrieveBuffer( &rawImage ) != FlyCapture2::PGRERROR_OK){
		printf("カメラから画像を取得できませんでした。\n");
		return;
  }
	imageHeader.data = (uchar*)rawImage.GetData();
} // grabImage

/************************************
						SET
 ************************************/

/* カメラインデックスをセットする */
void PGRCamera::setCameraIndex(int index){
	cameraIndex = index;
} // setCameraIndex

/* カスタムイメージパラメータを全部設定する。
	 ROIのサイズはカメラ毎に制限があるのでカメラのマニュアルを参照 */
int PGRCamera::setCustomImageParameter( int x, int y, int width, int height, FlyCapture2::PixelFormat format, FlyCapture2::Mode mode ){		// 第1引数、第2引数の順番に注意

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

	/* 必要であれば一時イメージヘッダを再作成 */
	if ( tmpImageHeader ){
		if ( tmpImageHeader->cols != width || tmpImageHeader->rows != height ){
			delete tmpImageHeader;
			tmpImageHeader = new Mat(height, width, CV_8UC1);
		}
	}else{
		tmpImageHeader = new Mat(height, width, CV_8UC1);
	}

	/* カメラのスタート後なら再スタートさせる */
	if ( isCameraStarted ){		restartCamera();		}

	isCustomParameterInitialized = 1;

	FlyCapture2::Error error;

  /* Custom Image Format7 Capability情報を得る */
  format7Info.mode = mode;		// mode 0
  error = camera.GetFormat7Info( &format7Info, &supported );
  if (error != FlyCapture2::PGRERROR_OK){
		return -1;
  }
	    
	/* フォーマット・ROIの設定 */
  format7ImageSettings.mode = format7Mode;					// モード0
  format7ImageSettings.offsetX = x;									// ROI X
  format7ImageSettings.offsetY = y;									// ROI Y
  format7ImageSettings.width = width;								// ROI Width
  format7ImageSettings.height = height;							// ROI Height
  format7ImageSettings.pixelFormat = pixelFormat;		// ピクセルフォーマット

  /* Custom Image設定をチェック */
	bool valid;
  error = camera.ValidateFormat7Settings( &format7ImageSettings, &valid, &format7PacketInfo );
  if (error != FlyCapture2::PGRERROR_OK){
		return -1;
  }
  if ( !valid ){
		printf("Format7設定が有効ではありません。\n");
    return -1;
  }

  /* Custom Image設定をカメラに転送 */
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

/* トリガモード */
int PGRCamera::setTriggerMode(unsigned int mode, unsigned int source, bool isRisingEdge, int timeOut, unsigned int parameter){
  camera.GetTriggerMode( &triggerMode );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("現在のTriggerModeが取得できませんでした。\n");
	  return -1;
  }

	/* TriggerMode有効化 */
	triggerMode.onOff = true;
	triggerMode.mode = mode;
	triggerMode.source = source;
	triggerMode.polarity = (unsigned int)isRisingEdge;
	triggerMode.parameter = parameter;

	/* 設定 */
	error = camera.SetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("TriggerModeが設定できませんでした。\n");
		return -1;
	}

	printf("TriggerModeを設定しました: Mode: %u, Source: %u\n", mode, source);

  /* タイムアウトの設定 */
	FlyCapture2::FC2Config config;
  error = camera.GetConfiguration( &config );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("カメラ設定が取得できませんでした。\n");
		return -1;
  }
  config.grabTimeout = timeOut;
	error = camera.SetConfiguration( &config );
  if (error != FlyCapture2::PGRERROR_OK){
		printf("カメラ設定が適用できませんでした。\n");
		return -1;
  }

	return 0;
} // setTriggerMode

/* トリガモード解除 */
int PGRCamera::resetTriggerMode(){

	camera.GetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("現在のTriggerModeが取得できませんでした。\n");
		return -1;
	}

	/* TriggerMode有効化 */
	triggerMode.onOff = false;

	/* 設定 */
	error = camera.SetTriggerMode( &triggerMode );
	if (error != FlyCapture2::PGRERROR_OK){
		printf("TriggerModeが設定できませんでした。\n");
		return -1;
	}

	return 0;
} // resetTriggerMode

/****************************************
					カラープロセス
	****************************************/
void PGRCamera::setColorProcessing(){
} // setColorProcessing

/**********************************
					初期化 / 終了
 **********************************/
/* 初期化 */
int PGRCamera::initialize(){

	if ( !isInitialized ){

		/* カメラ接続数確認 */
		if ( busManager.GetNumOfCameras( &numCameras ) != FlyCapture2::PGRERROR_OK ){
			return -1;
		}
		printf( "%d台のカメラが接続されています。\n", numCameras );

		if ( numCameras < 1 ){
			printf( "カメラが接続されていません。\n" );
			return -1;
		}

		/* GUID取得 */
		error = busManager.GetCameraFromIndex(cameraIndex, &guid);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* カメラ接続 */
		error = camera.Connect(&guid);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* カメラ情報取得 */
		error = camera.GetCameraInfo(&camInfo);
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		/* Custom Image Format7 Capability情報を得る */
		error = camera.GetFormat7Info( &format7Info, &supported );
		if (error != FlyCapture2::PGRERROR_OK){
			return -1;
		}

		roiRect.x = roiRect.y = 0;
		roiRect.width = format7Info.maxWidth;
		roiRect.height = format7Info.maxHeight;

		/* 外部トリガモード解除 */
		resetTriggerMode();

		printf("カメラの初期化が完了しました。\n");

		isInitialized = 1;
		return 0;
	}

	return -1;
} // capIinitialize

/* 初期化 */
int PGRCamera::initialize(int x, int y, int width, int height, FlyCapture2::PixelFormat format){
	setCustomImageParameter(y, x, width, height, format);		// 第1引数、第2引数の順番に注意
	return initialize();
} // capIinitialize

/* 終了 */
void PGRCamera::finalize(){
	if ( isInitialized ){

		if ( isCameraStarted ){
			stopCamera();
		}

		if ( tmpImageHeader ){
			delete tmpImageHeader;
		}

		/* コンテクストの破棄 */
		error = camera.Disconnect();
		if (error != FlyCapture2::PGRERROR_OK){
			return;
		}
		printf("カメラの解放が完了しました。\n");

		isInitialized = 0;
	}
} // finalize

/* カメラの再スタート */
void PGRCamera::restartCamera(){
	stopCamera();
	startCamera();
} // restartCamera

/***********************************
					開始 / 停止
 ***********************************/

/* 開始 */
int PGRCamera::startCamera(){
	if ( isInitialized ){
		if ( camera.StartCapture() != FlyCapture2::PGRERROR_OK ){
			return -1;
		}
		isCameraStarted = 1;
	}
	return 0;
} // startCamera

/* 停止 */
int PGRCamera::stopCamera(){
	if ( isInitialized ){
		if (camera.StopCapture() != FlyCapture2::PGRERROR_OK){
			return -1;
		}   
		isCameraStarted = 0;
	}
	return 0;
} // stopCamera

/* ゲイン */
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

/* ブオブジェクトネス */
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

/* エクスポージャ */
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

/* ガンマ */
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

/* シャッタスピード */
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

/* フレームレート */
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

/* ROIを取得 */
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
