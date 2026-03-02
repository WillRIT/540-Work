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


	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VertexShaderConstants);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Graphics::Device->CreateBuffer(&cbd, 0, vsConstantBuffer.GetAddressOf());
	

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

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;					// 2x 32-bit floats
		inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Set up the second element - a color, which is 4 more float values
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

	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVertexShader = LoadVertexShader(FixPath(L"VertexShader.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPixelShader = LoadPixelShader(FixPath(L"PixelShader.cso").c_str());


	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex triVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },

	};
	Vertex squareVertices[] =
	{
		{ XMFLOAT3(+0.7f, +0.7f, +0.0f), red },
		{ XMFLOAT3(+0.7f, +0.5f, +0.0f), blue },
		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), green },
		{ XMFLOAT3(+0.5f, +0.7f, +0.0f), green },
	};

	Vertex weirdVertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), blue },		// Center
		{ XMFLOAT3(0.0f, 0.8f, 0.0f), red },		// Top
		{ XMFLOAT3(0.76f, 0.25f, 0.0f), green },	// Top-right
		{ XMFLOAT3(0.47f, -0.65f, 0.0f), red },	// Bottom-right
		{ XMFLOAT3(-0.47f, -0.65f, 0.0f), green },	// Bottom-left
		{ XMFLOAT3(-0.76f, 0.25f, 0.0f), red },	// Top-left
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triIndices[] = { 0, 1, 2 };
	unsigned int squareIndices[] = { 0, 1, 2, 0, 2, 3 };
	unsigned int weirdIndices[] = {
	0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 1
	};

	// Make Materials

	std::shared_ptr <Material> matRed = std::make_shared<Material>(basicPixelShader, basicVertexShader, XMFLOAT4(0.75f, 0,0, 0 ));
	std::shared_ptr <Material> matBlue = std::make_shared<Material>(basicPixelShader, basicVertexShader, XMFLOAT4(0, 0, 0.75f, 0));
	std::shared_ptr <Material> matGreen = std::make_shared<Material>(basicPixelShader, basicVertexShader, XMFLOAT4(0, 0.75f, 0, 0));

	materials.push_back(matRed);
	materials.push_back(matBlue);
	materials.push_back(matGreen);





	triangle = std::make_shared<Mesh>(triVertices, triIndices, 3, 3);
	square = std::make_shared<Mesh>(squareVertices, squareIndices, 4, 6);
	weird = std::make_shared<Mesh>(weirdVertices, weirdIndices, 6, 15);

	entities.push_back(Entity(weird, matRed));
	entities.push_back(Entity(triangle, matBlue));
	entities.push_back(Entity(square, matGreen));
	entities.push_back(Entity(weird, matRed));
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



		/*
		ImGui::ColorEdit4("Tint", &vsData.colorTint.x);
		ImGui::DragFloat3("Offset", &vsData.offset.x, 0.01f, -1.0f, 1.0f);
		*/
		if (ImGui::CollapsingHeader("Mesh Info"))
		{
			ImGui::Text("Triangle Triangle: 1");

			ImGui::Text("Triangle Verts: %d", triangle->GetVertexCount());
			ImGui::Text("Triangle Indices: %d", triangle->GetIndexCount());

			ImGui::Text("Square Triangles: %d", square->GetIndexCount() / 3);
			ImGui::Text("Square Verts: %d", square->GetVertexCount());
			ImGui::Text("Square Indices: %d", square->GetIndexCount());
			
			ImGui::Text("Penta Triangles: %d", square->GetIndexCount() / 3);
			ImGui::Text("Penta Verts: %d", weird->GetVertexCount());
			ImGui::Text("Penta Indices: %d", weird->GetIndexCount());
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

	{
		//for (int i = 0; i < entities.size(); i++)
		//
		//{
		//	// Use active camera for rendering
		//	Camera* activeCam = camera->isActive ? camera.get() : camera2.get();
		//	VertexShaderConstants vsData = { DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), entities[i].GetTransform()->GetWorldMatrix(), activeCam->GetViewMatrix(), activeCam->GetProjectionMatrix()};
		//	D3D11_MAPPED_SUBRESOURCE mapped = {};
		//	Graphics::Context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		//	std::memcpy(mapped.pData, &vsData, sizeof(vsData));
		//	Graphics::Context->Unmap(vsConstantBuffer.Get(), 0);
		//	entities[i].Draw();
		//}
	}
	// ID3D11DeviceContext::DrawIndexed: The Vertex Shader expects application provided input data (which is to say data other than hardware auto-generated values such as VertexID or InstanceID). Therefore an Input Assembler object is expected, but none is bound.

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		for (auto& e: entities)
		{
			// get the mat

			std::shared_ptr<Material> mat = e.GetMaterial();

			// Make our pipeline
			Graphics::Context->VSSetShader(mat->GetVertexShader().Get(), 0, 0);
			Graphics::Context->PSSetShader(mat->GetPixelShader().Get(), 0, 0);

			VertexShaderConstants vsData = {};
			vsData.world = e.GetTransform()->GetWorldMatrix();
			vsData.view = camera->isActive ? camera->GetViewMatrix() : camera2->GetViewMatrix();
			vsData.projection = camera->isActive ? camera->GetProjectionMatrix() : camera2->GetProjectionMatrix();
			vsData.colorTint = mat->GetColorTint();

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
