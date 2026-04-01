#pragma once

#include <DirectXMath.h>

struct VertexShaderConstants
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct PixelShaderConstants
{

	int lightCount;
	DirectX::XMFLOAT3 ambientColor;

	// Camera related
	DirectX::XMFLOAT3 cameraPosition;
	float pad; // Alignment

	DirectX::XMFLOAT4 colorTint;  // 16 bytes
	float time;                    // 4 bytes
	DirectX::XMFLOAT3 padding;    // 12 bytes padding (total = 32 bytes, multiple of 16)
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
};