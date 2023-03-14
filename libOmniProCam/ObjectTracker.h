/*****************************************************************
		�g���b�L���O�N���X
		2014/06/17 by �����r��
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

	ObjectTracker();	// maxObjects: �z��̍ő��

	virtual ~ObjectTracker();

	/* ������ */
	virtual int initialize(int maxObjects){
		trackingObjectArray.resize(maxObjects);
		for (int i = 0; i < maxObjects; i++){
			trackingObjectArray[i].id = trackingObjectArray[i].lastUpdatedTime = 0;
			trackingObjectArray[i].x = trackingObjectArray[i].y = trackingObjectArray[i].size = 0.0f;
		}
		return 0;
	} // initialize

	/****************************************************
			���͔z��: Region�z��
			�o��: TrackingObject�z�񂨂�їL��TrackingObject�̌�
	 ****************************************************/
	virtual int trackObjects(const vector<Region> &srcRegionArray, unsigned int detectedRegionCount);

	/* �z��|�C���^��Ԃ� */
	virtual vector<TrackingObject> *getTrackingObjectArrayPtr(){
		return &trackingObjectArray;
	} // getTrackingObjectArrayPtr

	/* 臒l�̐ݒ� */
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