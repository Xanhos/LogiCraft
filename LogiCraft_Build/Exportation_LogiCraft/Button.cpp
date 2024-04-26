
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

#include "Button.h"

lc::Button::Button()
{
    //m_renderer = sf::RectangleShape({ 100.f, 100.f });
    //m_renderer.setPosition(100.f,100.f);
   // m_renderer.setFillColor(sf::Color::White);
    m_type = TYPE::BUTTON;
    m_typeName = "Button";
}

lc::Button::Button(std::string _name, sf::Vector2f _position, int function)
{
    m_typeName = "Button";
    m_renderer.setPosition(_position);
    m_renderer.setFillColor(sf::Color::White);
    m_function = function;
    m_name = _name;
    m_type = TYPE::BUTTON;
}

lc::Button::~Button()
{
}


void lc::Button::Load(std::ifstream& load)
{
    load >> m_function;
}

void lc::Button::SetupFunctionPool(WindowManager& window)
{
    s_function_pool_[9] = [&window] { window.toggleFullscreen(); };
}

void lc::Button::UpdateEvent(sf::Event& _window)
{
}

void lc::Button::Update(WindowManager& _window)
{
    if (getParent())
    {
        m_renderer.setSize(getParent()->getTransform().getSize());
        m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
        m_renderer.setScale(getParent()->getTransform().getScale());
        m_renderer.setRotation(getParent()->getTransform().getRotation());
        m_renderer.setOrigin(getParent()->getTransform().getOrigin());
    }

    if(Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), m_renderer.getGlobalBounds()))
    {
        if(MOUSE(Left))
        {
        	s_function_pool_[m_function]();
		}

    }
}

void lc::Button::Draw(WindowManager& _window)
{
}

void lc::Button::Draw(sf::RenderTexture& _window)
{
    
}

std::shared_ptr<lc::GameComponent> lc::Button::Clone()
{
    return std::make_shared<lc::Button>(*this);
}

