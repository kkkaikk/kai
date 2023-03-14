#include "libOmniProCam.h"

/* 初期化 */
PGRCVDLL_API int initialize(int roiX, int roiY, int roiWidth, int roiHeight, int maxRegionSize, int minRegionSize, int maxRegions) {

	SDL_Init(SDL_INIT_TIMER);
	timer = Timer::getInstance();
	timer->initialize();
	maxObjects = maxRegions;
	maxSizeThreshold = maxRegionSize;
	minSizeThreshold = minRegionSize;

	/* カメラ */
	if (cameraSDKType == CAMERA_SDK_TYPE_FLYCAPTURE2) {

		pgrCamera = new PGRCamera();
		if (pgrCamera->initialize() < 0) {
			return -1;
		}
		if (pgrCamera->setCustomImageParameter(roiX, roiY, roiWidth, roiHeight, FlyCapture2::PixelFormat::PIXEL_FORMAT_RAW8) < 0) {
			return -2;
		}
		if (pgrCamera->startCamera() < 0) {
			return -3;
		}
		roi = pgrCamera->getROI();
		currentCaptureDevicePtr = pgrCamera;

	} else 	if (cameraSDKType == CAMERA_SDK_TYPE_SPINNAKER) {
		blackflyS = new BlackflyS();
		if (blackflyS->initialize() < 0) {
			printf("カメラの初期化に失敗しました。\n");
			return -1;
		}
		//blackflyS->setMaxROI();
		blackflyS->setROI(roiX, roiY, roiWidth, roiHeight);
		//blackflyS->setShutter(2000);		// 2msec
		if (blackflyS->startCamera() < 0) {
			return -2;
		}
		roi = blackflyS->getROI();
		currentCaptureDevicePtr = blackflyS;
	
	/* カメラを使わない */
	} else {
		pgrCamera = NULL;
		blackflyS == NULL;
		currentCaptureDevicePtr = NULL;
		roi.x = roi.y = roi.width = roi.height = 0;
		return 0;
	}

	/* 中間画像ファイル初期化 */
	srcImageRGBAForUnity = cv::Mat::zeros(roi.height, roi.width, CV_8UC4);

	binarizedImageRGBAForUnity = cv::Mat::zeros(roi.height, roi.width, CV_8UC4);

	//srcImageGRAY = Mat::zeros(roi.height, roi.width, CV_8UC1);
	srcImageRGBAForUnity = Mat::zeros(roi.height, roi.width, CV_8UC4);

	backgroundImageGRAY = Mat::zeros(roi.height, roi.width, CV_8UC1);
	binarizedImageGRAY = Mat::zeros(roi.height, roi.width, CV_8UC1);
	binarizedImageRGBAForUnity = Mat::zeros(roi.height, roi.width, CV_8UC4);
	binarizedImageCopyGRAY = Mat::zeros(roi.height, roi.width, CV_8UC1);

	binarizingThreshold = 50;

	/* オブジェクトオブジェクト初期化 */
	//detectedLightObjectDataArray.resize(maxObjects);
	binarizer.initialize(roi.width, roi.height);
	resultRegionArray.resize(maxObjects);
	//for (int i = 0; i < maxObjects; i++) {
	//	detectedLightObjectDataArray[i].id = i + 1;
	//	detectedLightObjectDataArray[i].centroid.x = 0;
	//	detectedLightObjectDataArray[i].centroid.y = 0;
	//	detectedLightObjectDataArray[i].size = 0;
	//}

	objectTracker.initialize(maxObjects);
	trackedObjectArrayPtr = objectTracker.getTrackingObjectArrayPtr();

	/* Mask画像読み込み */
	maskImageGRAY = cv::imread("./data/image/maskImageGRAY.bmp", 0);
	if (maskImageGRAY.data == NULL) {
		printf("マスク画像が見つかりません: ./data/image/maskImageGRAY.bmp\n");
		maskImageGRAY = Mat::ones(roi.height, roi.width, CV_8UC1);
	}

	/* ネットワークの初期化 */
	SocketUDP::initializeSocket();
	udpClient = SocketUDP::createClient("localhost", 31416);

	sendBuffer.value = 123;

	return 0;
} // initialize

