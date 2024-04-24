
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

#include "Particule.h"

namespace lc
{
	std::list<std::shared_ptr<Particle>> Particles::s_threadParticles;

	bool Particles::s_threadUpdateIsOn = true;

	int Particles::s_numberOfParticleSystem = 0;

	std::thread Particles::s_updateThread;

	sf::Clock Particles::s_updateClock;

	sf::Time Particles::s_updateTime;

	Particle::Particle()
		: m_despawnCooldown(0.f), m_despawnTime(0.f), m_gravityForce(0.f), m_rotationSpeed(0.f), 
		m_hasGravity(false), m_needToBeDeleted(false)
	{
	}

	Particle::Particle(
		float _speed,
		float _despawnCooldown,
		float _spawnAngle,
		float _spawnRotation,
		float _rotationSpeed,
		float _gravityForce,
		bool _hasGravity,
		sf::Vector2f _spawnPositionInViewport,
		sf::Vector2f _spawnPositionInRender,
		sf::Vector2f _spawnOrigin)
		: m_despawnCooldown(_despawnCooldown), m_despawnTime(0.f), m_gravityForce(_gravityForce),
		  m_rotationSpeed(_rotationSpeed),
		m_hasGravity(_hasGravity), m_needToBeDeleted(false)
	{
		m_transform.getRotation() = _spawnRotation;
		m_transform.getPosition() = _spawnPositionInViewport;
		m_transform.getOrigin() = _spawnOrigin;

		m_rendererTransform.getRotation() = _spawnRotation;
		m_rendererTransform.getPosition() = _spawnPositionInRender;
		m_rendererTransform.getOrigin() = _spawnOrigin;

		m_velocity = sf::Vector2f(std::cos(_spawnAngle * (3.14159265358f / 180)) * _speed, std::sin(_spawnAngle * (3.14159265358f / 180)) * _speed);
	}

	Particle::~Particle()
	{
	}

	lc::Transform& Particle::getTransform()
	{
		return m_transform;
	}

	lc::Transform& Particle::getRendererTransform()
	{
		return m_rendererTransform;
	}

	sf::Vector2f& Particle::getVelocity()
	{
		return m_velocity;
	}

	float& Particle::getDespawnCooldown()
	{
		return m_despawnCooldown;
	}

	float& Particle::getDespawnTime()
	{
		return m_despawnTime;
	}

	float& Particle::getGravityForce()
	{
		return m_gravityForce;
	}

	float& Particle::getRotationSpeed()
	{
		return m_rotationSpeed;
	}

	bool& Particle::hasGravity()
	{
		return m_hasGravity;
	}

	bool& Particle::needToBeDeleted()
	{
		return m_needToBeDeleted;
	}

	Particles::Particles()
		: m_spawnColor(sf::Color::White),
		  m_spawnPointExtendSize(0.f),
		  m_spawnCooldown(0.1f),
		  m_spawnTime(0.f),
		  m_despawnCooldown(1.f),
		  m_rotationSpeed(10.f),
		  m_spawnRotation(0.f),
		  m_spawnSpread(45.f),
		  m_spawnAngle(0.f),
		  m_spawnSpeed(50.f),
		  m_baseShapeRadius(10.f),
		  m_lifeTimeTimer(0.f),
		  m_lifeTimeTime(5.f),
		  m_gravityForce(300.f),
		  m_baseShapePointCount(3),
		  m_spawnCount(1u),
		  m_hasProductHisParticles(false),
		  m_hasGravity(false),
		  m_isParticlesRenderedOnTheViewport(true),
		  m_isWindowTestIsOpen(false),
		  m_hasToFoundHisTexture(std::make_pair(false, ""))
	{
		m_name = "Particles System";
		m_typeName = "Particles System";
		m_type = TYPE::PARTICULES;

		m_particlesType = ParticlesSystemType::NORMAL;

		m_spawnPointParticles = sf::CircleShape(10.f);
		m_spawnPointParticles.setOrigin(sf::Vector2f(10.f, 10.f));

		m_baseShape = sf::CircleShape(m_baseShapeRadius, m_baseShapePointCount);
		m_particlesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

		m_spawnPointParticlesExtend.setSize(sf::Vector2f(0.f, 10.f));

		s_numberOfParticleSystem++;
		if (s_numberOfParticleSystem == 1)
			Particles::s_updateThread = std::thread(&Particles::ThreadUpdate, this);
	}

