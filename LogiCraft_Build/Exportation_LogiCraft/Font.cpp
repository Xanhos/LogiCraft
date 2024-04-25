
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

#include "Font.h"
#include "GameObject.h"
#include "Transform.h"


lc::Font::Font()
	: m_characterSize(10), m_outlineThickness(0)
{
	m_type = TYPE::FONT;
	m_typeName = "Text";
}

lc::Font::Font(const std::string& name,const std::string& path,const std::string& sentence)
{
	m_typeName = "Text";
	fs::path p = path;
	m_name = name;
	m_font.loadFromFile(path);
	m_characterSize = 40u;
	m_type = TYPE::FONT;
	m_sentence = "Lorem Ipsum";
	m_insideColor = sf::Color::White;
	m_outlineThickness = 0;
	m_outLineColor = sf::Color::Black;

	UpdateText();

}


lc::Font::Font(const Font& font)
{
	m_typeName = "Text";
	m_font = font.m_font;
	m_characterSize = font.m_characterSize;
	m_sentence = font.m_sentence;
	m_insideColor = font.m_insideColor;
	m_outlineThickness = font.m_outlineThickness;
	m_outLineColor = font.m_outLineColor;
	m_relativePosition = font.m_relativePosition;
	m_rendererTexture = font.m_rendererTexture;
	m_renderer.setTexture(&m_rendererTexture);
	m_renderer.setSize(font.m_renderer.getSize());
	m_renderer.setPosition(font.m_renderer.getPosition());
	m_renderer.setRotation(font.m_renderer.getRotation());
	m_renderer.setScale(font.m_renderer.getScale());
	m_renderer.setOrigin(font.m_renderer.getOrigin());
	m_type = font.m_type;
	m_typeName = font.m_typeName;
	m_name = font.m_name;

	UpdateText();

}

lc::Font::~Font()
{
}




void lc::Font::setNewSentence(const std::string& sentence)
{
	m_sentence = sentence;
	UpdateText();
}

void lc::Font::Load(std::ifstream& load)
{
	int tmp[8];
	load >> m_name >> m_characterSize >> m_outlineThickness >> m_sentence >>
		tmp[0] >> tmp[1] >> tmp[2] >> tmp[3] >>
		tmp[4] >> tmp[5] >> tmp[6] >> tmp[7] >>
		m_relativePosition.x >> m_relativePosition.y;

	m_insideColor = sf::Color(tmp[0], tmp[1], tmp[2], tmp[3]);
	m_outLineColor = sf::Color(tmp[4], tmp[5], tmp[6], tmp[7]);


	if (const auto file = dynamic_cast<FileReader*>(&load))
	{
		m_font.loadFromFile(file->getPath().parent_path().string() + "/Ressources/" + m_name);
	}

	UpdateText();
}


void lc::Font::UpdateEvent(sf::Event& _window)
{
}

void lc::Font::Update(WindowManager& _window)
{
}

void lc::Font::Draw(WindowManager& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	_window.draw(m_renderer);
}

void lc::Font::Draw(sf::RenderTexture& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	_window.draw(m_renderer);
}

std::shared_ptr<lc::GameComponent> lc::Font::Clone()
{
	return std::make_shared<lc::Font>(*this);
}

void lc::Font::UpdateText()
{
	sf::RenderTexture rt;
	sf::Text text(m_sentence, m_font, static_cast<unsigned>(m_characterSize));
	text.setFillColor(m_insideColor);
	text.setOutlineColor(m_outLineColor);
	text.setOutlineThickness(static_cast<float>(m_outlineThickness));
	text.setOrigin(text.getGlobalBounds().getSize() / 2.f);
	const auto render_texture_size = sf::Vector2f(text.getGlobalBounds().getSize().x, text.getGlobalBounds().getSize().y + text.getFont()->getLineSpacing(m_characterSize));
	rt.create(static_cast<unsigned>(render_texture_size.x), static_cast<unsigned>(render_texture_size.y));
	text.setPosition(render_texture_size / 2.f);
	rt.clear(sf::Color(0, 0, 0, 0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture, true);
	m_renderer.setSize(text.getGlobalBounds().getSize() + sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing()));
	m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
}
