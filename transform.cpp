#include "Transform.h"

using namespace DirectX;

Transform::Transform() : 
	position(0.0f, 0.0f, 0.0f),
	pitchYawRoll(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f),
	up(0.0f, 1.0f, 0.0f),
	right(1.0f, 0.0f, 0.0f),
	forward(0.0f, 0.0f, 1.0f),
	matrixDirty(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	matrixDirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
	matrixDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	matrixDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll = rotation;
	matrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	matrixDirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
	matrixDirty = true;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	matrixDirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;
	matrixDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;

	matrixDirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll.x += rotation.x;
	pitchYawRoll.y += rotation.y;
	pitchYawRoll.z += rotation.z;
	matrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	matrixDirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scaleOff)
{
	scale.x = scaleOff.x;
	scale.y = scaleOff.y;
	scale.z = scaleOff.z;
	matrixDirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX worldMat = scaleMat * rotMat * transMat;

	XMStoreFloat4x4(&worldMatrix, worldMat);
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX worldMat = scaleMat * rotMat * transMat;

	XMStoreFloat4x4(&worldMatrix, worldMat);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
	return worldInverseTransposeMatrix;
}