	Particles::Particles(ParticlesSystemType _type)
		: m_spawnColor(sf::Color::White),
		  m_spawnPointExtendSize(0.f),
		  m_spawnCooldown(0.1f),
		  m_spawnTime(0.f),
		  m_despawnCooldown(1.f),
		  m_rotationSpeed(10.f),
		  m_spawnRotation(0.f),
		  m_spawnSpread(45.f),
		  m_spawnAngle(0.f),
		  m_spawnSpeed(50.f),
		  m_baseShapeRadius(10.f),
		  m_lifeTimeTimer(0.f),
		  m_lifeTimeTime(5.f),
		  m_gravityForce(300.f),
		  m_baseShapePointCount(3),
		  m_spawnCount(1u),
		  m_hasProductHisParticles(false),
		  m_hasGravity(false),
		  m_isParticlesRenderedOnTheViewport(true),
		  m_isWindowTestIsOpen(false),
		  m_hasToFoundHisTexture(std::make_pair(false, ""))
	{
		m_name = "Particles System";
		m_typeName = "Particles System";
		m_type = TYPE::PARTICULES;
		m_particlesType = _type;

		m_spawnPointParticles = sf::CircleShape(10.f);
		m_spawnPointParticles.setOrigin(sf::Vector2f(10.f, 10.f));

		m_baseShape = sf::CircleShape(m_baseShapeRadius, m_baseShapePointCount);
		m_particlesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

		m_spawnPointParticlesExtend.setSize(sf::Vector2f(0.f, 10.f));

		s_numberOfParticleSystem++;
		if (s_numberOfParticleSystem == 1)
			Particles::s_updateThread = std::thread(&Particles::ThreadUpdate, this);
	}

	Particles::~Particles()
	{
		//Join the thread if no more particles system is used.
		s_numberOfParticleSystem--;
		if (s_numberOfParticleSystem == 0)
		{
			Particles::s_threadUpdateIsOn = false;
			Particles::s_updateThread.join();
			Particles::s_threadParticles.clear();
		}

		m_particles.clear();
	}

	void Particles::UpdateEvent(sf::Event& _event)
	{
		//Event for the zoom and dezoom. 
		m_renderer_.UpdateZoom(_event);
	}

	void Particles::Update(WindowManager& _window)
	{
		this->HasToFoundHisTexture();

		this->UpdateValue();

		this->UpdateRendererWindow();

		this->SpawnParticles();
	}

	void Particles::Draw(WindowManager& _window)
	{
	}

	void Particles::Draw(sf::RenderTexture& _window)
	{
		if (this->ParticlesHisRendered())
		{
			m_renderer_.Clear();

			this->SpawnPointDraw(_window);

			this->particles_draw(_window);

			m_renderer_.Display();
		}
	}

	std::shared_ptr<lc::GameComponent> Particles::Clone()
	{
		auto tmp_clone = std::make_shared<lc::Particles>(*this);
		tmp_clone->m_renderer_.get_render_texture() = std::make_shared<sf::RenderTexture>();
		tmp_clone->m_renderer_.get_render_texture()->create(
			static_cast<unsigned int>(m_renderer_.get_size().x), 
			static_cast<unsigned int>(m_renderer_.get_size().y));
		Particles::s_numberOfParticleSystem++;
		return tmp_clone;
	}

