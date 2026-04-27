#pragma once
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_LIGHTS 64
#include <DirectXMath.h>




struct Light
{
	int Type; // Which kind of light? 0, 1 or 2 (see above)
	DirectX::XMFLOAT3 Direction; // Directional and Spot lights need a direction
	float Range; // Point and Spot lights have a max range for attenuation
	DirectX::XMFLOAT3 Position; // Point and Spot lights have a position in space
	float Intensity; // All lights need an intensity
	DirectX::XMFLOAT3 Color; // All lights need a color
	float SpotInnerAngle; // Inner cone angle (in radians) – Inside this, full light!
	float SpotOuterAngle; // Outer cone angle (radians) – Outside this, no light!
	DirectX::XMFLOAT2 Padding; // Purposefully padding to hit the 16-byte boundary
};

// Copied from Chris Cascioli (@vixorien's github) https://github.com/vixorien/ggp-demos/blob/e983d886e571ab257ef16262d54a786a73d537ec/GGP/14%20-%20Shadow%20Mapping/Lights.h#L46
struct DemoShadowOptions
{
	int ShadowMapResolution;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowSRV;

	float ShadowProjectionSize;
	DirectX::XMFLOAT4X4 LightViewMatrix;
	DirectX::XMFLOAT4X4 LightProjectionMatrix;

	D3D11_RASTERIZER_DESC ShadowRasterizerDesc;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> ShadowRasterizerState;
};