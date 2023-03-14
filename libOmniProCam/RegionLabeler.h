/*****************************************************************
		�ȈՃ��x�����O�N���X
		2014/06/11 by �����r��@TokyoTech
 *****************************************************************/

#ifndef __REGIONLABELER_H__
#define __REGIONLABELER_H__

#include <vector>
using namespace std;
#include <opencv2/opencv.hpp>
using namespace cv;

/* ���x�����O���ʂ̃f�[�^�\�� */
typedef struct{	
	float size;							// �T�C�Y
	Point2f centroid;				// �d�S
	Rect boundingRect;			// ��܋�`
	float angle;						// �����厲�p
	vector<Point> shape;		// �֊s���
	vector<Point> *contourPtr;
	int contoursIndex;			// contours[]�̃C���f�b�N�X
} Region;

class RegionLabeler{

public:

	RegionLabeler();

	virtual ~RegionLabeler();

	/* ���x�����O */
	virtual int detectRegion(cv::Mat &srcImageGRAY, float maxSizeThreshold, float minSizeThreshold, 
		vector<Region> &resultRegionArray, bool enableShapeDetection = false);

	/* �֊s�z���Ԃ� */
	virtual vector<vector<Point>>* getContours() ;
	
protected:

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
};

#endif