	void Particles::setHierarchieFunc()
	{
		m_hierarchieInformation = [this]() 
			{
			ImGui::PushItemWidth(200.f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (ImGui::Checkbox("Render On Viewport", &m_isParticlesRenderedOnTheViewport))
			{
				if (m_isParticlesRenderedOnTheViewport)
				{
					m_hasProductHisParticles = false;
					m_lifeTimeTimer = 0.f;
				}
				else
				{
					for (auto& particle : m_particles)
						particle->needToBeDeleted() = true;

					m_particles.clear();
				}
			}
			
			ImGui::Checkbox("Gravity", &m_hasGravity);

			if (ImGui::Button("Open Renderer Window"))
				m_isWindowTestIsOpen = true;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
			
			bool tmp_isSelected(false);

			if (ImGui::BeginCombo("Selected Texture", m_particlesTexture.expired() ? "No Texture Selected" : m_particlesTexture.lock()->getName().c_str()))
			{
				if (!m_particlesTexture.expired())
				{
					if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_isSelected))
					{
						if (!m_particlesTexture.expired())
							m_particlesTexture.lock()->isUsedByAComponent() = false;

						m_particlesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

						m_particlesTexture.reset();
					}
				}

				for (auto& component : getParent()->getComponents())
				{
					if (auto tmp_textureComponent = std::dynamic_pointer_cast<lc::Texture>(component))
					{
						if (!tmp_textureComponent->isUsedByAComponent())
						{
							if (ImGui::Selectable(std::string(tmp_textureComponent->getName() + "##" + std::to_string(tmp_textureComponent->getID())).c_str(), tmp_isSelected))
							{
								if (!m_particlesTexture.expired())
									m_particlesTexture.lock()->isUsedByAComponent() = false;

								m_particlesTexture = tmp_textureComponent;
								tmp_textureComponent->isUsedByAComponent() = true;
								m_textureSize = tmp_textureComponent->getShape().getSize();
								m_particlesOrigin = m_textureSize / 2.f;
							}
						}
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat2("Relative Position", m_relativePosition);
			ImGui::DragFloat2("Particles Origin", m_particlesOrigin);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			static std::map<ParticlesSystemType, std::string> tmp_particlesTypeMap{ {ParticlesSystemType::NORMAL, "Normal"},{ParticlesSystemType::ONE_TIME, "One Time"}, {ParticlesSystemType::LIFE_TIME, "Life Time"} };
			tmp_isSelected = false;
			if (ImGui::BeginCombo("Particles System Type", tmp_particlesTypeMap[m_particlesType].c_str()))
			{
				for (auto& particleType : tmp_particlesTypeMap)
				{
					if (ImGui::Selectable(particleType.second.c_str(), tmp_isSelected))
					{
						m_particlesType = particleType.first;

						m_hasProductHisParticles = false;
						m_lifeTimeTimer = 0.f;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Cooldown", &m_spawnCooldown);
			ImGui::DragFloat("Despawn Cooldown", &m_despawnCooldown);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Particles Speed", &m_spawnSpeed);
			ImGui::DragFloat("Rotation Speed", &m_rotationSpeed);
			if (m_hasGravity)
				ImGui::DragFloat("Gravity Force", &m_gravityForce);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Point Extend Size", &m_spawnPointExtendSize);
			ImGui::DragFloat("Spawn Particles Angle", &m_spawnAngle);
			ImGui::DragFloat("Spawn Spread", &m_spawnSpread);
			ImGui::DragFloat("Spawn Angle", &m_spawnRotation);
			ImGui::ColorEdit4("Spawn Color", m_spawnColor, ImGuiColorEditFlags_AlphaBar);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (m_particlesType == ParticlesSystemType::LIFE_TIME)
				if (ImGui::DragFloat("Life Time Time", &m_lifeTimeTime))
					m_lifeTimeTimer = 0.f;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragInt("Spawn Cout", &m_spawnCount);
			if (m_particlesTexture.expired())
			{
				if (ImGui::DragFloat("Base Shape Radius", &m_baseShapeRadius))
					m_baseShape.setRadius(m_baseShapeRadius);

				if (ImGui::DragInt("Base Shape Point Count", &m_baseShapePointCount))
				{
					if (m_baseShapePointCount < 3)
						m_baseShapePointCount = 3;

					m_baseShape.setPointCount(m_baseShapePointCount);
				}

				m_textureSize = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);
			}
			else
			{
				if (ImGui::DragFloat2("Texture Size", m_textureSize))
					m_particlesTexture.lock()->getShape().setSize(m_textureSize);
			}

			if (ImGui::TreeNodeEx("Reset Button"))
			{
				if (ImGui::Button("Reset Spawn Cooldown"))
					m_spawnCooldown = 0.1f;
				if (ImGui::Button("Reset Despawn Cooldown"))
					m_despawnCooldown = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Particles Speed"))
					m_spawnSpeed = 50.f;
				if (ImGui::Button("Reset Rotation Speed"))
					m_rotationSpeed = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Particles Angle"))
					m_spawnAngle = 0.f;
				if (ImGui::Button("Reset Spawn Spread"))
					m_spawnSpread = 45.f;
				if (ImGui::Button("Reset Spawn Angle"))
					m_spawnRotation = 0.f;
				if (ImGui::Button("Reset Spawn Color"))
					m_spawnColor = sf::Color::White;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (m_particlesType == ParticlesSystemType::ONE_TIME)
				{
					if (ImGui::Button("Redo Particles"))
						m_hasProductHisParticles = false;
				}
				else if (m_particlesType == ParticlesSystemType::LIFE_TIME)
				{
					if (ImGui::Button("Reset Life Time Timer Particles"))
						m_lifeTimeTimer = 5.f;
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Cout"))
					m_spawnCount = 1;

				if (m_particlesTexture.expired())
				{
					if (ImGui::Button("Reset Base Shape Radius"))
						m_baseShapeRadius = 10.f;
					if (ImGui::Button("Reset Spawn Angle"))
						m_baseShapePointCount = 3;
				}

				ImGui::TreePop();
			}
			ImGui::PopItemWidth();
			};
	}

	void Particles::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
	{
		save << static_cast<int>(m_type)
			<< " " << m_typeName
			<< " " << static_cast<int>(m_particlesType)
			<< " " << static_cast<int>(m_spawnColor.r) 
			<< " " << static_cast<int>(m_spawnColor.g) 
			<< " " << static_cast<int>(m_spawnColor.b)
			<< " " << static_cast<int>(m_spawnColor.a)
			<< " " << m_textureSize.x
			<< " " << m_textureSize.y
			<< " " << m_particlesOrigin.x
			<< " " << m_particlesOrigin.y
			<< " " << m_spawnPointExtendSize
			<< " " << m_spawnCooldown
			<< " " << m_despawnCooldown
			<< " " << m_rotationSpeed
			<< " " << m_spawnRotation
			<< " " << m_spawnSpread
			<< " " << m_spawnAngle
			<< " " << m_spawnSpeed
			<< " " << m_baseShapeRadius
			<< " " << m_lifeTimeTime
			<< " " << m_gravityForce
			<< " " << m_baseShapePointCount
			<< " " << m_spawnCount
			<< " " << m_hasGravity
			<< " " << (!m_particlesTexture.expired() ? m_particlesTexture.lock()->getName() : std::string("No_Texture"));
	}

	void Particles::Load(std::ifstream& load)
	{
		int tmp_ParticlesSystemType(0);
		std::string tmp_textureName;
		int tmp_Color[4]{ 0, 0, 0, 0 };

		load >> m_typeName
			>> tmp_ParticlesSystemType
			>> tmp_Color[0]
			>> tmp_Color[1]
			>> tmp_Color[2]
			>> tmp_Color[3]
			>> m_textureSize.x
			>> m_textureSize.y
			>> m_particlesOrigin.x
			>> m_particlesOrigin.y
			>> m_spawnPointExtendSize
			>> m_spawnCooldown
			>> m_despawnCooldown
			>> m_rotationSpeed
			>> m_spawnRotation
			>> m_spawnSpread
			>> m_spawnAngle
			>> m_spawnSpeed
			>> m_baseShapeRadius
			>> m_lifeTimeTime
			>> m_gravityForce
			>> m_baseShapePointCount
			>> m_spawnCount
			>> m_hasGravity
			>> tmp_textureName;

		if (tmp_textureName != "No_Texture")
			m_hasToFoundHisTexture = std::make_pair(true, tmp_textureName);
		
		m_particlesType = static_cast<ParticlesSystemType>(tmp_ParticlesSystemType);
		m_spawnColor = sf::Color(tmp_Color[0], tmp_Color[1], tmp_Color[2], tmp_Color[3]);
	}

	void Particles::HasToFoundHisTexture()
	{
		if (m_hasToFoundHisTexture.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				if (auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
				{
					if (m_hasToFoundHisTexture.second == tmp_texture->getName())
					{
						m_particlesTexture = tmp_texture;
						tmp_texture->isUsedByAComponent() = true;
						break;
					}
				}
			}

			m_hasToFoundHisTexture.first = false;
		}
	}

	void Particles::UpdateValue()
	{
		if (m_particlesType == ParticlesSystemType::LIFE_TIME && this->ParticlesHisRendered())
			m_lifeTimeTimer += Tools::getDeltaTime();

		m_particlesSpawnCenter = (getParent()->getTransform().getPosition() + m_relativePosition);

		m_spawnTime += Tools::getDeltaTime();
	}

	void Particles::UpdateRendererWindow()
	{
		if (m_isWindowTestIsOpen)
		{
			ImGui::Begin(std::string("Window Particles Tester##" + std::to_string(m_ID) + std::to_string(getParent()->getID()) + "lc").c_str(), &m_isWindowTestIsOpen);

			m_renderer_.Update();

			ImGui::End();
		}
	}

	void Particles::SpawnParticles()
	{
		//Spawn of the particles with all the type of particles spawner.
		if (m_spawnTime > m_spawnCooldown && 
		(m_particlesType == ParticlesSystemType::NORMAL || 
		(m_particlesType == ParticlesSystemType::LIFE_TIME && m_lifeTimeTimer < m_lifeTimeTime) ||
		(m_particlesType == ParticlesSystemType::ONE_TIME && !m_hasProductHisParticles)))
		{
			if (m_particlesType == ParticlesSystemType::ONE_TIME)
				m_hasProductHisParticles = true;

			if (this->ParticlesHisRendered())
				for (int i = 0; i < m_spawnCount; i++)
				{
					const auto tmp_texture = m_particlesTexture.lock();

					const sf::Vector2f tmp_spawn_center = (tmp_texture ?
						(tmp_texture->getShape().getSize() / 2.f) - m_particlesOrigin :
						sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius) - m_particlesOrigin);

					const sf::Vector2f tmp_spawn_position = 
						(m_particlesSpawnCenter - tmp_spawn_center) +
						GetExtendSpawnPoint(sf::Vector2f(Tools::Rand(-m_spawnPointExtendSize / 2.f, m_spawnPointExtendSize / 2.f), 0.f));

					const sf::Vector2f tmp_spawn_position_in_renderer =
						sf::Vector2f(m_renderer_.get_render_texture()->getSize()) / 2.f +
						GetExtendSpawnPoint(sf::Vector2f(Tools::Rand(-m_spawnPointExtendSize / 2.f, m_spawnPointExtendSize / 2.f), 0.f));

					auto tmp_particule = std::make_shared<Particle>(
						m_spawnSpeed, m_despawnCooldown,
						m_spawnRotation + Tools::Rand(-m_spawnSpread / 2.f, m_spawnSpread / 2.f),
						m_spawnAngle, m_rotationSpeed, m_gravityForce,
						m_hasGravity,
						tmp_spawn_position, tmp_spawn_position_in_renderer, m_particlesOrigin);

					m_particles.push_back(tmp_particule);
					Particles::s_threadParticles.push_back(tmp_particule);
				}

			m_spawnTime = 0.f;
		}
	}

	void Particles::SpawnPointDraw(sf::RenderTexture& _window)
	{
		m_spawnPointParticlesExtend.setRotation(m_spawnRotation - 90.f);
		m_spawnPointParticlesExtend.setSize(sf::Vector2f(m_spawnPointExtendSize, m_spawnPointParticlesExtend.getSize().y));
		m_spawnPointParticlesExtend.setOrigin(sf::Vector2f(m_spawnPointExtendSize, m_spawnPointParticlesExtend.getOrigin().y) / 2.f);

		m_spawnPointParticles.setPosition(m_particlesSpawnCenter);
		m_spawnPointParticlesExtend.setPosition(m_particlesSpawnCenter);
		_window.draw(m_spawnPointParticles);
		_window.draw(m_spawnPointParticlesExtend);

		m_spawnPointParticles.setPosition(sf::Vector2f(m_renderer_.get_size()) / 2.f);
		m_spawnPointParticlesExtend.setPosition(sf::Vector2f(m_renderer_.get_render_texture()->getSize()) / 2.f);
		m_renderer_.Draw(m_spawnPointParticles);
		m_renderer_.Draw(m_spawnPointParticlesExtend);
	}

	void Particles::particles_draw(sf::RenderTexture& _window)
	{
		for (auto particle = m_particles.begin(); particle != m_particles.end();)
		{
			this->ParticleDraw(*particle, _window);

			if ((*particle)->needToBeDeleted())
				particle = m_particles.erase(particle);
			else
				++particle;
		}
	}

	void Particles::ParticleDraw(const std::shared_ptr<Particle>& _particle, sf::RenderTexture& _window)
	{
		auto tmp_texture = m_particlesTexture.lock();

		if (this->ParticlesHisRendered())
		{
			tmp_texture ? tmp_texture->getShape().setFillColor(m_spawnColor) : m_baseShape.setFillColor(m_spawnColor);
			tmp_texture ? tmp_texture->getShape().setOrigin(_particle->getTransform().getOrigin()) : m_baseShape.setOrigin(_particle->getTransform().getOrigin());
		}

		if (m_isParticlesRenderedOnTheViewport)
		{
			tmp_texture ? 
				tmp_texture->getShape().setPosition(_particle->getTransform().getPosition()) : 
				m_baseShape.setPosition(_particle->getTransform().getPosition());

			tmp_texture ? 
				tmp_texture->getShape().setRotation(_particle->getTransform().getRotation()) : 
				m_baseShape.setRotation(_particle->getTransform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					_window.getView().getCenter() - _window.getView().getSize() / 2.f,
					_window.getView().getSize()
				},
				{
					_particle->getTransform().getPosition() - _particle->getTransform().getOrigin(),
					tmp_texture ? m_textureSize : sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius)
				}))
			{
				tmp_texture ? _window.draw(tmp_texture->getShape()) : _window.draw(m_baseShape);
			}
		}

		if (m_isWindowTestIsOpen)
		{
			tmp_texture ? 
				tmp_texture->getShape().setPosition(_particle->getRendererTransform().getPosition()) : 
				m_baseShape.setPosition(_particle->getRendererTransform().getPosition());

			tmp_texture ? 
				tmp_texture->getShape().setRotation(_particle->getRendererTransform().getRotation()) : 
				m_baseShape.setRotation(_particle->getRendererTransform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					m_renderer_.get_view().getCenter() - m_renderer_.get_view().getSize() / 2.f,
					m_renderer_.get_view().getSize()
				},
				{
					_particle->getRendererTransform().getPosition() - _particle->getTransform().getOrigin(),
					tmp_texture ? m_textureSize : sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius)
				}))
			{
				tmp_texture ? m_renderer_.Draw(tmp_texture->getShape()) : m_renderer_.Draw(m_baseShape);
			}
		}
	}

	sf::Vector2f Particles::GetExtendSpawnPoint(sf::Vector2f _centerPosition) const
	{
		if (m_spawnPointExtendSize > 0.f)
		{
			const float tmp_angle_rad((m_spawnRotation - 90.f) * DEG2RAD);

			return {
				_centerPosition.x * std::cosf(tmp_angle_rad) - _centerPosition.y * std::sinf(tmp_angle_rad), 
				_centerPosition.x * std::sinf(tmp_angle_rad) + _centerPosition.y * std::cosf(tmp_angle_rad) };
		}

		return {};
	}

	bool Particles::ParticlesHisRendered() const
	{
		return m_isParticlesRenderedOnTheViewport || m_isWindowTestIsOpen;
	}

	void Particles::ThreadUpdate()
	{
		while (s_numberOfParticleSystem)
		{
			Particles::RestartThreadClock();

			for (auto particle = Particles::s_threadParticles.begin(); particle != Particles::s_threadParticles.end();)
			{
				if ((*particle)->hasGravity())
					(*particle)->getVelocity().y += (*particle)->getGravityForce() * Particles::GetThreadDeltaTime();

				(*particle)->getTransform().getPosition() += (*particle)->getVelocity() * Particles::GetThreadDeltaTime();
				(*particle)->getTransform().getRotation() += (*particle)->getRotationSpeed() * Particles::GetThreadDeltaTime();
				(*particle)->getRendererTransform().getPosition() += (*particle)->getVelocity() * Particles::GetThreadDeltaTime();
				(*particle)->getRendererTransform().getRotation() += (*particle)->getRotationSpeed() * Particles::GetThreadDeltaTime();

				(*particle)->getDespawnTime() += Particles::GetThreadDeltaTime();
				if ((*particle)->getDespawnTime() > (*particle)->getDespawnCooldown() || (*particle)->needToBeDeleted())
				{
					(*particle)->needToBeDeleted() = true;
					particle = Particles::s_threadParticles.erase(particle);
				}
				else
					++particle;
			}
		}
	}

	void Particles::RestartThreadClock()
	{
		Particles::s_updateTime = Particles::s_updateClock.restart();
	}

	float Particles::GetThreadDeltaTime()
	{
		return s_updateTime.asSeconds();
	}
}