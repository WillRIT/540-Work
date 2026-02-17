#pragma once

#include <DirectXMath.h>

struct VertexShaderConstants
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 world;
};
