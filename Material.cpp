#include "Material.h"


Material::Material(
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs,
	DirectX::XMFLOAT4 colorTint)
{
	this->ps = ps;
	this->vs = vs;
	this->colorTint = colorTint;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
	return ps;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
	return vs;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return DirectX::XMFLOAT4();
}
