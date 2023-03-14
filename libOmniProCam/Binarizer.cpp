#include "Binarizer.h"

Binarizer::Binarizer() {
	tmpImage = NULL;
}

Binarizer::~Binarizer() {
	delete tmpImage;
}

/* ������ */
int Binarizer::initialize(int width, int height) {
	if (tmpImage) {
		delete tmpImage;
		tmpImage = new cv::Mat(height, width, CV_8UC1);
	}
	else {
		tmpImage = new cv::Mat(height, width, CV_8UC1);
	}
	return 0;
} // initialize

/* 臒l��2�l�� */
void Binarizer::calcThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, double thresholdValue) {
	threshold(srcImageGRAY, dstImageGRAY, thresholdValue, 255.0, THRESH_BINARY);
} // calcThresohld

/* AdaptiveThreshold */
void Binarizer::calcAdaptiveThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, int blockSize, double cValue) {
	adaptiveThreshold(srcImageGRAY, dstImageGRAY, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, blockSize, cValue);
} // calcAdaptiveThreshold

/* �w�i���� */
void Binarizer::calcBackgroundSubtraction(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, double thresholdValue) {
	absdiff(srcImageGRAY, backgroundImageGRAY, *tmpImage);
	threshold(*tmpImage, dstBinarizedImage, thresholdValue, 255.0, THRESH_BINARY);
} // calcBackgroundSubtraction

/* �w�i���� */
void Binarizer::calcBackgroundSubtractionAdaptive(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, int blockSize, double cValue) {
	absdiff(srcImageGRAY, backgroundImageGRAY, *tmpImage);
	//subtract(srcImageGRAY, backgroundImageGRAY, *tmpImage);
	adaptiveThreshold(*tmpImage, dstBinarizedImage, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, cValue);
	//�����Ƃ��ƃR�����g�A�E�g����Ă������
	//adaptiveThreshold(*tmpImage, dstBinarizedImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, blockSize, cValue);
} // calcBackgroundSubtractionAdaptive
