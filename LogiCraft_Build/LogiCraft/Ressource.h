#pragma once
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace lc
{
	class Ressource : public lc::GameComponent
	{
	public:
		enum class TYPE { NONE = -1, TEXTURE, IA, BUTTON, FONT, MUSIC, SOUND, ANIMATION, RIGIDBODY, EVENT, PARTICULES_SYSTEM };
		protected:
		sf::RectangleShape m_renderer;//Renderer of the ressource
		bool m_displayThumbnail;
		bool m_isUsedByAComponent;
		sf::Vector2f m_thumbnailSize;
		sf::Vector2f m_relativePosition;//Relative position to the parent
		sf::Color m_filterColor;//Color of the filter in the content browser

		void onDraw(WindowManager& _window, sf::Color color);//Deprecated
	public:
		TYPE m_type;
		Ressource();
		~Ressource();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) = 0;
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override;
		virtual void Load(std::ifstream& load) = 0;

		virtual void UpdateEvent(sf::Event& _window) = 0;
		virtual void Update(WindowManager& _window) = 0;
		virtual void Draw(WindowManager& _window) = 0;
		virtual void Draw(sf::RenderTexture& _window) = 0;
		virtual std::shared_ptr<lc::GameComponent> Clone() = 0;

		sf::RectangleShape& getShape() { return m_renderer; };
		std::string& getName() { return m_name; };
		bool getThumbnail() { return m_displayThumbnail; }
		sf::Color getFilterColor() { return m_filterColor; }
		bool& isUsedByAComponent() { return m_isUsedByAComponent; }

		void ToggleThumbnail() { m_displayThumbnail = !m_displayThumbnail; }

		void setPosition(sf::Vector2f pos);
		void setSize(sf::Vector2f size) { m_thumbnailSize = size; }
		sf::Vector2f getMaximumSize() { return  m_relativePosition + sf::Vector2f{m_renderer.getSize().x * m_renderer.getScale().x, m_renderer.getSize().y * m_renderer.getScale().y}; }


	};
}

