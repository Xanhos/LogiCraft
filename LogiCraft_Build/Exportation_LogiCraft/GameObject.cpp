
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

#include <mutex>

#include "Texture.h"
#include "RigidBody.h"
#include "Font.h"
#include "AI.h"
#include "Animation.h"
#include "Button.h"
#include "DisplayCollider.h"
#include "Event.h"
#include "HeatShader.h"
#include "LightShader.h"
#include "Particule.h"
#include "WaterShader.h"
#include "PlayerSpawn.h"

lc::GameObject::GameObject()
	: m_ID(m_generalID++), m_depth(0), m_isVisible(true), m_isUpdated(true), m_needToBeRemove(false)
{
}

lc::GameObject::GameObject(std::string _name)
	: m_ID(m_generalID++), m_depth(0), m_isVisible(true), m_isUpdated(true), m_needToBeRemove(false)
{
}

lc::GameObject::GameObject(std::string _name, unsigned char _depth)
	: m_name(_name), m_ID(m_generalID++), m_depth(_depth), m_isVisible(true), m_isUpdated(true),
	  m_needToBeRemove(false)
{
}

lc::GameObject::~GameObject()
{
	m_components.clear();
}

void lc::GameObject::Load(std::ifstream& load)
{
	std::string garbage;
	auto check = [&](char c) {
		auto fileCursorPos = load.tellg();
		load >> garbage;
		if (garbage.find(c) == std::string::npos)
			load.seekg(fileCursorPos);
		};

	int tmp;
	load >> m_name;
	load >> m_transform.getPosition().x >> m_transform.getPosition().y;
	load >> m_transform.getScale().x >> m_transform.getScale().y;
	load >> m_transform.getSize().x >> m_transform.getSize().y;
	load >> m_transform.getRotation();
	load >> m_transform.getOrigin().x >> m_transform.getOrigin().y;
	load >> tmp;
	m_depth = static_cast<unsigned char>(tmp);
	while (garbage.find('{') == std::string::npos)
	{
		load >> garbage;
	}
	check('}');


	while (garbage.find('}') == std::string::npos)
	{
		int type;
		load >> type;
		if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::NONE)
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
			addComponent(std::make_shared<lc::Event>())->Load(load);
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
		{
			addComponent(std::make_shared<lc::Animation>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::SHADER)
		{
			std::string tmp_shader_name;
			load >> tmp_shader_name;

			if (tmp_shader_name == "Heat_Shader")
				addComponent(std::make_shared<lc::Shader::HeatShader>())->Load(load);
			else if (tmp_shader_name == "Water_Shader")
				addComponent(std::make_shared<lc::Shader::WaterShader>())->Load(load);
			else if (tmp_shader_name == "Light_Shader")
				addComponent(std::make_shared<lc::Shader::LightShader>())->Load(load);
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::CONVEX)
		{
			
		}
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::DISPLAY_COLLIDER)
		{
			addComponent(std::make_shared<lc::DisplayCollider>())->Load(load);
		}		
		else if (static_cast<Ressource::TYPE>(type) == Ressource::TYPE::SPAWN_POSITION)
		{
			addComponent(std::make_shared<lc::PlayerSpawn>())->Load(load);
		}
		check('}');
	}

	load >> garbage;
	while (garbage.find('{') == std::string::npos)
	{
		load >> garbage;
	}

	check('}');
	while (garbage.find('}') == std::string::npos)
	{
		this->addObject(CreateGameObject("OBJECT"))->Load(load);
		check('}');
	}
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
		if ((*object)->isUpdated())
			(*object)->Update(_window);
		
		if ((*object)->hasToBeRemoved())
			object = m_objects.erase(object);
		else
			++object;
	}

	for (auto component = m_components.begin(); component != m_components.end();)
	{
		if ((*component)->isUpdated())
			(*component)->Update(_window);
		
		if ((*component)->needToBeDeleted())
			component = m_components.erase(component);
		else
			++component;
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

void lc::GameObject::ClearGarbargeObjects()
{
	m_objects.remove_if([](std::shared_ptr<lc::GameObject> object) { return object->m_components.empty(); });
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

std::shared_ptr<lc::GameObject> lc::GameObject::CreateGameObject(std::string _name, unsigned char _depth)
{
	return std::make_shared<GameObject>(_name, _depth);
}

std::shared_ptr<lc::GameObject> lc::GameObject::LoadScene(std::string _SceneToLoad, WindowManager& window_)
{
	_SceneToLoad = "../Ressources/" + _SceneToLoad;
	std::cout << _SceneToLoad << "\n";
	auto world = CreateGameObject("WORLD");
	FileReader file(_SceneToLoad + "/export.lcg");
	int thread_number;
	file >> thread_number;
	ThreadManager thread_list(thread_number);
	std::mutex mutex;
	for(int i = 0; i < thread_number; i++)
	{
		sf::Vector2i index;
		file >> index;
		thread_list.AddNewThread([=, &mutex]
		{
			for (auto& dir_entry : fs::directory_iterator(_SceneToLoad))
			{
				if (dir_entry.path().extension() == ".png")
				{
					std::istringstream iss(Tools::replaceSpace(dir_entry.path().filename().stem().string()));			
					std::string garbage;
					int pos_bg_x(0), pos_bg_y(0), depth(0);
					sf::Vector2f position(0,0);
					iss >> pos_bg_x >> pos_bg_y >> garbage >> garbage >> depth;
					if(pos_bg_x == index.x and pos_bg_y == index.y)
					{
						position = sf::Vector2f(static_cast<float>(0 + (screenResolution.x * pos_bg_x)), static_cast<float>(0 + (screenResolution.y * pos_bg_y)));
						mutex.lock();
						auto object = world->addObject("BACKGROUND " + std::to_string(static_cast<unsigned char>(depth)),static_cast<unsigned char>(depth));
						std::cout << dir_entry.path().string() + "\n";
						mutex.unlock();
						object->addComponent(std::make_shared<Texture>(dir_entry.path().filename().string(), dir_entry.path().string()))->setRelativePosition(position);
					}
				}
			}
		});	
	}
	world->Update(window_);
	world->Load(file);
	return world;
}

std::shared_ptr<lc::GameObject> lc::GameObject::GetRoot(std::shared_ptr<GameObject> object)
{
	while (object->getParent())
		object = object->getParent();
	return object;
}

bool lc::GameObject::hasComponent(std::string _name)
{
	for (auto& component : m_components)
		if (component->getName() == _name)
			return true;

	return false;
}

bool lc::GameObject::hasComponent(std::string _name, unsigned int _ID)
{
	for (auto& component : m_components)
		if (component->getName() == _name && component->getID() == _ID)
			return true;

	return false;
}

void lc::GameObject::removeComponent(std::string _name)
{
	for (auto component = m_components.begin(); component != m_components.end();)
	{
		if ((*component)->getName() == _name)
		{
			component = m_components.erase(component);
			break;
		}
		else
			++component;
	}
}

void lc::GameObject::removeComponent(std::string _name, unsigned int _ID)
{
	for (auto component = m_components.begin(); component != m_components.end();)
	{
		if ((*component)->getName() == _name && (*component)->getID() == _ID)
		{
			component = m_components.erase(component);
			break;
		}
		else
			++component;
	}
}

std::shared_ptr<lc::GameObject> lc::GameObject::addObject(std::shared_ptr<GameObject> _object)
{
	_object->setParent(this->shared_from_this());
	m_objects.push_back(_object);
	m_objects.sort([](std::shared_ptr<GameObject> _a, std::shared_ptr<GameObject> _b) { return _a->getDepth() > _b->getDepth(); });
	return _object;
}

std::shared_ptr<lc::GameObject> lc::GameObject::addObject(std::string _name, unsigned char _depth)
{
	auto tmp_object = std::make_shared<GameObject>(_name, _depth);
	tmp_object->setParent(this->shared_from_this());
	m_objects.push_back(tmp_object);
	m_objects.sort([](std::shared_ptr<GameObject> _a, std::shared_ptr<GameObject> _b) { return _a->getDepth() > _b->getDepth(); });
	return tmp_object;
}

std::shared_ptr<lc::GameObject> lc::GameObject::getObject(std::string _name)
{
	for (auto& object : m_objects)
		if (object->getName() == _name)
			return object;

	throw std::runtime_error("Object not found.");
}

std::shared_ptr<lc::GameObject> lc::GameObject::getObject(std::string _name, unsigned int _ID)
{
	for (auto& object : m_objects)
		if (object->getName() == _name && object->getID() == _ID)
			return object;

	throw std::runtime_error("Object not found.");
}

bool lc::GameObject::hasObject(std::string _name)
{
	for (auto& object : m_objects)
		if (Tools::ToLower(object->getName()) == Tools::ToLower(_name))
			if (object->getName() == _name)
				return true;

	return false;
}

bool lc::GameObject::hasObject(std::string _name, unsigned int _ID)
{
	for (auto& object : m_objects)
		if (Tools::ToLower(object->getName()) == Tools::ToLower(_name))
			if (object->getName() == _name && object->getID() == _ID)
				return true;

	return false;
}

void lc::GameObject::removeObject(std::string _name)
{
	for (auto object = m_objects.begin(); object != m_objects.end();)
	{
		if ((*object)->getName() == _name)
		{
			object = m_objects.erase(object);
			break;
		}
		else
			object++;
	}
}

void lc::GameObject::removeObject(std::string _name, unsigned int _ID)
{
	for (auto object = m_objects.begin(); object != m_objects.end();)
	{
		if ((*object)->getName() == _name && (*object)->getID() == _ID)
		{
			object = m_objects.erase(object);
			break;
		}
		else
			object++;
	}
}

bool lc::GameObject::objectIsParent(std::string _name, unsigned int _ID)
{
	for (auto& gameObject : m_objects)
	{
		if (gameObject->getName() == _name && gameObject->getID() == _ID)
			return true;
		else if (gameObject->objectIsParent(_name, _ID))
			return true;
	}

	return false;
}

std::shared_ptr<lc::GameObject> lc::GameObject::Clone()
{
	auto Clone = lc::GameObject::CreateGameObject(m_name, m_depth);
	Clone->getTransform() = getTransform();
	Clone->getTransform().getPosition() += sf::Vector2f(10.f, 10.f);
	for (auto& component : m_components)
	{
		Clone->addComponent(component->Clone());
	}

	for (auto& object : m_objects)
	{
		Clone->addObject(object->Clone());
	}

	return Clone;
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