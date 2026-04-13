#pragma once


#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>

class Material
{

public:

	Material(
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs,
		DirectX::XMFLOAT4 colorTint,
		float roughness,
		DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2(1, 1),
		DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2(0, 0));
	
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();

	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& GetTextureMap();
	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11SamplerState>>& GetSamplerMap();
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(unsigned int index);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampler(unsigned int index);
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();
	float GetRoughness();
	DirectX::XMFLOAT4 GetColorTint();
	void SetUVScale(const DirectX::XMFLOAT2& scale);
	void SetUVOffset(const DirectX::XMFLOAT2& offset);
	void SetRoughness(float roughness);
	void SetColorTint(const DirectX::XMFLOAT4& tint);
	
	// Texture Stuff
	void AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void AddTexture(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void RemoveSampler(unsigned int index);
	void RemoveTexture(unsigned int index);
	void BindTexturesAndSamplers();


private:

	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;

	// Texture Stuff

	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT4 colorTint;
	float roughness;

};

