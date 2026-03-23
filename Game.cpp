#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"

#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <memory>
#include <cstring>
#include <cmath>
#include <string>


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();

	// Create vertex shader constant buffer
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VertexShaderConstants);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Graphics::Device->CreateBuffer(&cbd, 0, vsConstantBuffer.GetAddressOf());

	// Create pixel shader constant buffer
	D3D11_BUFFER_DESC psCbd = {};
	psCbd.Usage = D3D11_USAGE_DYNAMIC;
	psCbd.ByteWidth = sizeof(PixelShaderConstants);
	psCbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	psCbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Graphics::Device->CreateBuffer(&psCbd, 0, psConstantBuffer.GetAddressOf());

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	//Making my badass camera yuuuup

	// Create camera 1 with default constructor
	camera = std::make_shared<Camera>();
	// Create camera 2 with different position and FOV
	camera2 = std::make_shared<Camera>(DirectX::XMFLOAT3(0.0f, 2.0f, -8.0f), DirectX::XM_PIDIV4 / 1.5f);

	// Only one active at start: camera is active, camera2 inactive
	camera->isActive = true;
	camera2->isActive = false;

	// Keep cameras vector synced with shared_ptrs by storing copies
	cameras.clear();
	cameras.push_back(*camera);
	cameras.push_back(*camera2);

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Create an input layout 
				//  - This describes the layout of data sent to a vertex shader
				//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
				//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
		D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - UV coordinates, which is 2 float values
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;					// 2x 32-bit floats
		inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Set up the third element - a normal, which is 3 float values
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// 3x 32-bit floats
		inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input!
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element


		// Create the input layout, verifying our description against actual shader code
		ID3DBlob* vertexShaderBlob;
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			ARRAYSIZE(inputElements),				// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer

		// Set the input layout now that it exists
		Graphics::Context->IASetInputLayout(inputLayout.Get());
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
/// <summary>
/// Loads a pixel shader from a compiled shader object (.cso) file and returns a ComPtr to it.
/// </summary>
/// <param name="compiledShaderPath">shader path</param>
/// <returns></returns>
Microsoft::WRL::ComPtr<ID3D11PixelShader> Game::LoadPixelShader(const wchar_t* compiledShaderPath)
{
	// Read the contents of the compiled shader object to a blob
	ID3DBlob* shaderBlob;
	D3DReadFileToBlob(compiledShaderPath, &shaderBlob);

	// Create the pixel shader and return it
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Graphics::Device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		shaderBlob->GetBufferSize(),	// How big is that data?
		0,								// No classes in this shader
		shader.GetAddressOf());			// ID3D11PixelShader**
	return shader;
}

