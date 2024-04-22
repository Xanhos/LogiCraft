#pragma once
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/RessourcesManager.h"
#include "Ressource.h"

namespace lc
{
	class Texture : public Ressource
	{
	public:
		Texture();
		Texture(std::string name, sf::IntRect rect, std::string path, sf::Vector2f _position, sf::Vector2f _scale, sf::Vector2f _relative); // load constructor
		Texture(std::string name, sf::IntRect rect, std::string path, std::string newRepo = "");
		Texture(const Texture& _texture);
		~Texture();
		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void Load(std::ifstream& load) override;

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;

		sf::Vector2f getTextureSize() { return (sf::Vector2f)m_textureRect.getSize(); }
		std::string getTextureName() { return m_textureName; }
		std::string getTexturePath() { return m_texturePath; }
		sf::Texture& getTexture() { return m_texture; }
		sf::IntRect getTextureRect() { return m_textureRect; }
	private:
		sf::Texture m_texture;
		std::string m_textureName;
		std::string m_texturePath;
		sf::IntRect m_textureRect;
	};
}

