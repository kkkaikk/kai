using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DebugDataVisualizer : MonoBehaviour {

  protected OmniProCamDeviceManager balloonygenManagerPtr;
  protected OmniProCamData[] omniProCamDataArrayPtr;

  protected Text debugText1;
  protected Text debugText2;
  protected Text debugText3;

  protected CameraTexture[] cameraTexturePtr = new CameraTexture[2];

  /* カメラ画像表示用 */
  protected Image imagePtr;

  // Use this for initialization
  void Start() {
    balloonygenManagerPtr = GameObject.Find("LibOmniProCamManager").GetComponent<OmniProCamDeviceManager>();
    if (balloonygenManagerPtr == null) {
      Destroy(gameObject);
    }

    debugText1 = transform.Find("DebugText1").GetComponent<Text>();
    debugText2 = transform.Find("DebugText2").GetComponent<Text>();
    debugText3 = transform.Find("DebugText3").GetComponent<Text>();

    omniProCamDataArrayPtr = balloonygenManagerPtr.getOmniProCamDataArrayPtr();

    imagePtr = GameObject.Find("CameraImage").GetComponent<Image>();
  }

  // Update is called once per frame
  void Update () {
    if (balloonygenManagerPtr.getCameraTextureType() == CameraTextureType.CAMERA_TEXTURE_TYPE_SRC) {
      cameraTexturePtr[0] = balloonygenManagerPtr.getSrcCameraImageTexturePtr();
      imagePtr.material.SetTexture("_MainTex", cameraTexturePtr[0].texture);
    } else if (balloonygenManagerPtr.getCameraTextureType() == CameraTextureType.CAMERA_TEXTURE_TYPE_BINARIZED) {
      cameraTexturePtr[1] = balloonygenManagerPtr.getBinarizedCameraImageTexturePtr();
      imagePtr.material.SetTexture("_MainTex", cameraTexturePtr[1].texture);
    }
    debugText1.text = "Data1: " + omniProCamDataArrayPtr[0].id + " " + omniProCamDataArrayPtr[0].x
      + " " + omniProCamDataArrayPtr[0].y + " " + omniProCamDataArrayPtr[0].size;
    debugText2.text = "Data2: " + omniProCamDataArrayPtr[1].id + " " + omniProCamDataArrayPtr[1].x
      + " " + omniProCamDataArrayPtr[1].y + " " + omniProCamDataArrayPtr[1].size;
    debugText3.text = "Data3: " + omniProCamDataArrayPtr[2].id + " " + omniProCamDataArrayPtr[2].x
      + " " + omniProCamDataArrayPtr[2].y + " " + omniProCamDataArrayPtr[2].size;
  } // Update
}