/* 順番点灯 */
PGRCVDLL_API int process() {

	timer->update();

	if (cameraSDKType == CAMERA_SDK_TYPE_NO_CAMERA) {
	
		calcFPS();
		return 0;

	} else {

		/* 画像取得 */
		srcImageGRAY = currentCaptureDevicePtr->grabImage();

		/* 2値化 */
		binarizer.calcThreshold(*srcImageGRAY, binarizedImageGRAY, binarizingThreshold);
		//inarizer.calcBackgroundSubtraction(*srcImageGRAY, binarizedImageGRAY, backgroundImageGRAY, binarizingThreshold);

		/* 予測モデルの際に使用 */
		 /* ラベリング */
		 //binarizedImageGRAY.copyTo(binarizedImageCopyGRAY);
		 //regionCounter = regionLabeler.detectRegion(binarizedImageCopyGRAY,
			//maxSizeThreshold, minSizeThreshold, resultRegionArray);

		 /* トラッキング */
		 //regionCounter = objectTracker.trackObjects(resultRegionArray, regionCounter);

		/* FPS集計 */
		calcFPS();

		return regionCounter;
	}

} // processImageSequential

/* バッファリリース(Spinnaker Only) */
PGRCVDLL_API void releaseCameraImageBuffer() {
	blackflyS->releaseImageBuffer();
} // releaseCameraImageBuffer

/* 背景初期化 */
PGRCVDLL_API void resetBackground() {
	srcImageGRAY->copyTo(backgroundImageGRAY);
	printf("Backgound image has been captured!\n");
} // resetBackground
PGRCVDLL_API void saveBackgroundToFile() {
	cv::imwrite("./data/conf/background.bmp", backgroundImageGRAY);
	printf("Backgound image has been saved!\n");
} // saveBackgroundToFile
PGRCVDLL_API void loadBackgroundFromFile() {
	backgroundImageGRAY = imread("./data/conf/background.bmp", IMREAD_GRAYSCALE);
	printf("Backgound image has been loaded!\n");
} // loadBackgroundFromFile


/*****************************************************************
														受け渡し
*****************************************************************/

/* 結果を返す */
PGRCVDLL_API void getObjectData(OmniProCamData *resultDataArray) {
	static vector<Point2f> srcPoint(1), dstPoint(1);
	for (int i = 0; i < regionCounter; i++) {
		resultDataArray[i].id = (*trackedObjectArrayPtr)[i].id;
		resultDataArray[i].x = (*trackedObjectArrayPtr)[i].x;
		resultDataArray[i].y = (*trackedObjectArrayPtr)[i].y;
		resultDataArray[i].size = (*trackedObjectArrayPtr)[i].size;
		resultDataArray[i].boundingBoxX = (*trackedObjectArrayPtr)[i].boundingBox.x;
		resultDataArray[i].boundingBoxY = (*trackedObjectArrayPtr)[i].boundingBox.y;
		resultDataArray[i].boundingBoxWidth = (*trackedObjectArrayPtr)[i].boundingBox.width;
		resultDataArray[i].boundingBoxHeight = (*trackedObjectArrayPtr)[i].boundingBox.height;
	}
} // getObjectData

/* カメラ設定 */
PGRCVDLL_API void setShutterSpeed(int shutterSpeedUSec) {
	if (cameraSDKType == CAMERA_SDK_TYPE_FLYCAPTURE2) {
		pgrCamera->setShutterSpeed(shutterSpeedUSec / 1000);
	} else {
		blackflyS->setShutter(shutterSpeedUSec);
	}
} // setShutterSpeed

PGRCVDLL_API void setGain(float gain) {
	if (cameraSDKType == CAMERA_SDK_TYPE_FLYCAPTURE2) {
		pgrCamera->setGain(gain);
	} else {
		blackflyS->setGain(gain);
	}
} // setGain

	/********************************
			カメラ画像をコピーする
	********************************/

/* カメラ画像の取得 */
void getSrcImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC1, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	srcImageGRAY->copyTo(roiTmpMat);
} // getSrcImageGRAY

void getBinarizedImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC1, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	binarizedImageGRAY.copyTo(roiTmpMat);
} // getBinarizedImageGRAY

void getMaskedSrcImageGRAY(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC1, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	maskImageWithHandShapeGRAY.copyTo(roiTmpMat);
} // getMaskedSrcImageGRAY

	/*********************************************
				カメラ画像をコピーする(for Unity)
	*********************************************/

