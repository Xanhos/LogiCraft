
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
		enum class TYPE { NONE = -1, TEXTURE, IA, BUTTON, FONT, MUSIC, SOUND, ANIMATION, RIGIDBODY, EVENT, PARTICULES };
		protected:
		sf::RectangleShape m_renderer;//Renderer of the ressource
		bool m_isUsedByAComponent;
		sf::Vector2f m_relativePosition;//Relative position to the parent

	public:
		TYPE m_type;
		Ressource();
		virtual ~Ressource();

		virtual void Load(std::ifstream& load) = 0;

		virtual void UpdateEvent(sf::Event& _window) = 0;
		virtual void Update(WindowManager& _window) = 0;
		virtual void Draw(WindowManager& _window) = 0;
		virtual void Draw(sf::RenderTexture& _window) = 0;
		virtual std::shared_ptr<lc::GameComponent> Clone() = 0;

		sf::RectangleShape& getShape() { return m_renderer; };
		std::string& getName() { return m_name; };
		bool& isUsedByAComponent() { return m_isUsedByAComponent; }

		sf::Vector2f getRelativePosition();
		void setRelativePosition(sf::Vector2f pos);
		void setPosition(sf::Vector2f pos);
		sf::Vector2f getMaximumSize() { return  m_relativePosition + sf::Vector2f{m_renderer.getSize().x * m_renderer.getScale().x, m_renderer.getSize().y * m_renderer.getScale().y}; }

	};
}

