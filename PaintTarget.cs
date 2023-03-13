/********************************************************
    ProjectionTarget
    箱の物理的な回転操作はここに記述する。
    
 ********************************************************/

using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PaintTarget : ProjectionTarget {

  protected Texture2D canvasTexture;
  public Texture2D brushTexture;

  protected SpriteRenderer spriteRenderer;

  // Use this for initialization
  void Start() {

    /* マルチディスプレイ有効化 */
    if (Display.displays.Length > 1) {
      Display.displays[1].Activate();
    }

    if (Display.displays.Length > 2) {
      Display.displays[2].Activate();
    }

    omniProCamDeviceManagerPtr = GameObject.Find("OmniProCam/LibOmniProCamManager").GetComponent<OmniProCamDeviceManager>();
    spriteRenderer = GetComponent<SpriteRenderer>();

    canvasTexture = GetComponent<SpriteRenderer>().sprite.texture;

    /* 初期値 */
    loadAdjustmentValuesToFile();
  }

  // Update is called once per frame
  void Update() {

    /* スケールの微調整 */
    if (!Input.GetKey(KeyCode.O)) {

      /* 縦スケールの微調整 */
      if (Input.GetKey(KeyCode.E)) {
        scale.y += 0.1f * Time.deltaTime;
        scale.x += 0.1f * Time.deltaTime;
        transform.localScale = scale;
      } else if (Input.GetKey(KeyCode.D)) {
        scale.y -= 0.1f * Time.deltaTime;
        scale.x -= 0.1f * Time.deltaTime;
        transform.localScale = scale;
      }
      /* 縦スケールの微調整 */
      if (Input.GetKey(KeyCode.F)) {
        scale.x += 0.1f * Time.deltaTime;
        scale.y += 0.1f * Time.deltaTime;
        transform.localScale = scale;
      } else if (Input.GetKey(KeyCode.A)) {
        scale.x -= 0.1f * Time.deltaTime;
        scale.y -= 0.1f * Time.deltaTime;
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

    /* 書き出し */
    if (Input.GetKeyDown(KeyCode.W)) {
      saveAdjustmentValuesToFile();
      print("Projection Adjustment Parameters have been saved to: " + projectionAdjustmentParameterFile);
    }

    /*  */
    OmniProCamData[] ptr = omniProCamDeviceManagerPtr.getOmniProCamDataArrayPtr();
    foreach (OmniProCamData i in ptr) {
      if (i.id != 0) {
        print("Position: " + i.x + "," + i.y + " Size: " + i.size);
        canvasTexture.SetPixels((int)i.x, (int)i.y, 5, 5, brushTexture.GetPixels());
      }
    }

    spriteRenderer.material.SetTexture("_MainTex", omniProCamDeviceManagerPtr.getSrcCameraImageTexturePtr().texture);
  }

}
