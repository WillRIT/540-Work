#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
class Entity
{
public:



	// Getters
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw();

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
};

