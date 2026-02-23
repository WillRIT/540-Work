#include "Camera.h"
#include "Input.h"
#include "Window.h"

Camera::Camera(DirectX::XMFLOAT3 initPos, float initialFov) :
	fov(initialFov),
	nearClip(0.01f),
	farClip(100.0f),
	movSpeed(5.0f),
	mouseSpeed(0.1f),
	isActive(true)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(initPos);
	UpdateViewMatrix();
	UpdateProjectionMatrix(Window::AspectRatio());
}

Camera::~Camera()
{}

std::shared_ptr<Transform> Camera::GetTransform() { return transform; }

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMat;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projMat;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMMATRIX newProj = DirectX::XMMatrixPerspectiveFovLH(
		fov, 
		aspectRatio, 
		nearClip, 
		farClip);

	DirectX::XMStoreFloat4x4(&projMat, newProj);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMFLOAT3 forward = transform->GetForward();
	DirectX::XMFLOAT3 position = transform->GetPosition();

	DirectX::XMMATRIX newView = DirectX::XMMatrixLookToLH(
		DirectX::XMLoadFloat3(&position),
		DirectX::XMLoadFloat3(&forward), 
		worldUp);

	DirectX::XMStoreFloat4x4(&viewMat, newView);
}

void Camera::Update(float dt)
{
	// Only update cameras that are active; Game will ensure only one is active
	if (!isActive) return;

	if (Input::KeyDown('W'))
	{
		transform->MoveRelative(0, 0, movSpeed * dt);
	}

	if (Input::KeyDown('S'))
	{
		transform->MoveRelative(0, 0, -movSpeed * dt);
	}

	if (Input::KeyDown('A'))
	{
		transform->MoveRelative(-movSpeed * dt, 0, 0);
	}

	if (Input::KeyDown('D'))
	{
		transform->MoveRelative(movSpeed * dt, 0, 0);
	}

	if (Input::KeyDown(VK_SPACE))
	{
		transform->MoveAbsolute(0, movSpeed * dt, 0);
	}

	if (Input::KeyDown(VK_SHIFT))
	{
		transform->MoveAbsolute(0, -movSpeed * dt, 0);
	}

	if (Input::MouseLeftDown())
	{
		int cursorY = Input::GetMouseYDelta();
		int cursorX = Input::GetMouseXDelta();

		transform->Rotate(cursorY * mouseSpeed * dt, cursorX * mouseSpeed * dt, 0);
		// Figure out how to clamp rotation for X....
	}

	UpdateViewMatrix();
}
