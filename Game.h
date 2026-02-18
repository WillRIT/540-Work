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

	int number = 4;
	float colors[4] = {30, 30, 30, 30};
	bool isVisible = true;
	
	std::shared_ptr<Camera> camera;

	std::vector<Entity> entities; 

	std::shared_ptr<Mesh> weird;
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> square;


private:
	bool animateEntities = true;
	std::vector<DirectX::XMFLOAT3> entityPositions;
	std::vector<DirectX::XMFLOAT3> entityRotations;
	std::vector<DirectX::XMFLOAT3> entityScales;

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;
};