/* カメラ画像の取得 */
void getSrcImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC4, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	cvtColor(*srcImageGRAY, srcImageRGBAForUnity, CV_GRAY2RGBA);
	srcImageRGBAForUnity.copyTo(roiTmpMat);
} // getSrcImageRGBAForUnity

/* カメラ画像の取得 */
void getBinarizedImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC4, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	cvtColor(binarizedImageGRAY, binarizedImageRGBAForUnity, CV_GRAY2RGBA);
	binarizedImageRGBAForUnity.copyTo(roiTmpMat);
} // getSrcImageRGBAForUnity

/* マスク付きカメラ画像の取得 */
void getMaskedSrcImageRGBAForUnity(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight) {
	cv::Mat tmpMat = Mat(dstImageHeight, dstImagewidth, CV_8UC4, dstImagePixelsPtr);
	cv::Mat roiTmpMat = tmpMat(Rect(0, 0, roi.width, roi.height));
	cvtColor(maskImageWithHandShapeGRAY, maskedSrcImageRGBAForUnity, CV_GRAY2RGBA);
	maskedSrcImageRGBAForUnity.copyTo(roiTmpMat);
} // getMaskedSrcImageRGBAForUnity

/* 解放 */
PGRCVDLL_API void finalize() {

	delete udpClient;
	SocketUDP::finalizeSocket();

	if (cameraSDKType == CAMERA_SDK_TYPE_FLYCAPTURE2) {
		pgrCamera->stopCamera();
		pgrCamera->finalize();
		delete pgrCamera;
	} else if (cameraSDKType == CAMERA_SDK_TYPE_SPINNAKER) {
		blackflyS->stopCamera();
		blackflyS->finalize();
		delete blackflyS;
	}

	binarizedImageCopyGRAY.release();
	binarizedImageGRAY.release();
	binarizedImageRGBAForUnity.release();
	//srcImageGRAY.release();
	srcImageRGBAForUnity.release();

	SDL_Quit();
} // finalize

/* FPSを返す */
PGRCVDLL_API int getFPS() {
	return fps;
} // getFPS

/* 2値化閾値のセット */
PGRCVDLL_API void setBinarizingThreshold(float threshold) {
	binarizingThreshold = threshold;
} // setBinarizingThreshold

/* 2値化閾値のセット */
PGRCVDLL_API void setAdaptiveBinarizingThreshold(int blockSize, double cValue) {
	adaptiveThresholdBlockSize = blockSize;
	adaptiveThresholdCValue = cValue;
} // setAdaptiveBinarizingThreshold

/* 面積閾値のセット */
PGRCVDLL_API void setSizeThreshold(float min, float max) {
	maxSizeThreshold = max;
	minSizeThreshold = min;
} // setSizeThreshold

/* SDLタイプの設定 */
PGRCVDLL_API void setCameraSDKType(int type) {
	if (type == CAMERA_SDK_TYPE_FLYCAPTURE2 || type == CAMERA_SDK_TYPE_SPINNAKER) {
		cameraSDKType = (CAMERA_SDK_TYPE)type;
	} else {
		cameraSDKType = CAMERA_SDK_TYPE_NO_CAMERA;
	}
} // setCameraSDKType

PGRCVDLL_API int getPixelLuminance(int x, int y) {
	return srcImageGRAY->data[y * binarizedImageGRAY.step + x];
} // getPixelLuminance

PGRCVDLL_API int getImageBrightness() {

	if (maskImageGRAY.data != NULL) {
		cv::subtract(maskImageGRAY, binarizedImageGRAY, maskImageWithHandShapeGRAY);
		//imwrite("mask.bmp", maskImageCopyGRAY);
		return cv::mean(*srcImageGRAY, maskImageWithHandShapeGRAY)[0];
	}else {
		return cv::mean(*srcImageGRAY)[0];
	}
} // getImageBrightness

/* 送信 */
PGRCVDLL_API int sendData(int channel, int index, int value) {
	sendBuffer.index = index;
	sendBuffer.channel = channel;
	sendBuffer.value = value;
	return udpClient->send(&sendBuffer, sizeof(NetworkData));
} // Processingにデータ送信　sendData

PGRCVDLL_API void saveCameraImage(){
	cv::imwrite("./data/image/cameraImageGRAY.bmp", *srcImageGRAY);
} // saveCameraImage

PGRCVDLL_API float getTotalSize() {
	return totalSize;
} // getTotalSize