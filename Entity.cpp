#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	transform = std::make_shared<Transform>();
}

Entity::~Entity()
{ }


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