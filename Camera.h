#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include <memory>

class Camera
{
public:
	Camera();
	~Camera();


	// Camera specific floats
	float fov;
	float nearClip;
	float farClip;
	float movSpeed;
	float mouseSpeed;
	bool isActive;


	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);


	


private:
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4X4 projMat;
	std::shared_ptr <Transform> transform;
};

