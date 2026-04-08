#pragma once
#pragma once

#include "Mesh.h"
#include "Camera.h"

#include <memory>
#include <wrl/client.h> // Used for ComPtr
#include <d3d11.h>

class Sky
{
public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVS,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPS);
	~Sky();
	std::shared_ptr<Mesh> skyMesh;
	
	void Draw(std::shared_ptr<Camera> cam);


private:

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	// Skybox related resources
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPS;


	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyDepthState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
		
};

