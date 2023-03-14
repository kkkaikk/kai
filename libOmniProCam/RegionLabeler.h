/*****************************************************************
		簡易ラベリングクラス
		2014/06/11 by 佐藤俊樹@TokyoTech
 *****************************************************************/

#ifndef __REGIONLABELER_H__
#define __REGIONLABELER_H__

#include <vector>
using namespace std;
#include <opencv2/opencv.hpp>
using namespace cv;

/* ラベリング結果のデータ構造 */
typedef struct{	
	float size;							// サイズ
	Point2f centroid;				// 重心
	Rect boundingRect;			// 包含矩形
	float angle;						// 慣性主軸角
	vector<Point> shape;		// 輪郭情報
	vector<Point> *contourPtr;
	int contoursIndex;			// contours[]のインデックス
} Region;

class RegionLabeler{

public:

	RegionLabeler();

	virtual ~RegionLabeler();

	/* ラベリング */
	virtual int detectRegion(cv::Mat &srcImageGRAY, float maxSizeThreshold, float minSizeThreshold, 
		vector<Region> &resultRegionArray, bool enableShapeDetection = false);

	/* 輪郭配列を返す */
	virtual vector<vector<Point>>* getContours() ;
	
protected:

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
};

#endif