/********************************************************
    ProjectionTarget
    箱の物理的な回転操作はここに記述する。
    Created on 2018/08/26 
 ********************************************************/

using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ProjectionTarget : MonoBehaviour {

  protected OmniProCamDeviceManager omniProCamDeviceManagerPtr;
  protected OmniProCamData[] balloonygenData;

  protected MeshRenderer meshRenderer;

  /* 手動調節パラメータ */
  public string projectionAdjustmentParameterFile;
  protected Vector3 scale;
  protected Vector2 offset;
  protected float height;

  protected bool isRotating = false;
  public float binarizingThreshold = 100.0f;

  // Use this for initialization
  void Start () {

    /* マルチディスプレイ有効化 */
    if (Display.displays.Length > 1) {
      Display.displays[1].Activate();
    }

    if (Display.displays.Length > 2) {
      Display.displays[2].Activate();
    }

    omniProCamDeviceManagerPtr = GameObject.Find("OmniProCam/LibOmniProCamManager").GetComponent<OmniProCamDeviceManager>();
    meshRenderer = GetComponent<MeshRenderer>();

    /* 初期値 */
    //loadAdjustmentValuesToFile();

    LibOmniProCam.setBinarizingThreshold(binarizingThreshold);
  }
	
	// Update is called once per frame
	void Update () {
    
    /* スケールの微調整 */
    if (!Input.GetKey(KeyCode.O)) {

      /* 縦スケールの微調整 */
      if (Input.GetKey(KeyCode.E)) {
        scale.y += 0.1f * Time.deltaTime;
        transform.localScale = scale;
      } else if (Input.GetKey(KeyCode.D)) {
        scale.y -= 0.1f * Time.deltaTime;
        transform.localScale = scale;
      }
      /* 縦スケールの微調整 */
      if (Input.GetKey(KeyCode.F)) {
        scale.x += 0.1f * Time.deltaTime;
        transform.localScale = scale;
      } else if (Input.GetKey(KeyCode.A)) {
        scale.x -= 0.1f * Time.deltaTime;
        transform.localScale = scale;
      }

    /* オフセットの設定 */
    }else {

      /* 縦オフセットの微調整 */
      if (Input.GetKey(KeyCode.E)) {
        offset.y += 0.05f * Time.deltaTime;
        transform.position = new Vector3(offset.x, 0.0f, offset.y);
      } else if (Input.GetKey(KeyCode.D)) {
        offset.y -= 0.05f * Time.deltaTime;
        transform.position = new Vector3(offset.x, 0.0f, offset.y);
      }

      /* 横オフセットの微調整 */
      if (Input.GetKey(KeyCode.F)) {
        offset.x += 0.05f * Time.deltaTime;
        transform.position = new Vector3(offset.x, 0.0f, offset.y);
      } else if (Input.GetKey(KeyCode.A)) {
        offset.x -= 0.05f * Time.deltaTime;
        transform.position = new Vector3(offset.x, 0.0f, offset.y);
      }
    }

    /* 縦オフセットの微調整 */
    if (Input.GetKey(KeyCode.E)) {
      offset.y += 0.05f * Time.deltaTime;
      transform.position = new Vector3(offset.x, 0.0f, offset.y);
    }

    /* 2値化閾値 */
    if (Input.GetKey(KeyCode.UpArrow)) {
      binarizingThreshold += 1.0f;
      if (binarizingThreshold >= 255.0f) {
        binarizingThreshold = 255;
      }
      LibOmniProCam.setBinarizingThreshold(binarizingThreshold);
      print("BinarizingThreshold: " + binarizingThreshold);
    } else if (Input.GetKey(KeyCode.DownArrow)) {
      binarizingThreshold -= 1.0f;
      if (binarizingThreshold <= 0.0f) {
        binarizingThreshold = 0;
      }
      LibOmniProCam.setBinarizingThreshold(binarizingThreshold);
      print("BinarizingThreshold: " + binarizingThreshold);
    }

    ///* 高さ */
    //if (Input.GetKey(KeyCode.PageUp)) {
    //  transform.Translate(0.0f, 0.1f * Time.deltaTime, 0.0f);
    //} else if (Input.GetKey(KeyCode.PageDown)) {
    //  transform.Translate(0.0f, -0.1f * Time.deltaTime, 0.0f);
    //}

    /* 書き出し */
    if (Input.GetKeyDown(KeyCode.Alpha0)) {
      if (omniProCamDeviceManagerPtr.getCameraTextureType() == CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED) {
        omniProCamDeviceManagerPtr.setCameraTextureType(CameraTextureType.CAMERA_TEXTURE_TYPE_SRC);
        print("CameraTextureType: Src");
      } else {
        omniProCamDeviceManagerPtr.setCameraTextureType(CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED);
        print("CameraTextureType: Binarized");
      }
    }

    //OmniProCamData[] ptr = omniProCamDeviceManagerPtr.getOmniProCamDataArrayPtr();
    //foreach (OmniProCamData i in ptr) {
    //  if (i.id != 0) {
    //  }
    //}

    if (omniProCamDeviceManagerPtr.getCameraTextureType() == CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED) {
      meshRenderer.material.SetTexture("_MainTex", omniProCamDeviceManagerPtr.getBinarizedCameraImageTexturePtr().texture);
    } else {
      meshRenderer.material.SetTexture("_MainTex", omniProCamDeviceManagerPtr.getSrcCameraImageTexturePtr().texture);
    }
  }

  /* 保存 */
  public void saveAdjustmentValuesToFile() {
    StreamWriter streamWriter = new StreamWriter(projectionAdjustmentParameterFile, false);
    streamWriter.WriteLine(offset.x);
    streamWriter.WriteLine(offset.y);
    streamWriter.WriteLine(transform.localScale.x);
    streamWriter.WriteLine(transform.localScale.y);
    streamWriter.WriteLine(transform.localScale.z);
    streamWriter.WriteLine(transform.position.y);
    streamWriter.Flush();
    streamWriter.Close();
  } // saveAdjustmentValuesToFile

  /* 読み込み */
  public void loadAdjustmentValuesToFile() {
    StreamReader streamReader = new StreamReader(projectionAdjustmentParameterFile);
    offset.x = float.Parse(streamReader.ReadLine());
    offset.y = float.Parse(streamReader.ReadLine());
    float tmpX = float.Parse(streamReader.ReadLine());
    float tmpY = float.Parse(streamReader.ReadLine());
    float tmpZ = float.Parse(streamReader.ReadLine());
    scale = new Vector3(tmpX, tmpY, tmpZ);
    height = float.Parse(streamReader.ReadLine());
    streamReader.Close();

    print("Offset: " + offset + " Scale: " + scale);

    transform.Translate(offset.x, 0.0f, offset.y);
    transform.localScale = scale;
  } // loadAdjustmentValuesToFile

  /* テクスチャのセット */
  public void setTexture(int index, ref Texture2D texture) {
    meshRenderer.materials[index].SetTexture("_MainTex", texture);
  } // setTexture

}
