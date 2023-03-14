/*****************************************************************
		トラッキングクラス
		2014/06/17 by 佐藤俊樹
 *****************************************************************/
#ifndef __OBJECT_TRACKER_H__
#define __OBJECT_TRACKER_H__

#include <opencv2/opencv.hpp>

#include <Windows.h>

#include "./Timer.h"
#include "RegionLabeler.h"

#include <vector>
using namespace std;

typedef struct {
	unsigned int id;
	unsigned int lastUpdatedTime;
	float x, y;
	float size;
	float angle;
	Rect boundingBox;
	vector<Point> shape;
} TrackingObject;

class ObjectTracker{

public:

	ObjectTracker();	// maxObjects: 配列の最大個数

	virtual ~ObjectTracker();

	/* 初期化 */
	virtual int initialize(int maxObjects){
		trackingObjectArray.resize(maxObjects);
		for (int i = 0; i < maxObjects; i++){
			trackingObjectArray[i].id = trackingObjectArray[i].lastUpdatedTime = 0;
			trackingObjectArray[i].x = trackingObjectArray[i].y = trackingObjectArray[i].size = 0.0f;
		}
		return 0;
	} // initialize

	/****************************************************
			入力配列: Region配列
			出力: TrackingObject配列および有効TrackingObjectの個数
	 ****************************************************/
	virtual int trackObjects(const vector<Region> &srcRegionArray, unsigned int detectedRegionCount);

	/* 配列ポインタを返す */
	virtual vector<TrackingObject> *getTrackingObjectArrayPtr(){
		return &trackingObjectArray;
	} // getTrackingObjectArrayPtr

	/* 閾値の設定 */
	void setTrackingDistanceThreshold(float threshold){
		trackingDistanceThreshold = threshold;
	} // setTrackingDistanceThreshold

protected:

	vector<TrackingObject> trackingObjectArray;
	float trackingDistanceThreshold;
	unsigned int deleteWaitingTime;

	Timer *timer;

};

#endif