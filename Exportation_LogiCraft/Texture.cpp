
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

#include "Texture.h"
#include "GameObject.h"
#include "Tools_LC.h"


lc::Texture::Texture()
{
	m_renderer.setTexture(&m_texture);
}

lc::Texture::Texture(std::string name, std::string path, sf::IntRect rect )
{
	m_texture.loadFromFile(path);
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_name = name;
	m_typeName = "Texture";
	m_type = TYPE::TEXTURE;
}

lc::Texture::Texture(std::string name, const sf::Texture& texture, sf::IntRect rect)
{
	m_texture = texture;
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_name = name;
	m_typeName = "Texture";
	m_type = TYPE::TEXTURE;
}

lc::Texture::Texture(const Texture& _texture)
{
	m_texture = _texture.m_texture;
	m_textureRect = _texture.m_textureRect;
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_name = _texture.m_name;
	m_typeName = "Texture";
	m_type = TYPE::TEXTURE;
}


lc::Texture::~Texture()
{
}



void lc::Texture::Load(std::ifstream& load)
{
	load >>
		m_textureRect.left >>
		m_textureRect.top >>
		m_textureRect.width >>
		m_textureRect.height >>
		m_name >>
		m_relativePosition.x >>
		m_relativePosition.y;

	if (auto file = dynamic_cast<FileReader*>(&load))
	{
		m_texture.loadFromFile(file->getPath().parent_path().string() + "/Ressources/" + m_name);
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_type = TYPE::TEXTURE;
}

void lc::Texture::UpdateEvent(sf::Event& _window)
{
}

void lc::Texture::Update(WindowManager& _window)
{
	if (getParent())
		setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
}

void lc::Texture::Draw(WindowManager& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());

	_window.draw(m_renderer);
}

void lc::Texture::Draw(sf::RenderTexture& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());

	_window.draw(m_renderer);
}

std::shared_ptr<lc::GameComponent> lc::Texture::Clone()
{
	return std::make_shared<lc::Texture>(*this);
}
