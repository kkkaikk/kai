/*********************************************************************************************
    OmniProCam Camera Initializer(view matrix only)
    
 *********************************************************************************************/

using System;
using System.IO;
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public class CameraInitializer : MonoBehaviour {

  public GameObject cameraToSetup;
  protected Camera targetCamera;

  protected float[] projectorParametersArray = new float[LibProCamCalibration.SIZE_OF_PROJECTOR_PARAMETERS];
  protected float[] cameraParametersArray = new float[LibProCamCalibration.SIZE_OF_CAMERAR_PARAMETERS];

  /* カメラパラメータ行列 */
  protected Matrix4x4 cameraIntrinsic;
  protected Matrix4x4 cameraExtrinsic;
  protected Matrix4x4 cameraProjectionMatrix;

  protected float[] k = new float[4];
  protected Vector2 c;

  void Awake() {

    /* libProCamCalibrationの初期化 */
    int ret = LibProCamCalibration.initialize(1);
    if (ret < 0) {
      print("Failed to initialize libProCamCalibration: " + ret);
      return;
    } else {
      print("libProCamCalibration has been initialized.");
    }

    /* カメラ初期化 */
    targetCamera = cameraToSetup.GetComponent<Camera>();
    setupCamera(targetCamera, 0);

    print("Unity camera has been Initialized.");

  } // Awake

  /* Unityカメラのセットアップ */
  protected void setupCamera(Camera targetCamera, int cameraIndex) {

    /* プロジェクタパラメータの取得 */
    GCHandle projectorParametersArrayGCHandle = GCHandle.Alloc(projectorParametersArray, GCHandleType.Pinned);
    IntPtr projectorParametersArrayPtr = projectorParametersArrayGCHandle.AddrOfPinnedObject();
    LibProCamCalibration.getProjectorParameters(projectorParametersArrayPtr);

    /* Distortion Parameters */
    k[0] = projectorParametersArray[0];
    k[1] = projectorParametersArray[1];
    k[2] = projectorParametersArray[2];
    k[3] = projectorParametersArray[3];

    c = new Vector2(projectorParametersArray[4], projectorParametersArray[5]);

    /* Set Translation */
    targetCamera.transform.position = new Vector3(
      projectorParametersArray[6] / 100.0f,
      projectorParametersArray[7] / 100.0f,
      projectorParametersArray[8] / 100.0f);

    /* Set Rotation */
    targetCamera.transform.rotation = Quaternion.identity;
    targetCamera.transform.Rotate(Vector3.right, -projectorParametersArray[9]);
    targetCamera.transform.Rotate(Vector3.up, -projectorParametersArray[10]);
    targetCamera.transform.Rotate(Vector3.forward, -projectorParametersArray[11]);
    //targetCamera.transform.Rotate(Vector3.forward,  180.0f);

    print("Distortion parameters: "
      + projectorParametersArray[0] + ", "
      + projectorParametersArray[1] + ", "
      + projectorParametersArray[2] + ", "
      + projectorParametersArray[3]);

    print("Camera Offset parameters: "
      + projectorParametersArray[4] + ", " + projectorParametersArray[5]);

    print("Camera Translation parameters: "
      + projectorParametersArray[6] / 100.0f + ", "
      + projectorParametersArray[7] / 100.0f + ", "
      + projectorParametersArray[8] / 100.0f);

    print("Camera Rotation(Euler) parameters: "
      + projectorParametersArray[9] + ", " + projectorParametersArray[10] + " ," + projectorParametersArray[11]);

  } // setupCamera

  /* 歪みパラメータを渡す */
  public float[] getDistortionParameters() {
    return k;
  } // getDistortionParameters

  /* 歪みパラメータを渡す */
  public Vector2 getOffset() {
    return c;
  } // getOffset

}
