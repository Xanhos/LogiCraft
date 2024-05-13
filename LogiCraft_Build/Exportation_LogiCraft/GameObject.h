
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

#pragma once
#include <typeindex>

#include "SFML_ENGINE/WindowManager.h"

#include "GameComponent.h"
#include "Transform.h"
#include "Tools_LC.h"

class Ressources;

namespace lc
{
	class GameObject : public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject();
		GameObject(std::string _name, unsigned char _depth = 0);
		GameObject(std::string _name);
		~GameObject();

		virtual void Load(std::ifstream& load);
		

		virtual void UpdateEvent(sf::Event& _event);
		virtual void Update(WindowManager& _window);
		virtual void Draw(WindowManager& _window);
		virtual void Draw(sf::RenderTexture& _renderer);
		virtual void Draw(sf::RenderTexture& _renderer, unsigned char _depth);

		void ClearGarbargeObjects();
#pragma region STATIC
		/*
		* @brief A function to create a GameObject.
		*
		* @param _name : The name of the GameObject.
		*
		* @return Return the shared_ptr of the GameObject.
		*/
		static std::shared_ptr<GameObject> CreateGameObject(std::string _name, unsigned char _depth = 0);

		static std::shared_ptr<GameObject> LoadScene(std::string _SceneToLoad, WindowManager& window_);

		std::shared_ptr<GameObject> GetRoot();
#pragma endregion

#pragma region TEMPLATE

		/*
		* @brief Add a component to the GameObject.
		* @brief (You need to put Add_Component<The class that you want to be added>())
		*
		* @return Return the shared_ptr that as been created.
		*/
		template<class T>
		std::shared_ptr<T> addComponent()
		{
			auto tmp_component = std::make_shared<T>();
			tmp_component->setParent(this->shared_from_this());
			tmp_component->setID(lc::GameComponent::getGeneralID()++);
			m_components.push_back(tmp_component);

			return tmp_component;
		}

		/*
		* @brief Add a component to the GameObject.
		*
		* @param _component : The component that you want to be added.
		*
		* @return Return the shared_ptr that as been created.
		*/
		template<class T>
		std::shared_ptr<T> addComponent(std::shared_ptr<T> _component)
		{

			_component->setParent(this->shared_from_this());
			_component->setID(lc::GameComponent::getGeneralID()++);
			m_components.push_back(_component);

			return _component;
		}

		/*
		* @brief Add a component to the GameObject.
		*
		* @param _arg : If the component as parameter you can put them here.
		*
		* @return Return the shared_ptr that as been created.
		*/
		template<class T, typename... Arg>
		std::shared_ptr<T> addComponent(Arg... _arg)
		{
			auto tmp_component = std::make_shared<T>(_arg...);
			tmp_component->setParent(this->shared_from_this());
			tmp_component->setID(lc::GameComponent::getGeneralID()++);
			m_components.push_back(tmp_component);

			return tmp_component;
		}

		/*
		* @brief Add another GameObject to the GameObject.
		*
		* @param _component : The Object that you want to be added.
		*
		* @return Return the shared_ptr that as been created.
		*/
		std::shared_ptr<GameObject> addObject(std::shared_ptr<GameObject> _object)
		{
			_object->setParent(this->shared_from_this());
			m_objects.push_back(_object);
			m_objects.sort([](std::shared_ptr<GameObject> _a, std::shared_ptr<GameObject> _b) { return _a->getDepth() > _b->getDepth(); });
			return _object;
		}

		/*
		* @brief Add another GameObject to the GameObject.
		*
		* @param _name : The name of the Object that you want to be added.
		*
		* @return Return the shared_ptr that as been created.
		*/
		std::shared_ptr<GameObject> addObject(std::string _name, unsigned char _depth = 0)
		{
			auto tmp_object = std::make_shared<GameObject>(_name, _depth);
			tmp_object->setParent(this->shared_from_this());
			m_objects.push_back(tmp_object);
			m_objects.sort([](std::shared_ptr<GameObject> _a, std::shared_ptr<GameObject> _b) { return _a->getDepth() > _b->getDepth(); });
			return tmp_object;
		}

