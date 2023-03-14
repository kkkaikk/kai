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

/* 外部とやり取りするデータ形式(Unity用) */
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

	/* カメラSDKタイプの設定(initialize()より先に呼ぶ) */
	PGRCVDLL_API void setCameraSDKType(int type);

	/* 初期化 */
	PGRCVDLL_API int initialize(int roiX, int roiY, int roiWidth, int roiHeight, int maxRegions, int maxRegionSize, int minRegionSize);

	/* 処理(ループに入れる) */
	PGRCVDLL_API int process();

	/* 解放 */
	PGRCVDLL_API void finalize();

	/* 結果を返す */
	PGRCVDLL_API void getObjectData(OmniProCamData *resultDataArray);

	/* FPSを返す */
	PGRCVDLL_API int getFPS();

	/********************************
			カメラ画像をコピーする
	********************************/
	PGRCVDLL_API void getSrcImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getBinarizedImageGRAY(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getMaskedSrcImageGRAY(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

	/*********************************************
			カメラ画像をコピーする(for Unity)
	*********************************************/
	PGRCVDLL_API void getSrcImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getBinarizedImageRGBAForUnity(unsigned char *dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
	PGRCVDLL_API void getMaskedSrcImageRGBAForUnity(unsigned char* dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

	/********************************
			設定
	********************************/

	/* カメラ設定 */
	PGRCVDLL_API void setShutterSpeed(int shutterSpeedUSec);
	PGRCVDLL_API void setGain(float gain);

	/* 面積閾値のセット */
	PGRCVDLL_API void setSizeThreshold(float min, float max);

	/* 2値化閾値のセット */
	PGRCVDLL_API void setBinarizingThreshold(float threshold);

	/* 2値化閾値のセット */
	PGRCVDLL_API void setAdaptiveBinarizingThreshold(int blockSize, double cValue);

	/* バッファリリース */
	PGRCVDLL_API void releaseCameraImageBuffer();

	/* 背景再取得 */
	PGRCVDLL_API void resetBackground();

	/* 背景保存 */
	PGRCVDLL_API void saveBackgroundToFile();
	PGRCVDLL_API void loadBackgroundFromFile(); 

	/* ピクセル輝度値の取得 */
	PGRCVDLL_API int getPixelLuminance(int x, int y);

	/* ピクセル輝度平均 */
	PGRCVDLL_API int getImageBrightness();

	/* 送信 */
	PGRCVDLL_API int sendData(int channel, int index, int value);

	/* マスク画像作成用カメラ画像保存 */
	PGRCVDLL_API void saveCameraImage();

	/* 合計面積値 */
	PGRCVDLL_API float getTotalSize();
}
