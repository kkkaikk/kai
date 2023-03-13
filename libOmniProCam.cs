using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct OmniProCamData {
  public uint id;
  public float x;
  public float y;
  public float size;
  public float boundingBoxX;
  public float boundingBoxY;
  public float boundingBoxWidth;
  public float boundingBoxHeight;
};

public class LibOmniProCam {

  /* 使用するカメラSDKの種類の設定(initialize()より先に呼ぶ) */
  [DllImport("libOmniProCam")]
  public static extern void setCameraSDKType(int type);  // 0: No Camerea, 1: FlyCaptureSDK, 2: SpinnakerSDK

  /* 初期化 */
  [DllImport("libOmniProCam")]
  public static extern int initialize(int roiX, int roiY, int roiWidth, int roiHeight, int maxRegionSize, int minRegionSize, int maxRegions);

  /* 処理(ループに入れる) */
  [DllImport("libOmniProCam")]
  public static extern int process();   // 検出数をreturnする

  /* 解放 */
  [DllImport("libOmniProCam")]
  public static extern void finalize();

  /* 結果を返す */
  [DllImport("libOmniProCam")]
  public static extern void getObjectData([In, Out] OmniProCamData[] resultDataArray);

  /* FPSを返す */
  [DllImport("libOmniProCam")]
  public static extern int getFPS();

  /********************************
				カメラ画像をコピーする
	********************************/
  [DllImport("libOmniProCam")]
  public static extern void getSrcImageGRAY(IntPtr dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
  [DllImport("libOmniProCam")]
  public static extern void getBinarizedImageGRAY(IntPtr dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

  /*********************************************
			カメラ画像をコピーする(for Unity)
	*********************************************/
  [DllImport("libOmniProCam")]
  public static extern void getSrcImageRGBAForUnity(IntPtr dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);
  [DllImport("libOmniProCam")]
  public static extern void getBinarizedImageRGBAForUnity(IntPtr dstImagePixelsPtr, int dstImagewidth, int dstImageHeight);

  /********************************
							設定
	********************************/

  /* カメラ設定 */
  [DllImport("libOmniProCam")]
  public static extern void setShutterSpeed(int shutterSpeedUSec);
  [DllImport("libOmniProCam")]
  public static extern void setGain(float gain);

  /* 面積閾値のセット */
  [DllImport("libOmniProCam")]
  public static extern void setSizeThreshold(float min, float max);

  /* 2値化閾値のセット */
  [DllImport("libOmniProCam")]
  public static extern void setBinarizingThreshold(float threshold);

  /* バッファリリース */
  [DllImport("libOmniProCam")]
  public static extern void releaseCameraImageBuffer();
}
