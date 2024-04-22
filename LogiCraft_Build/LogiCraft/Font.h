#pragma once
#include "Ressource.h"

namespace lc
{
	class Font : public Ressource
	{
	public:
		Font();
		Font(std::string name, std::string path, std::string newRepo = "");
		Font(std::string name, std::string path, int character_size, int outline_thickness, std::string sentence, sf::Color insideColor, sf::Color outlineColor, sf::Vector2f _relative); // load constructor
		Font(const Font& font);
		~Font();
		sf::Font& getFont() { return m_font; }
		std::string getFontName() { return m_fontName; }
		std::string getFontPath() { return m_fontPath; }
		std::string& getSentence() { return m_sentence; }

		virtual void setHierarchieFunc();
		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void Load(std::ifstream& load) override;
	private:
		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;

		std::string m_fontName;
		sf::Font m_font;
		int m_characterSize;
		int m_outlineThickness;

		std::string m_fontPath;
		std::string m_sentence;
		sf::Color m_insideColor;
		sf::Color m_outLineColor;

		sf::Texture m_rendererTexture;
	};
}


