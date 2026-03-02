#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
#include "Material.h"
class Entity
{
public:


	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat);
	~Entity();
	// Getters
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	void Draw();

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

