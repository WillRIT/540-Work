#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "BufferStructs.h"
#include <vector>
#include "Entity.h"
#include "Camera.h"
#include <DirectXMath.h>
#include "Material.h"
#include "Lights.h"
#include "Sky.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator


	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(const wchar_t* compiledShaderPath);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader(const wchar_t* compiledShaderPath);

	int number = 4;
	float colors[4] = {30, 30, 30, 30};
	bool isVisible = true;
	
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Camera> camera2;

	std::vector<Camera> cameras;
	std::vector<Entity> entities;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;

	Sky sky;

	// Light management
	std::vector<Light> lights;
	// Directional Lights
	Light directionalLight1 = {};
	Light directionalLight2 = {};

	// Spot Lights
	Light spotlight1 = {};
	Light spotlight2 = {};

	// Point Lights
	Light pointLight1 = {};
	Light pointLight2 = {};

	// Ambient Light
	DirectX::XMFLOAT3 ambientLightColor = { 0.2f, 0.2f, 0.2f }; // Grey Sky

private:
	bool animateEntities = true;
	std::vector<DirectX::XMFLOAT3> entityPositions;
	std::vector<DirectX::XMFLOAT3> entityRotations;
	std::vector<DirectX::XMFLOAT3> entityScales;

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// Constant buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;  // Vertex shader constant buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> psConstantBuffer;  // Pixel shader constant buffer
};

