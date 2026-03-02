#pragma once


#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>


class Material
{

public:

	Material(
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs,
		DirectX::XMFLOAT4 colorTint);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	DirectX::XMFLOAT4 GetColorTint();
	


private:

	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;

	DirectX::XMFLOAT4 colorTint;

};

