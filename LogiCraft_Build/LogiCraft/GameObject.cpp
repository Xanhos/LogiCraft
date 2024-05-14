
/*------------------------------------LICENSE------------------------------------
MIT License

Copyright (c) [2024] [GRALLAN Yann]
Copyright (c) [2024] [LESAGE Charles]
Copyright (c) [2024] [MENA-BOUR Samy]

This software utilizes code from the following GitHub repositories, which are also licensed under the MIT License:

- [SFML](https://github.com/SFML)
- [ImGUI](https://github.com/ocornut/imgui)
- [ImNodes](https://github.com/Nelarius/imnodes)
- [SFML-Manager](https://github.com/Xanhos/Cpp-Manager-for-SFML)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
---------------------------------------------------------------------------------*/

#include "GameObject.h"
#include "Ressource.h"
#include "AI.h"
#include "Button.h"
#include "Font.h"
#include "RigidBody.h"
#include "Convex.h"
#include "Event.h"
#include "Particule.h"
#include "Animation.h"
#include "HeatShader.h"
#include "Shader.h"


lc::GameObject::GameObject()
	: m_ID(m_generalID++), m_depth(0), m_needToBeDeleted(false), m_isLock(false), m_isVisible(true), m_needToBeExported(false)
{}

lc::GameObject::GameObject(std::string _name)
	: m_ID(m_generalID++), m_depth(0), m_needToBeDeleted(false), m_isLock(false), m_isVisible(true), m_needToBeExported(false)
{}

lc::GameObject::GameObject(std::string _name, unsigned char _depth)
	: m_name(_name), m_ID(m_generalID++), m_depth(_depth), m_needToBeDeleted(false), m_isLock(false), m_isVisible(true), m_needToBeExported(false)
{}

lc::GameObject::~GameObject()
{
	m_components.clear();
}

void lc::GameObject::Save(std::ofstream& save, std::ofstream& exportation,sf::RenderTexture& texture, int _depth)
{
	std::ostringstream oss;
	oss << getName() <<
		" " << getTransform().getPosition().x << " " << getTransform().getPosition().y << 
		" " << getTransform().getScale().x << " " << getTransform().getScale().y << 
		" " << getTransform().getSize().x << " " << getTransform().getSize().y << 
		" " << getTransform().getRotation() << 
		" " << getTransform().getOrigin().x << " " << getTransform().getOrigin().y << 
		" " << static_cast<int>(getDepth()) << std::endl;
	oss << "Components" << std::endl << "{";
	save << oss.str();
	if(getNeedToBeExported() or !getParent())
		exportation << oss.str();
	for (auto& component : getComponents())
	{
		save << std::endl;
		if(getNeedToBeExported() or !getParent())
			exportation << std::endl;
		component->Save(save, texture, _depth);
		if(getNeedToBeExported() or !getParent())
			component->Export(exportation);
	}
	if(getNeedToBeExported() or !getParent())
	exportation << std::endl << "}" << std::endl;
	
	save << std::endl << "}" << std::endl;
	
	if(getNeedToBeExported() or !getParent())
		exportation << "Objects" << std::endl << "{";
	
	save << "Objects" << std::endl << "{";
	for (auto& object : getObjects())
	{
		save << std::endl;
		if(getNeedToBeExported())
			exportation << std::endl;
		object->Save(save, exportation, texture, _depth);
	}
	save << std::endl << "}" << std::endl;
	if(getNeedToBeExported() or !getParent())
		exportation << std::endl << "}" << std::endl;
}

void lc::GameObject::SaveRenderer(sf::RenderTexture& texture, int _depth, bool& _object_has_been_drew, const sf::FloatRect& _view_rect)
{
	if(!getNeedToBeExported())
	{
		if(Tools::Collisions::rect_rect(_view_rect,{getTransform().getPosition(),getTransform().getSize()}))
			for (auto& components : m_components)
			{
				components->SaveRenderer(texture, _depth);
				_object_has_been_drew = true;
			}
	}
	for (auto& objects : m_objects)
	{
		if (objects->getDepth() == _depth)
		{
			objects->SaveRenderer(texture, _depth, _object_has_been_drew,_view_rect);
		}
	}
}