		/*
		* @brief Just a function to return one of the wanted component.
		*
		* @return The shared_ptr of the wanted component.
		*
		* @throw If the component is not found.
		*/
		template<typename T>
		std::shared_ptr<T> getComponent(std::string _name)
		{
			for (auto& component : m_components)
				if (auto tmp_component = std::dynamic_pointer_cast<T>(component))
					if (tmp_component->getName() == _name)
						return tmp_component;

			throw std::runtime_error("Component not found.");
		}

		/*
		* @brief Just a function to return if the object has the wanted component.
		*
		* @return return true if component exist.
		*/
		bool hasComponent(std::string _name)
		{
			for (auto& component : m_components)
				if (Tools::ToLower(component->getName()) == Tools::ToLower(_name))
					return true;

			return false;
		}


		bool hasObject(std::string _name)
		{
			for (auto& object : m_objects)
				if (Tools::ToLower(object->getName()) == Tools::ToLower(_name))
					return true;

			return false;
		}
		/*
		* @brief Just a function to return one of the wanted component.
		*
		* @return The shared_ptr of the wanted object.
		*
		* @throw If the object is not found.
		*/
		std::shared_ptr<GameObject> getObject(std::string _name)
		{
			for (auto& object : m_objects)
				if (Tools::ToLower(object->getName()) == Tools::ToLower(_name))
					return object;

			throw std::runtime_error("Object not found.");
		}

		/*
		* @brief Just a function to return one of the wanted component.
		*
		* @return The shared_ptr of the wanted object.
		*
		* @throw If the object is not found.
		*/
		std::shared_ptr<GameObject> getObject(std::string _name, unsigned int _ID)
		{
			for (auto& object : m_objects)
				if (object->getName() == _name && object->getID() == _ID)
					return object;

			throw std::runtime_error("Object not found.");
		}

		/*
		* @brief Just a function to remove wanted object.
		*
		* @throw If the object is not found.
		*/
		void removeObject(std::string _name)
		{
			for (auto object = m_objects.begin(); object != m_objects.end();)
			{
				if ((*object)->getName() == _name)
					object = m_objects.erase(object);
				else
					object++;
			}
		}

		/*
		* @brief Just a function to remove wanted object.
		*
		* @throw If the object is not found.
		*/
		void removeObject(std::string _name, unsigned int _ID)
		{
			for (auto object = m_objects.begin(); object != m_objects.end();)
			{
				if ((*object)->getName() == _name && (*object)->getID() == _ID)
					object = m_objects.erase(object);
				else
					object++;
			}
		}

		bool objectIsParent(std::string _name, unsigned int _ID)
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

		std::shared_ptr<lc::GameObject> Clone()
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
#pragma endregion

#pragma region GETTER/SETTER
		lc::Transform& getTransform() { return m_transform; }

		std::string& getName() { return m_name; }

		void setParent(const std::shared_ptr<GameObject> _parent) { m_wptrParent = _parent; }
		auto getParent() { return m_wptrParent.lock(); }

		void setDepth(unsigned char _depth) { m_depth = _depth; }
		unsigned char getDepth() const { return m_depth; }

		auto& getComponents() { return m_components; }
		auto& getObjects() { return m_objects; }

		void setID(unsigned int _ID) { m_ID = _ID; }
		unsigned int getID() const { return m_ID; }

		void needToBeRemoved(bool _state) { m_needToBeRemove = _state; }
		bool hasToBeRemoved() const { return m_needToBeRemove; }



		static unsigned int& getGeneralID() { return m_generalID; }
		void CheckMaxSize();

#pragma endregion
	private:

		inline static unsigned int m_generalID = 0u;

		std::string m_name;
		unsigned int m_ID;
		unsigned char m_depth;

		bool m_needToBeRemove;


		std::list<std::shared_ptr<GameComponent>> m_components;
		std::list<std::shared_ptr<GameObject>> m_objects;

		std::weak_ptr<GameObject> m_wptrParent;

		lc::Transform m_transform;
	};
}