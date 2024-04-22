#pragma once
#include "Viewport.h"
#include "Ressource.h"
#include <functional>
#include "SFML_ENGINE/WindowManager.h"

namespace lc
{
	class Button : public Ressource
	{
	public:
		Button();
		Button(std::string _name, sf::Vector2f _position, sf::Vector2f _scale);
		Button(std::string _name, sf::Vector2f _relative, short _function); // load constructor
		~Button();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;
	private:
		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;

		std::string m_buttonName;
		short m_function = -1;
		bool m_showCollider = false;
		bool m_setFunction = false;
	};
}