void lc::GameObject::Load(std::ifstream& load)
{
	std::string Garbage;
	auto check = [&](char c) {
		auto t = load.tellg();
		load >> Garbage;
		if (Garbage.find(c) == std::string::npos)
			load.seekg(t);
		};

	int tmp;
	load >> m_name;
	load >> m_transform.getPosition().x >> m_transform.getPosition().y;
	load >> m_transform.getScale().x >> m_transform.getScale().y;
	load >> m_transform.getSize().x >> m_transform.getSize().y;
	load >> m_transform.getRotation();
	load >> m_transform.getOrigin().x >> m_transform.getOrigin().y;
	load >> tmp;
	m_depth = (unsigned char)tmp;
	while (Garbage.find("{") == std::string::npos)
	{
		load >> Garbage;
	}
	check('}');


	while (Garbage.find("}") == std::string::npos)
	{
		int type;
		load >> type;
		if ((Ressource::TYPE)type == Ressource::TYPE::NONE)
		{

		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::TEXTURE)
		{
			addComponent(std::make_shared<Texture>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::FONT)
		{
			addComponent(std::make_shared<Font>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::IA)
		{
			addComponent(std::make_shared<AI>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::RIGIDBODY)
		{
			addComponent(std::make_shared<RigidBody>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::EVENT)
		{
			addComponent(std::make_shared<Event>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::BUTTON)
		{
			addComponent(std::make_shared<lc::Button>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::PARTICULES)
		{
			addComponent(std::make_shared<lc::Particles>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::ANIMATION)
			addComponent(std::make_shared<lc::Animation>())->Load(load);
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::CONVEX)
		{			
			addComponent(std::make_shared<lc::Convex>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::SHADER)
		{
			std::string tmp_shader_name;
			load >> tmp_shader_name;

			if (tmp_shader_name == "Heat Shader")
				addComponent(std::make_shared<lc::shader::heat_shader>())->Load(load);
		}
		check('}');
	}

	load >> Garbage;
	while (Garbage.find("{") == std::string::npos)
	{
		load >> Garbage;
	}

	check('}');
	while (Garbage.find("}") == std::string::npos)
	{
		this->addObject(CreateGameObject("OBJECT"))->Load(load);
		check('}');
	}


}

std::shared_ptr<lc::GameObject> lc::GameObject::GetRoot(std::shared_ptr<GameObject> object)
{
	while (object->getParent())
		object = object->getParent();
	return object;
}

void lc::GameObject::NeedToBeExported(std::list<std::string> ComponentToCheck)
{
	if(!m_needToBeExported)
		for (auto& i : ComponentToCheck)
		{
			if (hasComponent(i))
			{
				if(i == "Event")
				{
					auto event = std::dynamic_pointer_cast<lc::Event>(getComponent<lc::Event>(i));
					event->GetObjectAffiliated().first.lock()->m_needToBeExported = true;
					event->GetObjectAffiliated().second.lock()->m_needToBeExported = true;
				}

				m_needToBeExported = true;
				break;
			}
		}

	for (auto& object : m_objects)
	{
		object->NeedToBeExported(ComponentToCheck);
	}
}

void lc::GameObject::ResetExport()
{
	m_needToBeExported = false;
	for (auto& object : m_objects)
	{
		object->ResetExport();
	}
}




std::shared_ptr<lc::GameObject> lc::GameObject::CreateGameObject(std::string _name, unsigned char _depth)
{
	return std::make_shared<GameObject>(_name, _depth);
}

void lc::GameObject::CheckMaxSize()
{
	lc::Ressource* SizeX{ 0 },* SizeY{ 0 };
	for (auto& component : getComponents())
	{

		if (auto ressources = dynamic_cast<lc::Ressource*>(&*component))
		{
			if (!SizeX or SizeX->getMaximumSize().x < ressources->getMaximumSize().x)
				SizeX = ressources;

			if (!SizeY or SizeY->getMaximumSize().y < ressources->getMaximumSize().y)
				SizeY = ressources;
		}
	}

	if(SizeX)
		m_transform.getSize().x = SizeX->getMaximumSize().x;
	if(SizeY)
		m_transform.getSize().y = SizeY->getMaximumSize().y;
}

void lc::GameObject::UpdateEvent(sf::Event& _event)
{
	for (auto& object : m_objects)
		object->UpdateEvent(_event);

	for (auto& component : m_components)
		component->UpdateEvent(_event);
}

void lc::GameObject::Update(WindowManager& _window)
{
	for (auto object = m_objects.begin(); object != m_objects.end();)
	{
		if(object->get()->m_name != "Background_Holder")
			(*object)->Update(_window);
		
		if ((*object)->needToBeDeleted())
			object = m_objects.erase(object);
		else
			object++;
	}

	for (auto component = m_components.begin(); component != m_components.end();)
	{
		(*component)->Update(_window);
		
		if ((*component)->needToBeDeleted())
			component = m_components.erase(component);
		else
			component++;
	}

	CheckMaxSize();	
}

void lc::GameObject::Draw(WindowManager& _window)
{
	for (auto& object : m_objects)
		if (object->isVisible())
			object->Draw(_window);

	for (auto& component : m_components)
		if (component->isVisible())
			component->Draw(_window);
}

void lc::GameObject::Draw(sf::RenderTexture& _renderer)
{
	for (auto& object : m_objects)
		if (object->isVisible())
			object->Draw(_renderer);

	for (auto& component : m_components)
		if (component->isVisible())
			component->Draw(_renderer);
}

void lc::GameObject::Draw(sf::RenderTexture& _renderer, unsigned char _depth)
{
	for (auto& object : m_objects)
		if (object->isVisible())
			object->Draw(_renderer, _depth);

	for (auto& component : m_components)
		if (component->isVisible())
			if (m_depth == _depth)
				component->Draw(_renderer);
}

bool lc::GameObject::is_in_window_view(WindowManager& window)
{
	auto& tmp_window_view = window.getWindow().getView();
	return Tools::Collisions::rect_rect({getTransform().getPosition(), getTransform().getSize()},
		{tmp_window_view.getCenter() - (tmp_window_view.getSize() / 2.f), tmp_window_view.getSize()});
}

bool lc::GameObject::is_in_window_view(const sf::RenderTexture& window)
{
	auto& tmp_window_view = window.getView();
	return Tools::Collisions::rect_rect({getTransform().getPosition(), getTransform().getSize()},
		{tmp_window_view.getCenter() - (tmp_window_view.getSize() / 2.f), tmp_window_view.getSize()});
}
