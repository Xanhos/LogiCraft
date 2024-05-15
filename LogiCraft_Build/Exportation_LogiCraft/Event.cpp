
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

#include "Event.h"

lc::Event::Event()
{
	m_name = "Event";
	m_typeName = "Event";
	m_type = TYPE::EVENT;
}

lc::Event::Event(std::string name, short function)
{
	m_name = "Event";
	m_typeName = "Event";
	m_type = TYPE::EVENT;

	sf::RenderTexture rt;
	sf::Text text(m_name, GET_MANAGER->getFont("arial"), 30);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);
	auto t = sf::Vector2f(text.getGlobalBounds().getSize().x, text.getGlobalBounds().getSize().y + text.getFont()->getLineSpacing(20));
	rt.create(static_cast<unsigned>(t.x), static_cast<unsigned>(t.y));

	rt.clear(sf::Color(0, 0, 0, 0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture, true);
	m_renderer.setSize(text.getGlobalBounds().getSize() + sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing()));
}



lc::Event::~Event()
{
}



void lc::Event::Load(std::ifstream& load)
{
	load >> m_condition >> m_isReverse >> m_objectA_name >> m_objectB_name;
	m_objectA_name = Tools::replaceSpace(m_objectA_name);
	m_objectB_name = Tools::replaceSpace(m_objectB_name);

	m_type = TYPE::EVENT;
}

void lc::Event::UpdateEvent(sf::Event& _window)
{
}

void lc::Event::Update(WindowManager& _window)
{
	if(m_objectA.expired())
	{
		m_objectA = lc::GameObject::GetRoot(getParent())->getObject(m_objectA_name);
	}
	if (m_objectB.expired())
	{
		m_objectB = lc::GameObject::GetRoot(getParent())->getObject(m_objectB_name);
	}
}

void lc::Event::Draw(WindowManager& _window)
{
	_window.draw(m_renderer);
}

void lc::Event::Draw(sf::RenderTexture& _window)
{
	//_window.draw(m_renderer);

}

std::shared_ptr<lc::GameComponent> lc::Event::Clone()
{
	return std::make_shared<lc::Event>(*this);
}

