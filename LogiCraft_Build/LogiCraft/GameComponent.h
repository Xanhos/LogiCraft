#pragma once
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"
#include "tools_imgui.h"

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

		~GameComponent();

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

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) = 0;
		virtual void SaveRenderer(sf::RenderTexture& texture, int _depth) = 0;
		virtual void Load(std::ifstream& load) = 0;

		/*
		*@brief This function is used to set the function that will be called when the component is drawn in the hierarchy.
		* 
		*/
		virtual void setHierarchieFunc() = 0;
#pragma endregion

	protected:
		std::string m_name;
		std::string m_typeName;

		bool m_needToBeDeleted;

		std::function<void()> m_hierarchieInformation;

		unsigned short m_ID;
		inline static unsigned short m_generalID = 0u;
	private:
		std::weak_ptr<lc::GameObject> m_wptrParent;
	};
}