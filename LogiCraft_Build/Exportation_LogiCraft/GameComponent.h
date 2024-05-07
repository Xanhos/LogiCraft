
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
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

#include <functional>

namespace lc
{
	/*
	* @brief Forward declaration.
	* @brief Its there to use GameObject because GameComponent is already include in GameObject.
	* @see GameObject()
	*/
	class GameObject;

	class GameComponent
	{
	public:

#pragma region STATIC
		/*
		* @brief This function is here to create Game Component more easly.
		* @brief And to avoid calling std::make_shared.
		* 
		* @return std::shared_ptr<T> : This will be the shared_ptr that as been created.
		* @return If you want to modify it after his creation, you can.
		*/
		template<typename T>
		static std::shared_ptr<T> CreateGameComponent()
		{
			return std::make_shared<T>();
		}

		/*
		* @brief This function is here to create Game Component more easly.
		* @brief And to avoid calling std::make_shared.
		* @brief And this one you can put a shared_ptr.
		* 
		* @param _component : If you want to create another component from an already existing one.
		* 
		* @return std::shared_ptr<T> : This will be the shared_ptr that as been created.
		* @return If you want to modify it after his creation, you can.
		*/
		template<typename T>
		static std::shared_ptr<T> CreateGameComponent(std::shared_ptr<T> _component)
		{
			return std::make_shared<T>(_component);
		}

		/*
		* @brief This function is here to create Game Component more easly.
		* @brief And to avoid calling std::make_shared.
		* @brief This one allow you to put directly the argument in the function.
		*
		* @param _arg : If your component have _arg in his constructor you can put them directly in it.
		*
		* @return std::shared_ptr<T> : This will be the shared_ptr that as been created.
		* @return If you want to modify it after his creation, you can.
		*/
		template<typename T, typename... Arg>
		static std::shared_ptr<T> CreateGameComponent(Arg... _arg)
		{
			return std::make_shared<T>(_arg...);
		}
#pragma endregion

		GameComponent();
		GameComponent(std::string _name);

		virtual ~GameComponent();

		virtual void UpdateEvent(sf::Event& _event) = 0;

		virtual void Update(WindowManager& _window) = 0;

		virtual void Draw(WindowManager& _window) = 0;
		virtual void Draw(sf::RenderTexture& _window) = 0;
		void Hierarchie_Draw(unsigned int _parentID, std::list<std::shared_ptr<lc::GameComponent>>::iterator& _actComp, std::list<std::shared_ptr<lc::GameComponent>>& _compList);

		virtual std::shared_ptr<lc::GameComponent> Clone() = 0;

#pragma region SETTER/GETTER
		/*
		* @brief This function is used to set the parent of the component.
		* @brief Its use when you call AddComponent from the class GameObject.
		* @see GameObject()
		*/
		inline void setParent(const std::shared_ptr<lc::GameObject> _parent) { m_wptrParent = _parent; }

		/*
		* @brief This function just return the shared_ptr of the parent.
		*/
		inline std::shared_ptr<lc::GameObject> getParent() { return m_wptrParent.lock(); }

		void setName(std::string _name) { m_name = _name; }
		std::string getName() const { return m_name; }

		void setTypeName(std::string _typeName) { m_typeName = _typeName; }
		std::string getTypeName() const { return m_typeName; }

		unsigned short getID() const { return m_ID; }
		void setID(unsigned short _ID) { m_ID = _ID; }

		void hasToBeDeleted(bool _needToBeDeleted) { m_needToBeDeleted = _needToBeDeleted; }
		bool needToBeDeleted() const { return m_needToBeDeleted; }

		static unsigned short& getGeneralID() { return m_generalID; }

		virtual void Load(std::ifstream& load) = 0;	
#pragma endregion

	protected:
		std::string m_name;
		std::string m_typeName;

		bool m_needToBeDeleted;


		unsigned short m_ID;
		inline static unsigned short m_generalID = 0u;
	private:
		std::weak_ptr<lc::GameObject> m_wptrParent;
	};
}