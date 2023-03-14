/*****************************************************************
		2ílâªÉNÉâÉX
		2014/06/11 by ç≤ì°èré˜@UECISMS
 *****************************************************************/

#ifndef __BINARIZER_H__
#define __BINARIZER_H__

#include <opencv2/opencv.hpp>
using namespace cv;

class Binarizer {

public:

	Binarizer();

	virtual ~Binarizer();

	/* èâä˙âª */
	virtual int initialize(int width, int height);

	/* ËáílÇ≈2ílâª */
	virtual void calcThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, double thresholdValue);

	/* AdaptiveThreshold */
	void Binarizer::calcAdaptiveThreshold(cv::Mat& srcImageGRAY, cv::Mat& dstImageGRAY, int blockSize = 51, double cValue = 20.0f);

	/* îwåiç∑ï™ */
	virtual void calcBackgroundSubtraction(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, double thresholdValue);

	/* îwåiç∑ï™ */
	virtual void calcBackgroundSubtractionAdaptive(cv::Mat& srcImageGRAY, cv::Mat& dstBinarizedImage, cv::Mat& backgroundImageGRAY, int blockSize = 51, double cValue = 20.0f);

protected:

	cv::Mat* tmpImage;

};

#endif