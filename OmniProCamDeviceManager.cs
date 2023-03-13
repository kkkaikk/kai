/************************************************************************
  OmniProCamDeviceManager: OmniProCamデバイスの初期化とデータの取得クラス
  
 ************************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using System.Threading;
using System.Runtime.InteropServices;

/* カメラテクスチャ */
public class CameraTexture {
  public CameraTexture(int width, int height) {
    texture = new Texture2D(width, height, TextureFormat.RGBA32, false);
    pixels = texture.GetPixels32();
    pixelsHandle = GCHandle.Alloc(pixels, GCHandleType.Pinned);
    pixelsIntPtr = pixelsHandle.AddrOfPinnedObject();
  }
  public Texture2D texture = null;
  public Color32[] pixels;
  public IntPtr pixelsIntPtr;
  public GCHandle pixelsHandle;
} // CameraTexture

public enum CameraTextureType {
  CAMERA_TEXTURE_TYPE_SRC,        // カメラ画像そのまま
  CAMERA_TEXTURE_TYPE_BINARIZED,  // 2値画像
  CAMERA_TEXTURE_TYPE_NO_RESULT,  // 取得しない
};

public class OmniProCamDeviceManager : MonoBehaviour {

  public string settingFilePath;
  protected int maxObjects;
  protected Rect roi;
  protected int maxSizeThreshold, minSizeThreshold;
  protected bool isInitialized = false;
  protected int sdkType = 1;

  /* OmniProCamから受け取ったデータ */
  protected OmniProCamData[] resultOmniProCamData;

  /* 結果画像のタイプ */
  public CameraTextureType cameraTextureType = CameraTextureType.CAMERA_TEXTURE_TYPE_SRC;

  protected float fpsTick;
  protected int frameCounter;
  protected int fps;

  public float shutterSpeed = 10000.0f;
  public float gain = 18.0f;
 
  protected CameraTexture[] cameraTextureArray = new CameraTexture[2];    // 0: src image, 1: binarized image

  // Use this for initialization
  void Awake() {

    /* 設定ファイル読み込み */
    try {
      print("Start loading OmniProCam config parameters...");
      using (StreamReader streamReader = new StreamReader(settingFilePath)) {

        while (streamReader.EndOfStream == false) {
          sdkType = int.Parse(streamReader.ReadLine());
          maxObjects = int.Parse(streamReader.ReadLine());
          roi.x = float.Parse(streamReader.ReadLine());
          roi.y = float.Parse(streamReader.ReadLine());
          roi.width = float.Parse(streamReader.ReadLine());
          roi.height = float.Parse(streamReader.ReadLine());
          maxSizeThreshold = int.Parse(streamReader.ReadLine());
          minSizeThreshold = int.Parse(streamReader.ReadLine());
          print("Parameters have been loaded from " + settingFilePath);
          print(" SDK Type: " + sdkType + "  MaxObjects: " + maxObjects + ", ROI.x: " + roi.x + ", ROI.y: " + roi.y
             + ", ROI.width: " + roi.width + ", ROI.height: " + roi.height 
             + ", MaxSizeThreshold: " + maxSizeThreshold + ", MinSizeThreshold: " + minSizeThreshold);
        } // while

        /* 初期化 */
        print("Start initializing OmniProCam devices...");
        LibOmniProCam.setCameraSDKType(sdkType);
        int returnedValue = LibOmniProCam.initialize((int)roi.x, (int)roi.y, (int)roi.width, (int)roi.height, 
          maxSizeThreshold, minSizeThreshold, maxObjects);
        if (returnedValue == 0) {
          print("LibOmniProCam has been initialized!");
          isInitialized = true;
        } else {
          print("Failed to initialize LibOmniProCam: " + returnedValue.ToString());
          isInitialized = false;
          Destroy(gameObject);
        }
      }

      /* 結果データ初期化 */
      resultOmniProCamData = new OmniProCamData[maxObjects];

      /* カメラ画像配列の初期化 */
      cameraTextureArray[0] = new CameraTexture((int)roi.width, (int)roi.height);   // src
      cameraTextureArray[1] = new CameraTexture((int)roi.width, (int)roi.height);   // src

      LibOmniProCam.setShutterSpeed((int)shutterSpeed);
      LibOmniProCam.setGain(gain);
      LibOmniProCam.setBinarizingThreshold(50.0f);

    } catch (Exception e) {
      print("OmniProCam config file(" + settingFilePath + ") could not be loaded(" + e.ToString() + ")");
      Destroy(gameObject);
    }

  } // Awake

  // Update is called once per frame
  void Update() {

    /* Process */
    //int counter = LibOmniProCam.process();
    LibOmniProCam.process();
    //print(counter);

    /* 結果取得 */
    LibOmniProCam.getObjectData(resultOmniProCamData);

    /* カメラ画像の取得 */
    if (cameraTextureType == CameraTextureType.CAMERA_TEXTURE_TYPE_SRC) {
      LibOmniProCam.getSrcImageRGBAForUnity(cameraTextureArray[0].pixelsIntPtr, (int)roi.width, (int)roi.height);
      cameraTextureArray[0].texture.SetPixels32(cameraTextureArray[0].pixels);
      cameraTextureArray[0].texture.Apply();
    } else if (cameraTextureType == CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED) {
      LibOmniProCam.getBinarizedImageRGBAForUnity(cameraTextureArray[1].pixelsIntPtr, (int)roi.width, (int)roi.height);
      cameraTextureArray[1].texture.SetPixels32(cameraTextureArray[1].pixels);
      cameraTextureArray[1].texture.Apply();
    }
    //Debug.Log(roi);
  } // Update

  void OnDestroy() {
    if (isInitialized) {
      LibOmniProCam.finalize();
      print("libOmniProCamThread: Closed.");
    }
  } // OnDestroy

  /* カメラテクスチャタイプの設定 */
  public void setCameraTextureType(CameraTextureType type) {
    cameraTextureType = type;
  } // setCameraTextureType
  public CameraTextureType getCameraTextureType() {
    return cameraTextureType;
  } // getCameraTextureType

  /* OmniProCamデータの取得 */
  public OmniProCamData[] getOmniProCamDataArrayPtr() {
    return resultOmniProCamData;
  } // getOmniProCamDataArrayPtr

  public CameraTexture getSrcCameraImageTexturePtr() {
    return cameraTextureArray[0];
  } // getSrcCameraImageTexturePtr
  public CameraTexture getBinarizedCameraImageTexturePtr() {
    return cameraTextureArray[1];
  } // getBinarizedCameraImageTexturePtr

}
