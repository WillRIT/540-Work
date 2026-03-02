#pragma once

#include <DirectXMath.h>

struct VertexShaderConstants
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct PixelShaderConstants
{
	DirectX::XMFLOAT3 colorTint;
	float time;
};