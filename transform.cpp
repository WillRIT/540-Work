#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{

}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{


}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{

}

void Transform::SetScale(float x, float y, float z)
{
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return DirectX::XMFLOAT4X4();
}

void Transform::MoveAbsolute(float x, float y, float z)
{
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
}

void Transform::Scale(float x, float y, float z)
{
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
}
