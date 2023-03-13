/***************************************************************************
    LibProCamCalibration: libProCamCalibration.dllのUnity用APIクラス
    
 ***************************************************************************/

using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class LibProCamCalibration {

  public static int SIZE_OF_PROJECTOR_PARAMETERS = 12;
  public static int SIZE_OF_CAMERAR_PARAMETERS = 27;

  ///* Unityとやりとりするプロジェクタパラメータのファイルフォーマット */
  //typedef struct{
	 // float k[4];
	 // float c[2];
	 // float translation[3];
	 // float rotation[3];
  //} ProjectorParametersForUnity;

  ///* Unityとやりとりするカメラパラメータのファイルフォーマット */
  //typedef struct{
	 // float intrinsicMatrix[9];
	 // float extrinsicMatrix[16];
	 // float roiWidth;
	 // float roiHeight;
  //} CameraParametersForUnity;

  /* 初期化 */
  [DllImport("libProCamCalibration")]
  public static extern int initialize(int enableLog);

  /* カメラパラメータの取得 */
  [DllImport("libProCamCalibration")]
  public static extern int getCameraParameters(int index, IntPtr dstCameraParameters);

  /* プロジェクタパラメータの取得 */
  [DllImport("libProCamCalibration")]
  public static extern int getProjectorParameters(IntPtr dstProjectorParameters);
}
