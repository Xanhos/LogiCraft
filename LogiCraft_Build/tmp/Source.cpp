#include "GameObject.h"



int main()
{
	auto object = lc::GameObject::CreateGameObject("World");
	std::ifstream load("D:\\LogiCraft\\LogiCraft_Build\\Ressources\\Test\\export.lcg");
	object->Load(load);


	return 1;
}
