/*****************************************************************************************
			libOmniProCam
			2018/10/19 SATOToshiki@TokyoTech
*****************************************************************************************/

#pragma once
#ifdef PGRCVDLL
#define PGRCVDLL_API __declspec(dllexport)

#include "Definition.h"

#else
#define PGRCVDLL_API __declspec(dllimport)
#endif

/* �O���Ƃ���肷��f�[�^�`��(Unity�p) */
typedef struct {
	unsigned int id;
	float x;
	float y;
	float size;
	float boundingBoxX;
	float boundingBoxY;
	float boundingBoxWidth;
	float boundingBoxHeight;
} OmniProCamData;

extern "C" {

	/* �J����SDK�^�C�v�̐ݒ�(initialize()����ɌĂ�) */
	PGRCVDLL_API void setCameraSDKType(int type);

	/* ������ */
	PGRCVDLL_API int initialize(int roiX, int roiY, int roiWidth, int roiHeight, int maxRegions, int maxRegionSize, int minRegionSize);

	/* ����(���[�v�ɓ����) */
	PGRCVDLL_API int process();

	/* ��� */
	PGRCVDLL_API void finalize();

	/* ���ʂ�Ԃ� */
	PGRCVDLL_API void getObjectData(OmniProCamData *resultDataArray);

	/* FPS��Ԃ� */
	PGRCVDLL_API int getFPS();

	/********************************
			�J�����摜���R�s�[����
	********************************/
	PGRCVDLL_API void getSrcImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getBinarizedImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getMaskedSrcImageGRAY(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

	/*********************************************
			�J�����摜���R�s�[����(for Unity)
	*********************************************/
	PGRCVDLL_API void getSrcImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getBinarizedImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getMaskedSrcImageRGBAForUnity(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

	/********************************
			�ݒ�
	********************************/

	/* �J�����ݒ� */
	PGRCVDLL_API void setShutterSpeed(int shutterSpeedUSec);
	PGRCVDLL_API void setGain(float gain);

	/* �ʐ�臒l�̃Z�b�g */
	PGRCVDLL_API void setSizeThreshold(float min, float max);

	/* 2�l��臒l�̃Z�b�g */
	PGRCVDLL_API void setBinarizingThreshold(float threshold);

	/* 2�l��臒l�̃Z�b�g */
	PGRCVDLL_API void setAdaptiveBinarizingThreshold(int blockSize, double cValue);

	/* �o�b�t�@�����[�X */
	PGRCVDLL_API void releaseCameraImageBuffer();

	/* �w�i�Ď擾 */
	PGRCVDLL_API void resetBackground();

	/* �w�i�ۑ� */
	PGRCVDLL_API void saveBackgroundToFile();
	PGRCVDLL_API void loadBackgroundFromFile(); 

	/* �s�N�Z���P�x�l�̎擾 */
	PGRCVDLL_API int getPixelLuminance(int x, int y);

	/* �s�N�Z���P�x���� */
	PGRCVDLL_API int getImageBrightness();

	/* ���M */
	PGRCVDLL_API int sendData(int channel, int index, int value);

	/* �}�X�N�摜�쐬�p�J�����摜�ۑ� */
	PGRCVDLL_API void saveCameraImage();

	/* ���v�ʐϒl */
	PGRCVDLL_API float getTotalSize();
}
