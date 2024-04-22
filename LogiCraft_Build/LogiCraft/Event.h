#pragma once
#include "Ressource.h"
#include "imnodes.h"
#include "imnodes_internal.h"

namespace lc
{
	class Event : public lc::Ressource
	{
		std::weak_ptr<lc::GameObject> m_objectA;
		std::string m_objectA_name;
		std::weak_ptr<lc::GameObject> m_objectB;
		std::string m_objectB_name;
		sf::Texture m_rendererTexture;
		short m_condition = -1;
		bool m_setFunction = false;
		bool m_isReverse = false;
		bool m_isOpen = false;
	public:
		Event();
		Event(std::string name, short function);
		~Event();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth);
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load);

		virtual void UpdateEvent(sf::Event& _window);
		virtual void Update(WindowManager& _window);
		virtual void Draw(WindowManager& _window);
		virtual void Draw(sf::RenderTexture& _window);
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		/*
		* @brief Open a window to set event's interaction.
		*/
		void setHierarchieFunc();
		std::vector<std::string> setContainer(std::shared_ptr<lc::GameObject> _scene);
		void setCondition();
	};
}

