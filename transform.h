#pragma once
#include <DirectXMath.h>
class Transform
{
public:

	Transform();

	// Transformers
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation); // XMFLOAT4 for quaternion
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scaleOff);


	// Getters
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetForward();

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll(); // XMFLOAT4 GetRotation() for quaternion
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();


	/*
	The easiest (although least efficient) time to recalculate the world matrix is whenever any of the
position, rotation or scale values change. Feel free to do it this way for now. However, chances are that
more than one of these will change in the same frame. Here are other approaches:
• Since the world matrix isn’t actually used until the draw phase of the game loop, it would be
more efficient to simply recreate the matrix once per frame, after all possible updating occurs.
• An even more optimized version would track whether or not any of the 3 major transformations
have changed, meaning the matrix is “dirty” and needs an update. You would then update the
matrix right before it's actually returned by a GetWorldMatrix() method, and only if it's dirty
*/

	/*
	To easily represent and change the values of the transform, store the raw transformation data (position,
rotation and scale) as DirectX Math vectors. You should also store two 4x4 matrices: world and
worldInverseTranspose. The “world inverse transpose” matrix will be used in a future assignment.
Note that the “vector” for rotation is really just individual pitch/yaw/roll angles stored as the
x/y/z components of a 3D vector for convenience.
Alternatively, it could be a true quaternion, stored as a 4D vector, though this and future
assignments assume you’re simply storing the three angles. Feel free to experiment!
*/

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	bool matrixDirty;
	bool vectorsDirty;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 forward;

	// World and Inverse World
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
};

