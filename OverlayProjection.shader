/**************************************************************
			OmniProCam用	2019/09/21 
**************************************************************/

Shader "OmniProCam/OverlayProjection" {

Properties{
	_FrontTex("Foreground(RGB)", 2D) = "black" {}
	_BackTex0("Background0(RGB)", 2D) = "black" {}
	_AlphaTex0("Alpha0", 2D) = "gray" {}
}

SubShader{

	Pass{
		CGPROGRAM

	struct VertInput {
		float4 vertexPosition :		POSITION;
		float2 texCoord :					TEXCOORD0;
		float2 texCoordCamera0 :	TEXCOORD1;
	};

	struct FragInput {
		float4 vertexPosition :		POSITION;
		float2 texCoord :					TEXCOORD0;
		float2 texCoordCamera0 :	TEXCOORD1;
	};

#pragma vertex vertexShader
#pragma fragment fragmentShader
//#pragma fragment fragmentShaderLerp
#pragma target 3.0

#include "UnityCG.cginc"

	uniform sampler2D _FrontTex;
	uniform sampler2D _BackTex0;
	uniform sampler2D _AlphaTex0;					// camera image
	uniform float _BinarizingThreshold;		// Note: 0-255 scale, so it must be converted to 0-1.0 scale
	uniform int _imageSwitchingFlag;

	/***********************************************************
		Vertex Shader
	 ***********************************************************/
	FragInput vertexShader(VertInput vertexInput) {
		FragInput vertexOutput;
		vertexOutput.vertexPosition = UnityObjectToClipPos(vertexInput.vertexPosition);
		vertexOutput.texCoord = MultiplyUV(UNITY_MATRIX_TEXTURE0, vertexInput.texCoord);
		vertexOutput.texCoordCamera0 = MultiplyUV(UNITY_MATRIX_TEXTURE1, vertexInput.texCoordCamera0);
		return vertexOutput;
	} // vertexShader

	/***********************************************************
		Fragment Shader
	 ***********************************************************/
	float4 fragmentShader(FragInput fragInput) : COLOR{

		if (tex2D(_AlphaTex0, fragInput.texCoordCamera0).r > _BinarizingThreshold / 255.0f) {

			if (_imageSwitchingFlag) {
				return tex2D(_FrontTex, fragInput.texCoord);
			} else {
				return float4(0, 0, 0, 0);
			}


		} else {

			return float4(0, 0, 0, 0);
			//return tex2D(_BackTex0, fragInput.texCoord);

		}

	} // fragmentShader


	float4 fragmentShaderLerp(FragInput fragInput) : COLOR{

		return lerp(tex2D(_FrontTex, fragInput.texCoord), tex2D(_BackTex0, fragInput.texCoord), tex2D(_AlphaTex0, fragInput.texCoordCamera0).r);

	} // fragmentShaderLerp

		ENDCG
	}
	}
}