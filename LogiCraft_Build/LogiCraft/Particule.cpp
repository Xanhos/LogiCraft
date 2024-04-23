
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
	std::list<std::shared_ptr<Particule>> Particules::s_threadParticules;

	bool Particules::s_threadUpdateIsOn = true;

	int Particules::s_numberOfParticuleSystem = 0;

	std::thread Particules::s_updateThread;

	sf::Clock Particules::s_updateClock;

	sf::Time Particules::s_updateTime;

	Particule::Particule()
		: m_despawnCooldown(0.f), m_despawnTime(0.f), m_rotationSpeed(0.f), m_gravityForce(0.f), 
		m_hasGravity(false), m_needToBeDeleted(false)
	{
	}

	Particule::Particule(
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
		: m_despawnCooldown(_despawnCooldown), m_despawnTime(0.f), m_rotationSpeed(_rotationSpeed), m_gravityForce(_gravityForce),
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

	Particule::~Particule()
	{
	}

	lc::Transform& Particule::getTransform()
	{
		return m_transform;
	}

	lc::Transform& Particule::getRendererTransform()
	{
		return m_rendererTransform;
	}

	sf::Vector2f& Particule::getVelocity()
	{
		return m_velocity;
	}

	float& Particule::getDespawnCooldown()
	{
		return m_despawnCooldown;
	}

	float& Particule::getDespawnTime()
	{
		return m_despawnTime;
	}

	float& Particule::getGravityForce()
	{
		return m_gravityForce;
	}

	float& Particule::getRotationSpeed()
	{
		return m_rotationSpeed;
	}

	bool& Particule::hasGravity()
	{
		return m_hasGravity;
	}

	bool& Particule::needToBeDeleted()
	{
		return m_needToBeDeleted;
	}

	Particules::Particules()
		: m_spawnSpeed(50.f),
		m_spawnCooldown(0.1f),
		m_spawnTime(0.f),
		m_despawnCooldown(1.f),
		m_rotationSpeed(10.f),
		m_spawnSpread(45.f),
		m_spawnAngle(0.f),
		m_spawnRotation(0.f),
		m_baseShapeRadius(10.f),
		m_lifeTimeTimer(0.f),
		m_lifeTimeTime(5.f),
		m_rendererZoom(1.f),
		m_gravityForce(300.f),
		m_spawnPointExtendSize(0.f),
		m_baseShapePointCount(3),
		m_spawnCount(1u),
		m_isParticulesRenderedOnTheViewport(true),
		m_isWindowTestIsOpen(false),
		m_hasProductHisParticules(false),
		m_renderderIsFocus(false),
		m_rendererIsGrabbed(false),
		m_hasGravity(false),
		m_hasToFoundHisTexture(std::make_pair(false, "")),
		m_spawnColor(sf::Color::White)
	{
		m_name = "Particules System";
		m_typeName = "Particules System";
		m_type = TYPE::PARTICULES;

		m_particulesType = ParticulesSystemType::NORMAL;

		m_rendererView = sf::View(sf::Vector2f(1920.f / 2.f, 1080.f / 2.f), sf::Vector2f(1920.f, 1080.f));
		m_spawnPointParticules = sf::CircleShape(10.f);
		m_spawnPointParticules.setOrigin(sf::Vector2f(10.f, 10.f));
		m_renderer = std::make_shared<sf::RenderTexture>();
		m_renderer->create(1920u, 1080u);

		m_baseShape = sf::CircleShape(m_baseShapeRadius, m_baseShapePointCount);
		m_particulesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

		m_spawnPointParticulesExtend.setSize(sf::Vector2f(0.f, 10.f));

		s_numberOfParticuleSystem++;
		if (s_numberOfParticuleSystem == 1)
			Particules::s_updateThread = std::thread(&Particules::ThreadUpdate, this);
	}

	Particules::Particules(ParticulesSystemType _type)
		: m_spawnSpeed(50.f),
		m_spawnCooldown(0.1f),
		m_spawnTime(0.f),
		m_despawnCooldown(1.f),
		m_rotationSpeed(10.f),
		m_spawnSpread(45.f),
		m_spawnAngle(0.f),
		m_spawnRotation(0.f),
		m_baseShapeRadius(10.f),
		m_lifeTimeTimer(0.f),
		m_lifeTimeTime(5.f),
		m_rendererZoom(1.f),
		m_gravityForce(300.f),
		m_spawnPointExtendSize(0.f),
		m_baseShapePointCount(3),
		m_spawnCount(1u),
		m_isParticulesRenderedOnTheViewport(true),
		m_isWindowTestIsOpen(false),
		m_hasProductHisParticules(false),
		m_renderderIsFocus(false),
		m_rendererIsGrabbed(false),
		m_hasGravity(false),
		m_hasToFoundHisTexture(std::make_pair(false, "")),
		m_spawnColor(sf::Color::White)
	{
		m_name = "Particules System";
		m_typeName = "Particules System";
		m_type = TYPE::PARTICULES;
		m_particulesType = _type;

		m_rendererView = sf::View(sf::Vector2f(1920.f / 2.f, 1080.f / 2.f), sf::Vector2f(1920.f, 1080.f));
		m_spawnPointParticules = sf::CircleShape(10.f);
		m_spawnPointParticules.setOrigin(sf::Vector2f(10.f, 10.f));
		m_renderer = std::make_shared<sf::RenderTexture>();
		m_renderer->create(1920u, 1080u);

		m_baseShape = sf::CircleShape(m_baseShapeRadius, m_baseShapePointCount);
		m_particulesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

		m_spawnPointParticulesExtend.setSize(sf::Vector2f(0.f, 10.f));

		s_numberOfParticuleSystem++;
		if (s_numberOfParticuleSystem == 1)
			Particules::s_updateThread = std::thread(&Particules::ThreadUpdate, this);
	}

	Particules::~Particules()
	{
		//Join the thread if no more particules system is used.
		s_numberOfParticuleSystem--;
		if (s_numberOfParticuleSystem == 0)
		{
			Particules::s_threadUpdateIsOn = false;
			Particules::s_updateThread.join();
			Particules::s_threadParticules.clear();
		}

		m_renderer.reset();
		m_particules.clear();
	}

	void Particules::UpdateEvent(sf::Event& _event)
	{
		//Event for the zoom and dezoom. 
		if (_event.type == sf::Event::MouseWheelScrolled && m_renderderIsFocus)
		{
			if (_event.mouseWheelScroll.delta > 0.f && m_rendererZoom > 0.2f)
				m_rendererZoom *= (1 / 1.1f);
			else if (_event.mouseWheelScroll.delta < 0.f && m_rendererZoom < 8.f)
				m_rendererZoom *= 1.1f;
		}
	}

	void Particules::Update(WindowManager& _window)
	{
		this->HasToFoundHisTexture();

		this->UpdateValue();

		this->UpdateRendererWindow();

		this->SpawnParticules();
	}

	void Particules::Draw(WindowManager& _window)
	{
	}

	void Particules::Draw(sf::RenderTexture& _window)
	{
		if (this->ParticulesHisRendered())
		{
			m_renderer->clear(sf::Color::Transparent);
			m_renderer->setView(m_rendererView);

			this->SpawnPointDraw(_window);

			this->ParticulesDraw(_window);

			m_renderer->display();
		}
	}

	std::shared_ptr<lc::GameComponent> Particules::Clone()
	{
		auto tmp_clone = std::make_shared<lc::Particules>(*this);
		tmp_clone->m_renderer = std::make_shared<sf::RenderTexture>();
		tmp_clone->m_renderer->create(this->m_renderer->getSize().x, this->m_renderer->getSize().y);
		Particules::s_numberOfParticuleSystem++;
		return tmp_clone;
	}

	void Particules::setHierarchieFunc()
	{
		m_hierarchieInformation = [this]() 
			{
			ImGui::PushItemWidth(200.f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (ImGui::Checkbox("Render On Viewport", &m_isParticulesRenderedOnTheViewport))
			{
				if (m_isParticulesRenderedOnTheViewport)
				{
					m_hasProductHisParticules = false;
					m_lifeTimeTimer = 0.f;
				}
				else
				{
					for (auto& particule : m_particules)
						particule->needToBeDeleted() = true;

					m_particules.clear();
				}
			}
			
			ImGui::Checkbox("Gravity", &m_hasGravity);

			if (ImGui::Button("Open Renderer Window"))
				m_isWindowTestIsOpen = true;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
			
			bool tmp_isSelected(false);

			if (ImGui::BeginCombo("Selected Texture", m_particulesTexture.expired() ? "No Texture Selected" : m_particulesTexture.lock()->getName().c_str()))
			{
				if (!m_particulesTexture.expired())
				{
					if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_isSelected))
					{
						if (!m_particulesTexture.expired())
							m_particulesTexture.lock()->isUsedByAComponent() = false;

						m_particulesOrigin = sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius);

						m_particulesTexture.reset();
					}
				}

				for (auto& component : getParent()->getComponents())
				{
					if (auto tmp_textureConponent = std::dynamic_pointer_cast<lc::Texture>(component))
					{
						if (ImGui::Selectable(std::string(tmp_textureConponent->getName() + "##" + std::to_string(tmp_textureConponent->getID())).c_str(), tmp_isSelected))
						{
							if (!m_particulesTexture.expired())
								m_particulesTexture.lock()->isUsedByAComponent() = false;

							m_particulesTexture = tmp_textureConponent;
							tmp_textureConponent->isUsedByAComponent() = true;
							m_textureSize = tmp_textureConponent->getShape().getSize();
							m_particulesOrigin = m_textureSize / 2.f;
						}
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat2("Relative Position", m_relativePosition);
			ImGui::DragFloat2("Particules Origin", m_particulesOrigin);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			static std::map<ParticulesSystemType, std::string> tmp_particulesTypeMap{ {ParticulesSystemType::NORMAL, "Normal"},{ParticulesSystemType::ONE_TIME, "One Time"}, {ParticulesSystemType::LIFE_TIME, "Life Time"} };
			tmp_isSelected = false;
			if (ImGui::BeginCombo("Particules System Type", tmp_particulesTypeMap[m_particulesType].c_str()))
			{
				for (auto& particuleType : tmp_particulesTypeMap)
				{
					if (ImGui::Selectable(particuleType.second.c_str(), tmp_isSelected))
					{
						m_particulesType = particuleType.first;

						m_hasProductHisParticules = false;
						m_lifeTimeTimer = 0.f;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Cooldown", &m_spawnCooldown);
			ImGui::DragFloat("Despawn Cooldown", &m_despawnCooldown);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Particules Speed", &m_spawnSpeed);
			ImGui::DragFloat("Rotation Speed", &m_rotationSpeed);
			if (m_hasGravity)
				ImGui::DragFloat("Gravity Force", &m_gravityForce);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Point Extend Size", &m_spawnPointExtendSize);
			ImGui::DragFloat("Spawn Particules Angle", &m_spawnAngle);
			ImGui::DragFloat("Spawn Spread", &m_spawnSpread);
			ImGui::DragFloat("Spawn Angle", &m_spawnRotation);
			ImGui::ColorEdit4("Spawn Color", m_spawnColor, ImGuiColorEditFlags_AlphaBar);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (m_particulesType == ParticulesSystemType::LIFE_TIME)
				if (ImGui::DragFloat("Life Time Time", &m_lifeTimeTime))
					m_lifeTimeTimer = 0.f;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragInt("Spawn Cout", &m_spawnCount);
			if (m_particulesTexture.expired())
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
					m_particulesTexture.lock()->getShape().setSize(m_textureSize);
			}

			if (ImGui::TreeNodeEx("Reset Button"))
			{
				if (ImGui::Button("Reset Spawn Cooldown"))
					m_spawnCooldown = 0.1f;
				if (ImGui::Button("Reset Despawn Cooldown"))
					m_despawnCooldown = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Particules Speed"))
					m_spawnSpeed = 50.f;
				if (ImGui::Button("Reset Rotation Speed"))
					m_rotationSpeed = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Particules Angle"))
					m_spawnAngle = 0.f;
				if (ImGui::Button("Reset Spawn Spread"))
					m_spawnSpread = 45.f;
				if (ImGui::Button("Reset Spawn Angle"))
					m_spawnRotation = 0.f;
				if (ImGui::Button("Reset Spawn Color"))
					m_spawnColor = sf::Color::White;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (m_particulesType == ParticulesSystemType::ONE_TIME)
				{
					if (ImGui::Button("Redo Particules"))
						m_hasProductHisParticules = false;
				}
				else if (m_particulesType == ParticulesSystemType::LIFE_TIME)
				{
					if (ImGui::Button("Reset Life Time Timer Particules"))
						m_lifeTimeTimer = 5.f;
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Cout"))
					m_spawnCount = 1;

				if (m_particulesTexture.expired())
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

	void Particules::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
	{
		save << static_cast<int>(m_type)
			<< " " << static_cast<int>(m_particulesType)
			<< " " << static_cast<int>(m_spawnColor.r) 
			<< " " << static_cast<int>(m_spawnColor.g) 
			<< " " << static_cast<int>(m_spawnColor.b)
			<< " " << static_cast<int>(m_spawnColor.a)
			<< " " << m_textureSize.x
			<< " " << m_textureSize.y
			<< " " << m_particulesOrigin.x
			<< " " << m_particulesOrigin.y
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
			<< " " << (!m_particulesTexture.expired() ? m_particulesTexture.lock()->getName() : std::string("No_Texture"));
	}

	void Particules::Load(std::ifstream& load)
	{
		int tmp_ParticulesSystemType(0);
		std::string tmp_textureName;
		int tmp_Color[4]{ 0, 0, 0, 0 };

		load >> tmp_ParticulesSystemType
			>> tmp_Color[0]
			>> tmp_Color[1]
			>> tmp_Color[2]
			>> tmp_Color[3]
			>> m_textureSize.x
			>> m_textureSize.y
			>> m_particulesOrigin.x
			>> m_particulesOrigin.y
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
		
		m_particulesType = static_cast<ParticulesSystemType>(tmp_ParticulesSystemType);
		m_spawnColor = sf::Color(tmp_Color[0], tmp_Color[1], tmp_Color[2], tmp_Color[3]);
	}

	void Particules::HasToFoundHisTexture()
	{
		if (m_hasToFoundHisTexture.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				if (auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
				{
					if (m_hasToFoundHisTexture.second == tmp_texture->getName())
					{
						m_particulesTexture = tmp_texture;
						tmp_texture->isUsedByAComponent() = true;
						break;
					}
				}
			}

			m_hasToFoundHisTexture.first = false;
		}
	}

	void Particules::UpdateValue()
	{
		if (m_particulesType == ParticulesSystemType::LIFE_TIME && this->ParticulesHisRendered())
			m_lifeTimeTimer += Tools::getDeltaTime();

		m_particulesSpawnCenter = (getParent()->getTransform().getPosition() + m_relativePosition);

		m_spawnTime += Tools::getDeltaTime();
	}

	void Particules::UpdateRendererWindow()
	{
		if (m_isWindowTestIsOpen)
		{
			ImGui::Begin(std::string("Window Particules Tester##" + std::to_string(m_ID) + std::to_string(getParent()->getID()) + "lc").c_str(), &m_isWindowTestIsOpen);

			this->UpdateRenderer();

			this->RendererMovement();

			ImGui::Image(*m_renderer, m_rendererSize);

			ImGui::End();
		}
	}

	void Particules::SpawnParticules()
	{
		//Spawn of the particules with all the type of particules spawner.
		if (m_spawnTime > m_spawnCooldown && 
		(m_particulesType == ParticulesSystemType::NORMAL || 
		(m_particulesType == ParticulesSystemType::LIFE_TIME && m_lifeTimeTimer < m_lifeTimeTime) ||
		(m_particulesType == ParticulesSystemType::ONE_TIME && !m_hasProductHisParticules)))
		{
			if (m_particulesType == ParticulesSystemType::ONE_TIME)
				m_hasProductHisParticules = true;

			if (this->ParticulesHisRendered())
				for (int i = 0; i < m_spawnCount; i++)
				{
					auto tmp_texture = m_particulesTexture.lock();

					sf::Vector2f tmp_spawnCenter = (tmp_texture ?
						(tmp_texture->getShape().getSize() / 2.f) - m_particulesOrigin :
						sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius) - m_particulesOrigin);

					sf::Vector2f tmp_spawnPosition = 
						(m_particulesSpawnCenter - tmp_spawnCenter) +
						GetExtendSpawnPoint(sf::Vector2f(Tools::Rand(-m_spawnPointExtendSize / 2.f, m_spawnPointExtendSize / 2.f), 0.f));

					sf::Vector2f tmp_spawnPositionInRenderer =
						sf::Vector2f(m_renderer->getSize()) / 2.f +
						GetExtendSpawnPoint(sf::Vector2f(Tools::Rand(-m_spawnPointExtendSize / 2.f, m_spawnPointExtendSize / 2.f), 0.f));

					auto tmp_particule = std::make_shared<Particule>(
						m_spawnSpeed, m_despawnCooldown,
						m_spawnRotation + Tools::Rand(-m_spawnSpread / 2.f, m_spawnSpread / 2.f),
						m_spawnAngle, m_rotationSpeed, m_gravityForce,
						m_hasGravity,
						tmp_spawnPosition, tmp_spawnPositionInRenderer, m_particulesOrigin);

					m_particules.push_back(tmp_particule);
					Particules::s_threadParticules.push_back(tmp_particule);
				}

			m_spawnTime = 0.f;
		}
	}

	void Particules::UpdateRenderer()
	{
		m_renderderIsFocus = ImGui::IsWindowFocused();

		m_rendererView.setCenter((m_rendererSize / 2.f + m_redererAddedPosition) * m_rendererZoom);
		m_rendererView.setSize(m_rendererSize * m_rendererZoom);

		if (m_rendererSize != sf::Vector2f(ImGui::GetContentRegionAvail()))
		{
			m_rendererSize = ImGui::GetContentRegionAvail();
			m_renderer->create(static_cast<unsigned>(m_rendererSize.x), static_cast<unsigned>(m_rendererSize.y));
		}
	}

	void Particules::RendererMovement()
	{
		if (MOUSE(Right))
		{
			//If mouse right is press and its in the window bounds.
			if (Tools::Collisions::point_rect(ImGui::GetMousePos(), { ImGui::GetWindowPos(), ImGui::GetWindowSize() }) && !m_rendererIsGrabbed && !Tools::CameraGrabbed)
			{
				//And the mouse is not on another window the camera is grabbed.
				if (!Tools::IG::MouseIsOnAboveWindow())
				{
					Tools::CameraGrabbed = true;
					m_rendererIsGrabbed = true;
					m_redererLastAddedPosition = m_redererAddedPosition;
					ImGui::SetWindowFocus();
				}
			}
			else if (m_rendererIsGrabbed)
			{
				m_redererAddedPosition = -sf::Vector2f(ImGui::GetMouseDragDelta(ImGuiMouseButton_Right)) + m_redererLastAddedPosition;
			}
		}
		else
		{
			Tools::CameraGrabbed = false;
			m_rendererIsGrabbed = false;
		}
	}

	void Particules::SpawnPointDraw(sf::RenderTexture& _window)
	{
		m_spawnPointParticulesExtend.setRotation(m_spawnRotation - 90.f);
		m_spawnPointParticulesExtend.setSize(sf::Vector2f(m_spawnPointExtendSize, m_spawnPointParticulesExtend.getSize().y));
		m_spawnPointParticulesExtend.setOrigin(sf::Vector2f(m_spawnPointExtendSize, m_spawnPointParticulesExtend.getOrigin().y) / 2.f);

		m_spawnPointParticules.setPosition(m_particulesSpawnCenter);
		m_spawnPointParticulesExtend.setPosition(m_particulesSpawnCenter);
		_window.draw(m_spawnPointParticules);
		_window.draw(m_spawnPointParticulesExtend);

		m_spawnPointParticules.setPosition(sf::Vector2f(m_renderer->getSize()) / 2.f);
		m_spawnPointParticulesExtend.setPosition(sf::Vector2f(m_renderer->getSize()) / 2.f);
		m_renderer->draw(m_spawnPointParticules);
		m_renderer->draw(m_spawnPointParticulesExtend);
	}

	void Particules::ParticulesDraw(sf::RenderTexture& _window)
	{
		for (auto particule = m_particules.begin(); particule != m_particules.end();)
		{
			this->ParticuleDraw(*particule, _window, *m_renderer);

			if ((*particule)->needToBeDeleted())
				particule = m_particules.erase(particule);
			else
				particule++;
		}
	}

	void Particules::ParticuleDraw(std::shared_ptr<Particule> _particule, sf::RenderTexture& _window, sf::RenderTexture& _renderer)
	{
		auto tmp_texture = m_particulesTexture.lock();

		if (this->ParticulesHisRendered())
		{
			tmp_texture ? tmp_texture->getShape().setFillColor(m_spawnColor) : m_baseShape.setFillColor(m_spawnColor);
			tmp_texture ? tmp_texture->getShape().setOrigin(_particule->getTransform().getOrigin()) : m_baseShape.setOrigin(_particule->getTransform().getOrigin());
		}

		if (m_isParticulesRenderedOnTheViewport)
		{
			tmp_texture ? 
				tmp_texture->getShape().setPosition(_particule->getTransform().getPosition()) : 
				m_baseShape.setPosition(_particule->getTransform().getPosition());

			tmp_texture ? 
				tmp_texture->getShape().setRotation(_particule->getTransform().getRotation()) : 
				m_baseShape.setRotation(_particule->getTransform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					_window.getView().getCenter() - _window.getView().getSize() / 2.f,
					_window.getView().getSize()
				},
				{
					_particule->getTransform().getPosition() - _particule->getTransform().getOrigin(),
					tmp_texture ? m_textureSize : sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius)
				}))
			{
				tmp_texture ? _window.draw(tmp_texture->getShape()) : _window.draw(m_baseShape);
			}
		}

		if (m_isWindowTestIsOpen)
		{
			tmp_texture ? 
				tmp_texture->getShape().setPosition(_particule->getRendererTransform().getPosition()) : 
				m_baseShape.setPosition(_particule->getRendererTransform().getPosition());

			tmp_texture ? 
				tmp_texture->getShape().setRotation(_particule->getRendererTransform().getRotation()) : 
				m_baseShape.setRotation(_particule->getRendererTransform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					_renderer.getView().getCenter() - _renderer.getView().getSize() / 2.f,
					_renderer.getView().getSize()
				},
				{
					_particule->getRendererTransform().getPosition() - _particule->getTransform().getOrigin(),
					tmp_texture ? m_textureSize : sf::Vector2f(m_baseShapeRadius, m_baseShapeRadius)
				}))
			{
				tmp_texture ? _renderer.draw(tmp_texture->getShape()) : _renderer.draw(m_baseShape);
			}
		}
	}

	sf::Vector2f Particules::GetExtendSpawnPoint(sf::Vector2f _centerPosition)
	{
		if (m_spawnPointExtendSize > 0.f)
		{
			float tmp_angleRad((m_spawnRotation - 90.f) * DEG2RAD);

			return sf::Vector2f(
				_centerPosition.x * std::cosf(tmp_angleRad) - _centerPosition.y * std::sinf(tmp_angleRad), 
				_centerPosition.x * std::sinf(tmp_angleRad) + _centerPosition.y * std::cosf(tmp_angleRad));
		}

		return sf::Vector2f();
	}

	bool Particules::ParticulesHisRendered() const
	{
		return m_isParticulesRenderedOnTheViewport || m_isWindowTestIsOpen;
	}

	void Particules::ThreadUpdate()
	{
		while (s_numberOfParticuleSystem)
		{
			Particules::RestartThreadClock();

			for (auto particule = Particules::s_threadParticules.begin(); particule != Particules::s_threadParticules.end();)
			{
				if ((*particule)->hasGravity())
					(*particule)->getVelocity().y += (*particule)->getGravityForce() * Particules::GetThreadDeltaTime();

				(*particule)->getTransform().getPosition() += (*particule)->getVelocity() * Particules::GetThreadDeltaTime();
				(*particule)->getTransform().getRotation() += (*particule)->getRotationSpeed() * Particules::GetThreadDeltaTime();
				(*particule)->getRendererTransform().getPosition() += (*particule)->getVelocity() * Particules::GetThreadDeltaTime();
				(*particule)->getRendererTransform().getRotation() += (*particule)->getRotationSpeed() * Particules::GetThreadDeltaTime();

				(*particule)->getDespawnTime() += Particules::GetThreadDeltaTime();
				if ((*particule)->getDespawnTime() > (*particule)->getDespawnCooldown() || (*particule)->needToBeDeleted())
				{
					(*particule)->needToBeDeleted() = true;
					particule = Particules::s_threadParticules.erase(particule);
				}
				else
					particule++;
			}
		}
	}

	void Particules::RestartThreadClock()
	{
		Particules::s_updateTime = Particules::s_updateClock.restart();
	}

	float Particules::GetThreadDeltaTime()
	{
		return s_updateTime.asSeconds();
	}
}