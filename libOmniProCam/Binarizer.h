/*****************************************************************
		2�l���N���X
		2014/06/11 by �����r��@UECISMS
 *****************************************************************/

#ifndef __BINARIZER_H__
#define __BINARIZER_H__

#include <opencv2/opencv.hpp>
using namespace cv;

class Binarizer {

public:

	Binarizer();

	virtual ~Binarizer();

	/* ������ */
	virtual int initialize(int width, int height);

	/* 臒l��2�l�� */
	virtual void calcThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, double thresholdValue);

	/* AdaptiveThreshold */
	void Binarizer::calcAdaptiveThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, int blockSize = 51, double cValue = 20.0f);

	/* �w�i���� */
	virtual void calcBackgroundSubtraction(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, double thresholdValue);

	/* �w�i���� */
	virtual void calcBackgroundSubtractionAdaptive(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, int blockSize = 51, double cValue = 20.0f);

protected:

	cv::Mat* tmpImage;

};

#endif