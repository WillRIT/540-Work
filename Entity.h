#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
class Entity
{
public:


	Entity(std::shared_ptr<Mesh> mesh);
	~Entity();
	// Getters
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw();

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
};

