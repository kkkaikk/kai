using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OverlayProjectionTarget : ProjectionTarget {
  
  protected float tick = 0;

  public float imageBlinkingInterval = 0.0f;

  // Update is called once per frame
  void Update() {

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
    } else {

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
      meshRenderer.material.SetFloat("_BinarizingThreshold", binarizingThreshold);
    } else if (Input.GetKey(KeyCode.DownArrow)) {
      binarizingThreshold -= 1.0f;
      if (binarizingThreshold <= 0.0f) {
        binarizingThreshold = 0;
      }
      LibOmniProCam.setBinarizingThreshold(binarizingThreshold);
      meshRenderer.material.SetFloat("_BinarizingThreshold", binarizingThreshold);
      print("BinarizingThreshold: " + binarizingThreshold);
    }

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

    /* Set Camera Image to Material */
    if (omniProCamDeviceManagerPtr.getCameraTextureType() == CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED) {
      meshRenderer.material.SetTexture("_AlphaTex0", omniProCamDeviceManagerPtr.getBinarizedCameraImageTexturePtr().texture);
    } else {
      meshRenderer.material.SetTexture("_AlphaTex0", omniProCamDeviceManagerPtr.getSrcCameraImageTexturePtr().texture);
    }

    /* Image blinking */
    if (Input.GetKey(KeyCode.Return) || tick + imageBlinkingInterval < Time.realtimeSinceStartup) {
      meshRenderer.material.SetInt("_imageSwitchingFlag", 1);
      tick = Time.realtimeSinceStartup;
    } else {
      meshRenderer.material.SetInt("_imageSwitchingFlag", 0);
    }
  }
}
