#include "Entity.h"


std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Transform> Entity::GetTransform()
{
	return std::make_shared<Transform>(transform);
}

void Entity::Draw()
{
	GetMesh()->Draw();
}