#include "Camera.h"
#include "Input.h"

Camera::Camera() :
	fov(DirectX::XM_PIDIV4),
	nearClip(0.01f),
	farClip(100.0f),
	movSpeed(5.0f),
	mouseSpeed(0.1f),
	isActive(true)
{
}


DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return DirectX::XMFLOAT4X4();
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
	DirectX::XMVECTOR worldUp = DirectX::XMVECTOR(0, 1, 0, 0);
	DirectX::XMVECTOR forward = XMLoadFloat3(&transform.GetForward());
	DirectX::XMVECTOR position = XMLoadFloat3(&transform.GetPosition());

	DirectX::XMMATRIX newView = DirectX::XMMatrixLookToLH(
		position, 
		forward, 
		worldUp);

	DirectX::XMStoreFloat4x4(&viewMat, newView);
}

void Camera::Update(float dt)
{
	if (Input::KeyDown('W'))
	{
		transform.MoveRelative(0, 0, movSpeed * dt);
	}

	if (Input::KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -movSpeed * dt);
	}

	if (Input::KeyDown('A'))
	{
		transform.MoveRelative(-movSpeed * dt, 0, 0);
	}

	if (Input::KeyDown('D'))
	{
		transform.MoveRelative(movSpeed * dt, 0, 0);
	}

	if (Input::KeyDown(VK_SPACE))
	{
		transform.MoveAbsolute(0, movSpeed * dt, 0);
	}

	if (Input::KeyDown(VK_SHIFT))
	{
		transform.MoveAbsolute(0, -movSpeed * dt, 0);
	}

	if (Input::MouseLeftDown())
	{
		int cursorY = Input::GetMouseYDelta();
		int cursorX = Input::GetMouseXDelta();

		transform.Rotate(cursorY * mouseSpeed * dt, cursorX * mouseSpeed * dt, 0);
		// Figure out how to clamp rotation for X....
	}

	UpdateViewMatrix();
}
