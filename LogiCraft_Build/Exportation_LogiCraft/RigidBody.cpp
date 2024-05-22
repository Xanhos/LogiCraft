
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

#include "RigidBody.h"
#include "GameObject.h"

constexpr float g = 400.f;
constexpr float gravity_offset = -0.1f;

lc::RigidBody::RigidBody()
{
	m_name = "RigidBody";
	m_type = TYPE::RIGIDBODY;
	m_typeName = "RigidBody";
}

lc::RigidBody::RigidBody(sf::FloatRect _collider, sf::Vector2f _velocity, sf::Vector2f _relative, bool kinetic)
{
	m_name = "RigidBody";
	m_typeName = "RigidBody";
	m_collider = _collider;
	m_velocity = _velocity;
	m_relativePosition = _relative;
	m_isKinetic = kinetic;
	m_type = TYPE::RIGIDBODY;
}

lc::RigidBody::~RigidBody()
{
}


void lc::RigidBody::Load(std::ifstream& load)
{
	load
		>> m_collider.left
		>> m_collider.top
		>> m_collider.width
		>> m_collider.height
		>> m_relativePosition.x
		>> m_relativePosition.y
		>> m_isKinetic
		>> m_is_flying;
}

void lc::RigidBody::UpdateEvent(sf::Event& _window)
{
}

void lc::RigidBody::Update(WindowManager& _window)
{
	auto scene = getParent();
	while (scene->getParent())
	{
		scene = scene->getParent();
	}

	if(!m_collider.width && !m_collider.height)
	{
		m_collider.width = getParent()->getTransform().getSize().x;
		m_collider.height = getParent()->getTransform().getSize().y;
	}
	if (m_isKinetic)
	{
		for (auto& i : m_inputFunc)
		{
			if (i)
				i(this);
		}
		if(!m_is_flying)
			m_velocity.y += (g)*Tools::getDeltaTime();
		if (m_canCollide)
			CheckAllObject(scene);
	} 

	

	else m_velocity = { 0.f,0.f };





	if (m_isKinetic)
		if (getParent())
		{
			getParent()->getTransform().getPosition() += m_velocity * Tools::getDeltaTime();
		}
	if (getParent())
	{
		m_collider.left = getParent()->getTransform().getPosition().x + m_relativePosition.x;
		m_collider.top = getParent()->getTransform().getPosition().y + m_relativePosition.y;
	}


	if (m_velocity.x < 0)
	{
		if (getParent()->getTransform().getScale().x < 0)
			getParent()->getTransform().getScale().x = -getParent()->getTransform().getScale().x;

		getParent()->getTransform().getOrigin() = sf::Vector2f();
	}
	else if (m_velocity.x > 0)
	{
		if (getParent()->getTransform().getScale().x > 0)
			getParent()->getTransform().getScale().x = -getParent()->getTransform().getScale().x;

		getParent()->getTransform().getOrigin().x = m_collider.getSize().x;
		//std::cout << getParent()->getTransform().getOrigin().x << std::endl;
	}

	m_velocity.x =0;
}

void lc::RigidBody::Draw(WindowManager& _window)
{
	
}

void lc::RigidBody::Draw(sf::RenderTexture& _window)
{
	
}

std::shared_ptr<lc::GameComponent> lc::RigidBody::Clone()
{
	return std::make_shared<lc::RigidBody>(*this);
}


void lc::RigidBody::AddInputFunction(std::function<void(lc::RigidBody*)> func)
{
	m_inputFunc.push_back(func);
}

bool lc::RigidBody::CheckAllObject(std::shared_ptr<lc::GameObject> _object)
{
	for (auto& i : _object->getComponents())
	{
		if(&*i != this)
			if (auto rb = dynamic_cast<lc::RigidBody*>(&*i) )
			{
				if (rb->m_canCollide)
				{
					if (m_velocity.y > 0.f)
					{
						if (Tools::Collisions::lineRect({ m_collider.getPosition() + sf::Vector2f{0.f,m_collider.getSize().y} + sf::Vector2f{0.f,m_velocity.y} *Tools::getDeltaTime(), {{m_collider.getPosition() + m_collider.getSize() + sf::Vector2f{0.f,m_velocity.y}*Tools::getDeltaTime() }} }, { rb->m_collider.getPosition(), rb->m_collider.getSize() }))
						{
							m_velocity.y = 0.f;
							getParent()->getTransform().getPosition().y = rb->getCollider().getPosition().y - m_collider.getSize().y;
						}
					}
					else if (m_velocity.y < 0.f)
					{
						if (Tools::Collisions::lineRect({ m_collider.getPosition() + sf::Vector2f{0.f,m_velocity.y} *Tools::getDeltaTime(), {{m_collider.getPosition() + sf::Vector2f{m_collider.getSize().x,0.f} + sf::Vector2f{0.f,m_velocity.y}*Tools::getDeltaTime()}} }, { rb->m_collider.getPosition() , rb->m_collider.getSize() }))
						{
							m_velocity.y = 0.f;
						}
					}
					if (m_velocity.x < 0.f)
					{
						if (Tools::Collisions::lineRect({ m_collider.getPosition() + sf::Vector2f{m_velocity.x,0.f} *Tools::getDeltaTime(), {{m_collider.getPosition() + sf::Vector2f{0.f,m_collider.getSize().y + gravity_offset} + sf::Vector2f{m_velocity.x,0.f}*Tools::getDeltaTime()}} }, { rb->m_collider.getPosition(), rb->m_collider.getSize() }))
						{
							m_velocity.x = 0.f;
						}
					}
					else if (m_velocity.x > 0.f)
					{
						if (Tools::Collisions::lineRect({ m_collider.getPosition() + sf::Vector2f{m_collider.getSize().x,0.f} + sf::Vector2f{m_velocity.x,0.f} *Tools::getDeltaTime(), {{ m_collider.getPosition() + m_collider.getSize() + sf::Vector2f{m_velocity.x,gravity_offset}*Tools::getDeltaTime() }} }, { rb->m_collider.getPosition(), rb->m_collider.getSize() }))
						{
							m_velocity.x = 0.f;
						}
					}
				}
			}
	}

	for (auto& i : _object->getObjects())
		if(i->getName() != background_holder_name)
		CheckAllObject(i);

	return true;
}