/// <summary>
/// Loads a vertex shader from a compiled shader object (.cso) file and returns a ComPtr to it.
/// </summary>
/// <param name="compiledShaderPath">shader path</param>
/// <returns></returns>
Microsoft::WRL::ComPtr<ID3D11VertexShader> Game::LoadVertexShader(const wchar_t* compiledShaderPath)
{
	// Read the contents of the compiled shader object to a blob
	ID3DBlob* shaderBlob;
	D3DReadFileToBlob(compiledShaderPath, &shaderBlob);

	// Create the pixel shader and return it
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Graphics::Device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		shaderBlob->GetBufferSize(),	// How big is that data?
		0,								// No classes in this shader
		shader.GetAddressOf());			// Address of the ID3D11PixelShader pointer
	return shader;
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	//Load Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVertexShader = LoadVertexShader(FixPath(L"VertexShader.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPixelShader = LoadPixelShader(FixPath(L"PixelShader.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> uvPixelShader = LoadPixelShader(FixPath(L"DebugUVsPS.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> normalPixelShader = LoadPixelShader(FixPath(L"DebugNormalsPS.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> fancyShader = LoadPixelShader(FixPath(L"CustomPS.cso").c_str());

	// Load Models - Convert wide strings to narrow strings using WideToNarrow helper
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/cube.obj")).c_str());
	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/cylinder.obj")).c_str());
	std::shared_ptr<Mesh> helixMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/helix.obj")).c_str());
	std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/quad.obj")).c_str());
	std::shared_ptr<Mesh> quadDoubleMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/quad_double_sided.obj")).c_str());
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/sphere.obj")).c_str());
	std::shared_ptr<Mesh> torusMesh = std::make_shared<Mesh>(WideToNarrow(FixPath(L"../../Assets/Meshes/torus.obj")).c_str());

	meshes.push_back(cubeMesh);
	meshes.push_back(cylinderMesh);
	meshes.push_back(helixMesh);
	meshes.push_back(quadMesh);
	meshes.push_back(quadDoubleMesh);
	meshes.push_back(sphereMesh);
	meshes.push_back(torusMesh);

	// Create Materials

	std::shared_ptr<Material> redMat = std::make_shared<Material>(basicPixelShader, basicVertexShader, DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f));
	std::shared_ptr<Material> greenMat = std::make_shared<Material>(basicPixelShader, basicVertexShader, DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f));
	std::shared_ptr<Material> blueMat = std::make_shared<Material>(basicPixelShader, basicVertexShader, DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f));
	std::shared_ptr<Material> uvMat = std::make_shared<Material>(uvPixelShader, basicVertexShader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	std::shared_ptr<Material> normalMat = std::make_shared<Material>(normalPixelShader, basicVertexShader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	std::shared_ptr<Material> fancyMat = std::make_shared<Material>(fancyShader, basicVertexShader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));



	materials.push_back(redMat);
	materials.push_back(greenMat);
	materials.push_back(blueMat);
	materials.push_back(uvMat);
	materials.push_back(normalMat);
	materials.push_back(fancyMat);

	// Create Entities

	entities.push_back(Entity(meshes[0], redMat));        // 0: cube
	entities.push_back(Entity(meshes[1], greenMat));      // 1: cylinder
	
	entities.push_back(Entity(meshes[2], uvMat));         // 2: helix
	entities.push_back(Entity(meshes[5], uvMat));         // 3: sphere

	entities.push_back(Entity(meshes[1], normalMat));     // 1: cube
	entities.push_back(Entity(meshes[0], normalMat));     // 0: 

	entities.push_back(Entity(meshes[4], fancyMat));      // 6: quad_double
	entities.push_back(Entity(meshes[6], fancyMat));      // 7: torus


	entities[0].GetTransform()->MoveAbsolute(-9, 4, 0);
	entities[1].GetTransform()->MoveAbsolute(-6, 5, 0);
	entities[2].GetTransform()->MoveAbsolute(-3, -4, 0);
	entities[3].GetTransform()->MoveAbsolute(0, -5, 0);
	entities[4].GetTransform()->MoveAbsolute(3, 0, 0);
	entities[5].GetTransform()->MoveAbsolute(6, 0, 0);
	entities[6].GetTransform()->MoveAbsolute(9, -7, 0);
	entities[7].GetTransform()->MoveAbsolute(12, 0, 0);


	// Initialize transform vectors to match entity count
	entityPositions.assign(entities.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));
	entityRotations.assign(entities.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));
	entityScales.assign(entities.size(), XMFLOAT3(1.0f, 1.0f, 1.0f));
}



// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (int i = 0; i < cameras.size(); i++) {
		cameras[i].UpdateProjectionMatrix(Window::AspectRatio());

	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Put this all in a helper method that is called from Game::Update()
// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	
	// Update only the active camera
	if (camera->isActive)
		camera->Update(deltaTime);
	else if (camera2->isActive)
		camera2->Update(deltaTime);
	
	// Make a New Window
	if (isVisible)
	{
		ImGui::Begin("My AWESOME CUSTOM Window");

		ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::Text("Window is %d pixels wide and %d pixels high", Window::Width(), Window::Height());
		ImGui::ColorEdit4("RGBA color editor", &colors[0]);
		ImGui::SliderInt("Choose an awesome number", &number, 0, 100);
		ImGui::Checkbox("Toggle Window Visibility", &isVisible);
		ImGui::Checkbox("Animate Entities", &animateEntities);
		ImGui::Text("Now this is just some cool text to show off the text function.");
		ImGui::Button("PRESS ME!");

		// Camera toggle UI: mutual-exclusive radio buttons
		ImGui::Separator();
		ImGui::Text("Active Camera");
		if (ImGui::RadioButton("Camera 1", camera->isActive))
		{
			camera->isActive = true;
			camera2->isActive = false;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Camera 2", camera2->isActive))
		{
			camera2->isActive = true;
			camera->isActive = false;
		}

		if (ImGui::CollapsingHeader("Camera Info"))
		{
			ImGui::Text("Camera 1:");
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera->GetTransform()->GetPosition().x, camera->GetTransform()->GetPosition().y, camera->GetTransform()->GetPosition().z);
			ImGui::Text("Rotation (Pitch/Yaw/Roll): (%.2f, %.2f, %.2f)", camera->GetTransform()->GetPitchYawRoll().x, camera->GetTransform()->GetPitchYawRoll().y, camera->GetTransform()->GetPitchYawRoll().z);
			ImGui::Text("FOV: %.2f degrees", camera->fov * (180.0f / XM_PI));
		}

		if (ImGui::CollapsingHeader("Entity Transforms"))
		{
			for (size_t i = 0; i < entities.size(); i++)
			{
				std::string label = "Entity " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::PushID(static_cast<int>(i));
					ImGui::DragFloat3("Position", &entityPositions[i].x, 0.01f);
					ImGui::DragFloat3("Rotation", &entityRotations[i].x, 0.01f);
					ImGui::DragFloat3("Scale", &entityScales[i].x, 0.01f, 0.01f, 5.0f);
					ImGui::PopID();


					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}

	for (size_t i = 0; i < entities.size(); i++)
	{
		XMFLOAT3 position = entityPositions[i];
		XMFLOAT3 rotation = entityRotations[i];
		XMFLOAT3 scale = entityScales[i];

		if (animateEntities)
		{
			float t = totalTime + static_cast<float>(i);
			float radius = 0.2f + 0.1f * static_cast<float>(i);
			position.x += std::cos(t * 0.7f) * radius;
			position.y += std::sin(t * 0.9f) * radius;
			rotation.z += t;
		}

		auto transform = entities[i].GetTransform();
		transform->SetPosition(position);
		transform->SetRotation(rotation);
		transform->SetScale(scale);
	}
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	colors);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		 // Re-bind input layout and topology (ImGui may have changed these)
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		for (auto& e: entities)
		{
			// Grab the material
			std::shared_ptr<Material> mat = e.GetMaterial();

			// Set up the pipeline for this draw
			Graphics::Context->VSSetShader(mat->GetVertexShader().Get(), 0, 0);
			Graphics::Context->PSSetShader(mat->GetPixelShader().Get(), 0, 0);

			// Set vertex shader data
			VertexShaderConstants vsData{};
			vsData.world = e.GetTransform()->GetWorldMatrix();
			vsData.view = camera->GetViewMatrix();
			vsData.projection = camera->GetProjectionMatrix();
			Graphics::FillAndBindNextConstantBuffer(&vsData, sizeof(VertexShaderConstants), D3D11_VERTEX_SHADER, 0);

			// Set pixel shader data
			PixelShaderConstants psData{};
			psData.colorTint = mat->GetColorTint();
			psData.time = totalTime;
			Graphics::FillAndBindNextConstantBuffer(&psData, sizeof(PixelShaderConstants), D3D11_PIXEL_SHADER, 0);

			// Draw one entity
			e.Draw();
		}
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}
