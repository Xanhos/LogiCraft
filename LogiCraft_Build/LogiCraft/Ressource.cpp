#include "Ressource.h"
#include "GameObject.h"
#include "SFML_ENGINE/RessourcesManager.h"

void lc::Ressource::onDraw(WindowManager& _window, sf::Color color)
{
}

lc::Ressource::Ressource()
{
	m_typeName = "Ressources";
	m_displayThumbnail = false;
	m_type = Ressource::TYPE::NONE;
	m_isUsedByAComponent = false;
}

lc::Ressource::~Ressource()
{
}

void lc::Ressource::SaveRenderer(sf::RenderTexture& texture, int _depth)
{
	if(getParent())
		if(getParent()->getDepth() == _depth)
			if(m_renderer.getTexture())
				texture.draw(m_renderer);
}

void lc::Ressource::setPosition(sf::Vector2f pos)
{
	m_renderer.setPosition(pos);
}
