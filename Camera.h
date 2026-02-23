#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include <memory>

class Camera
{
public:
	// Modified constructor: allow initial position and FOV without adding new methods
	Camera(DirectX::XMFLOAT3 initPos = DirectX::XMFLOAT3(0,0,-5), float initialFov = DirectX::XM_PIDIV4);
	~Camera();


	// Camera specific floats
	float fov;
	float nearClip;
	float farClip;
	float movSpeed;
	float mouseSpeed;
	bool isActive;

	std::shared_ptr<Transform> GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

protected:
	std::shared_ptr <Transform> transform;
private:
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4X4 projMat;
};

