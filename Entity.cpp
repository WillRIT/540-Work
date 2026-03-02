#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat)
{
	this->mesh = mesh;
	this->material = mat;
	transform = std::make_shared<Transform>();
}

Entity::~Entity()
{ }

std::shared_ptr<Material> Entity::GetMaterial() 
{ 
	return material; 
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Transform> Entity::GetTransform()
{
	return transform;
}

void Entity::Draw()
{
	GetMesh()->Draw();
}