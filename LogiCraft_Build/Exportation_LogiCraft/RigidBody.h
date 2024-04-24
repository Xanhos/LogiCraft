
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
#include <any>

#include "Ressource.h"


namespace lc
{
	class RigidBody : public lc::Ressource
	{
	public:
		RigidBody();
		RigidBody(sf::FloatRect _collider, sf::Vector2f _velocity, sf::Vector2f _relative, sf::Vector2f posBeforeSimulate, bool showCollide, bool kinetic);
		~RigidBody();

		void Load(std::ifstream& load) override;
		
		void UpdateEvent(sf::Event& _window) override;
		void Update(WindowManager& _window) override;
		void Draw(WindowManager& _window) override;
		void Draw(sf::RenderTexture& _window) override;
		std::shared_ptr<lc::GameComponent> Clone() override;

		virtual void AddInputFunction(std::function<void(lc::RigidBody*)> func);


		sf::Vector2f& getVelocity() { return m_velocity; }
		const sf::FloatRect getCollider() const { return m_collider; }
	private:
		bool CheckAllObject(std::shared_ptr<lc::GameObject> _object);

		sf::FloatRect m_collider;
		sf::Vector2f m_velocity;
		bool m_isKinetic = false;
		std::list<std::function<void(lc::RigidBody*)>> m_inputFunc;
	};

}

