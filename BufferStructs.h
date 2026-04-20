#pragma once

#include <DirectXMath.h>
#include "Lights.h"

struct VertexShaderConstants
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;


	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjMatrix;
};

// FOR THE LOVE OF GOD MAKE SURE THIS MATCHES WITH THE HLSL VERSION OF THE BUFFERS CRYING EMOJI CHINESE BRUH
struct PixelShaderConstants
{	
	int numLights;
	DirectX::XMFLOAT3 ambientColor;

	Light lights[MAX_LIGHTS];

	DirectX::XMFLOAT3 cameraPosition;
	float pad;

	DirectX::XMFLOAT4 colorTint;
	float roughness;
	DirectX::XMFLOAT3 padding;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
};