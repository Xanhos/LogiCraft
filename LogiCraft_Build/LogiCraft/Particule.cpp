
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
	std::list<std::shared_ptr<Particle>> Particles::s_thread_particles_;

	bool Particles::s_thread_update_is_on_ = true;

	int Particles::s_number_of_particle_system_ = 0;

	std::thread Particles::s_update_thread_;

	sf::Clock Particles::s_update_clock_;

	sf::Time Particles::s_update_time_;

	Particle::Particle()
		: m_despawn_cooldown_(0.f), m_despawn_time_(0.f), m_gravity_force_(0.f), m_rotation_speed_(0.f), 
		m_has_gravity_(false), m_need_to_be_deleted_(false)
	{
	}

	Particle::Particle(
		float speed,
		float despawn_cooldown,
		float spawn_angle,
		float spawn_rotation,
		float rotation_speed,
		float gravity_force,
		bool has_gravity,
		sf::Vector2f spawn_position_in_viewport,
		sf::Vector2f spawn_position_in_render,
		sf::Vector2f spawn_origin)
		: m_despawn_cooldown_(despawn_cooldown), m_despawn_time_(0.f), m_gravity_force_(gravity_force),
		  m_rotation_speed_(rotation_speed),
		m_has_gravity_(has_gravity), m_need_to_be_deleted_(false)
	{
		m_transform_.getRotation() = spawn_rotation;
		m_transform_.getPosition() = spawn_position_in_viewport;
		m_transform_.getOrigin() = spawn_origin;

		m_renderer_transform_.getRotation() = spawn_rotation;
		m_renderer_transform_.getPosition() = spawn_position_in_render;
		m_renderer_transform_.getOrigin() = spawn_origin;

		m_velocity_ = sf::Vector2f(std::cos(spawn_angle * (3.14159265358f / 180)) * speed, std::sin(spawn_angle * (3.14159265358f / 180)) * speed);
	}

	Particle::~Particle()
	{
	}

	lc::Transform& Particle::get_transform()
	{
		return m_transform_;
	}

	lc::Transform& Particle::get_renderer_transform()
	{
		return m_renderer_transform_;
	}

	sf::Vector2f& Particle::get_velocity()
	{
		return m_velocity_;
	}

	float& Particle::get_despawn_cooldown()
	{
		return m_despawn_cooldown_;
	}

	float& Particle::get_despawn_time()
	{
		return m_despawn_time_;
	}

	float& Particle::get_gravity_force()
	{
		return m_gravity_force_;
	}

	float& Particle::get_rotation_speed()
	{
		return m_rotation_speed_;
	}

	bool& Particle::has_gravity()
	{
		return m_has_gravity_;
	}

	bool& Particle::need_to_be_deleted()
	{
		return m_need_to_be_deleted_;
	}

	Particles::Particles()
		: m_spawn_color_(sf::Color::White),
		  m_spawn_point_extend_size_(0.f),
		  m_spawn_cooldown_(0.1f),
		  m_spawn_timer_(0.f),
		  m_despawn_cooldown_(1.f),
		  m_rotation_speed_(10.f),
		  m_spawn_rotation_(0.f),
		  m_spawn_spread_(45.f),
		  m_spawn_angle_(0.f),
		  m_spawn_speed_(50.f),
		  m_base_shape_radius_(10.f),
		  m_life_time_timer_(0.f),
		  m_life_time_time_(5.f),
		  m_gravity_force_(300.f),
		  m_base_shape_point_count_(3),
		  m_spawn_count_(1u),
		  m_has_product_his_particles_(false),
		  m_has_gravity_(false),
		  m_is_particles_rendered_on_the_viewport_(true),
		  m_is_window_test_is_open_(false),
		  ressource_to_search_(std::make_pair(false, ""))
	{
		m_name = "Particles";
		m_typeName = "Particles";
		m_type = TYPE::PARTICULES;

		m_particles_type_ = ParticlesSystemType::NORMAL;

		m_spawn_point_particles_.setRadius(10.f);
		m_spawn_point_particles_.setOrigin(sf::Vector2f(10.f, 10.f));
		m_spawn_point_particles_.setFillColor({ 220u, 220u, 220u, 200u });
		m_spawn_point_particles_.setOutlineThickness(-1);

		m_spawn_point_particles_extend_.setSize(sf::Vector2f(0.f, 10.f));
		m_spawn_point_particles_extend_.setFillColor({ 220u, 220u, 220u, 200u });
		m_spawn_point_particles_extend_.setOutlineThickness(-1);

		m_base_shape_ = sf::CircleShape(m_base_shape_radius_, m_base_shape_point_count_);
		m_particles_origin_ = sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_);

		s_number_of_particle_system_++;
		if (s_number_of_particle_system_ == 1)
			Particles::s_update_thread_ = std::thread(&Particles::thread_update, this);
	}

	Particles::Particles(const ParticlesSystemType type)
		: m_spawn_color_(sf::Color::White),
		  m_spawn_point_extend_size_(0.f),
		  m_spawn_cooldown_(0.1f),
		  m_spawn_timer_(0.f),
		  m_despawn_cooldown_(1.f),
		  m_rotation_speed_(10.f),
		  m_spawn_rotation_(0.f),
		  m_spawn_spread_(45.f),
		  m_spawn_angle_(0.f),
		  m_spawn_speed_(50.f),
		  m_base_shape_radius_(10.f),
		  m_life_time_timer_(0.f),
		  m_life_time_time_(5.f),
		  m_gravity_force_(300.f),
		  m_base_shape_point_count_(3),
		  m_spawn_count_(1u),
		  m_has_product_his_particles_(false),
		  m_has_gravity_(false),
		  m_is_particles_rendered_on_the_viewport_(true),
		  m_is_window_test_is_open_(false),
		  ressource_to_search_(std::make_pair(false, ""))
	{
		m_name = "Particles";
		m_typeName = "Particles";
		m_type = TYPE::PARTICULES;
		m_particles_type_ = type;

		m_spawn_point_particles_ = sf::CircleShape(10.f);
		m_spawn_point_particles_.setOrigin(sf::Vector2f(10.f, 10.f));
		m_spawn_point_particles_.setFillColor({220u, 220u, 220u, 200u});
		m_spawn_point_particles_.setOutlineThickness(-1);

		m_spawn_point_particles_extend_.setSize(sf::Vector2f(0.f, 10.f));
		m_spawn_point_particles_extend_.setFillColor({ 220u, 220u, 220u, 200u });
		m_spawn_point_particles_extend_.setOutlineThickness(-1);

		m_base_shape_ = sf::CircleShape(m_base_shape_radius_, m_base_shape_point_count_);
		m_particles_origin_ = sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_);

		s_number_of_particle_system_++;
		if (s_number_of_particle_system_ == 1)
			Particles::s_update_thread_ = std::thread(&Particles::thread_update, this);
	}

	Particles::~Particles()
	{
		//Join the thread if no more particles system is used.
		s_number_of_particle_system_--;
		if (s_number_of_particle_system_ == 0)
		{
			Particles::s_thread_update_is_on_ = false;
			Particles::s_update_thread_.join();
			Particles::s_thread_particles_.clear();
		}

		if (!m_particles_ressource_.expired())
			m_particles_ressource_.lock()->isVisible() = false;

		m_particles_.clear();
	}

	void Particles::UpdateEvent(sf::Event& event)
	{
		//Event for the zoom and dezoom. 
		m_renderer_.UpdateZoom(event);
	}

	void Particles::Update(WindowManager& window)
	{
		this->texture_to_search();

		this->update_value();

		this->update_renderer_window();

		this->spawn_particles();
	}

	void Particles::Draw(WindowManager& window)
	{
	}

	void Particles::Draw(sf::RenderTexture& window)
	{
		if (this->particles_his_rendered())
		{
			m_renderer_.Clear();

			this->spawn_point_draw(window);

			this->particles_draw(window);

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
		if (!tmp_clone->m_particles_ressource_.expired())
			tmp_clone->ressource_to_search_ = std::make_pair(true, tmp_clone->m_particles_ressource_.lock()->getName());
		tmp_clone->m_particles_ressource_.reset();
		tmp_clone->m_particles_.clear();
		Particles::s_number_of_particle_system_++;
		return tmp_clone;
	}

	void Particles::setHierarchieFunc()
	{
		m_hierarchieInformation = [this]() 
			{
			ImGui::PushItemWidth(200.f);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (ImGui::Checkbox("Render On Viewport", &m_is_particles_rendered_on_the_viewport_))
			{
				if (m_is_particles_rendered_on_the_viewport_)
				{
					m_has_product_his_particles_ = false;
					m_life_time_timer_ = 0.f;
				}
				else
				{
					for (auto& particle : m_particles_)
						particle->need_to_be_deleted() = true;

					m_particles_.clear();
				}
			}
			
			ImGui::Checkbox("Gravity", &m_has_gravity_);

			if (ImGui::Button("Open Renderer Window"))
				m_is_window_test_is_open_ = true;

			if (ImGui::Button("Save Animation As A .ptcl"))
				this->save_animation_file();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			//Lock of the selected ressource (or not if there selected ressource)
			const auto tmp_ressource = m_particles_ressource_.lock();

			if (ImGui::BeginCombo("Selected Ressource", tmp_ressource ? tmp_ressource->getName().c_str() : "No Ressource Selected"))
			{
				const bool tmp_is_selected(false);

				if (tmp_ressource)
				{
					if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_is_selected))
					{
						if (tmp_ressource)
							tmp_ressource->isVisible() = false;

						//Reset of the origin to the one for the base shape.
						m_particles_origin_ = { m_base_shape_radius_, m_base_shape_radius_ };

						m_particles_ressource_.reset();
					}
				}

				for (const auto& component : getParent()->getComponents())
				{
					std::shared_ptr<lc::Ressource> tmp_ressource_component(std::dynamic_pointer_cast<lc::Texture>(component));
					if (!tmp_ressource_component)
						tmp_ressource_component = std::dynamic_pointer_cast<lc::Animation>(component);

					if (tmp_ressource_component)
					{
						//If the ressource is not already use by another component we use it.
						if (!tmp_ressource_component->isVisible())
						{
							if (ImGui::Selectable(std::string(tmp_ressource_component->getName() + "##" + std::to_string(tmp_ressource_component->getID())).c_str(), tmp_is_selected))
							{
								//If there were already a component used on the particles, we set it to un use,
								//to replace by the new one.
								if (tmp_ressource)
								{
									tmp_ressource->getShape().setFillColor(sf::Color::White);
									tmp_ressource->isVisible() = false;
								}

								//The new ressource is set to use.
								tmp_ressource_component->isVisible() = true;
								//The weak_ptr of the ressource is set to the new one,
								//the texture size is change to the new ressource,
								//and the particles origin is set to the half of the size.
								m_particles_ressource_ = tmp_ressource_component;
								m_texture_size_ = tmp_ressource_component->getShape().getSize();
								m_particles_origin_ = m_texture_size_ / 2.f;
							}
						}
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat2("Relative Position", m_relativePosition);
			ImGui::DragFloat2("Particles Origin", m_particles_origin_);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			static std::map<ParticlesSystemType, std::string> tmp_particlesTypeMap{ {ParticlesSystemType::NORMAL, "Normal"},{ParticlesSystemType::ONE_TIME, "One Time"}, {ParticlesSystemType::LIFE_TIME, "Life Time"} };
			
			if (ImGui::BeginCombo("Particles System Type", tmp_particlesTypeMap[m_particles_type_].c_str()))
			{
				const bool tmp_is_selected(false);

				for (const auto& particleType : tmp_particlesTypeMap)
				{
					if (ImGui::Selectable(particleType.second.c_str(), tmp_is_selected))
					{
						m_particles_type_ = particleType.first;

						m_has_product_his_particles_ = false;
						m_life_time_timer_ = 0.f;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Cooldown", &m_spawn_cooldown_);
			ImGui::DragFloat("Despawn Cooldown", &m_despawn_cooldown_);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Particles Speed", &m_spawn_speed_);
			ImGui::DragFloat("Rotation Speed", &m_rotation_speed_);
			if (m_has_gravity_)
				ImGui::DragFloat("Gravity Force", &m_gravity_force_);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragFloat("Spawn Point Extend Size", &m_spawn_point_extend_size_);
			ImGui::DragFloat("Spawn Particles Angle", &m_spawn_angle_);
			ImGui::DragFloat("Spawn Spread", &m_spawn_spread_);
			ImGui::DragFloat("Spawn Angle", &m_spawn_rotation_);
			ImGui::ColorEdit4("Spawn Color", m_spawn_color_, ImGuiColorEditFlags_AlphaBar);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			if (m_particles_type_ == ParticlesSystemType::LIFE_TIME)
				if (ImGui::DragFloat("Life Time Time", &m_life_time_time_))
					m_life_time_timer_ = 0.f;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::DragInt("Spawn Cout", &m_spawn_count_);
			if (m_particles_ressource_.expired())
			{
				if (ImGui::DragFloat("Base Shape Radius", &m_base_shape_radius_))
					m_base_shape_.setRadius(m_base_shape_radius_);

				if (ImGui::DragInt("Base Shape Point Count", &m_base_shape_point_count_))
				{
					if (m_base_shape_point_count_ < 3)
						m_base_shape_point_count_ = 3;

					m_base_shape_.setPointCount(m_base_shape_point_count_);
				}

				m_texture_size_ = { m_base_shape_radius_, m_base_shape_radius_ };
			}
			else
			{
				if (ImGui::DragFloat2("Texture Size", m_texture_size_))
					m_particles_ressource_.lock()->getShape().setSize(m_texture_size_);
			}

			if (ImGui::TreeNodeEx("Reset Button"))
			{
				if (ImGui::Button("Reset Spawn Cooldown"))
					m_spawn_cooldown_ = 0.1f;
				if (ImGui::Button("Reset Despawn Cooldown"))
					m_despawn_cooldown_ = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Particles Speed"))
					m_spawn_speed_ = 50.f;
				if (ImGui::Button("Reset Rotation Speed"))
					m_rotation_speed_ = 1.f;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Particles Angle"))
					m_spawn_angle_ = 0.f;
				if (ImGui::Button("Reset Spawn Spread"))
					m_spawn_spread_ = 45.f;
				if (ImGui::Button("Reset Spawn Angle"))
					m_spawn_rotation_ = 0.f;
				if (ImGui::Button("Reset Spawn Color"))
					m_spawn_color_ = sf::Color::White;

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (m_particles_type_ == ParticlesSystemType::ONE_TIME)
				{
					if (ImGui::Button("Redo Particles"))
						m_has_product_his_particles_ = false;
				}
				else if (m_particles_type_ == ParticlesSystemType::LIFE_TIME)
				{
					if (ImGui::Button("Reset Life Time Timer Particles"))
						m_life_time_timer_ = 5.f;
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::Button("Reset Spawn Cout"))
					m_spawn_count_ = 1;

				if (m_particles_ressource_.expired())
				{
					if (ImGui::Button("Reset Base Shape Radius"))
						m_base_shape_radius_ = 10.f;
					if (ImGui::Button("Reset Spawn Angle"))
						m_base_shape_point_count_ = 3;
				}

				ImGui::TreePop();
			}
			ImGui::PopItemWidth();
			};
	}

	void Particles::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
	{
		save << static_cast<int>(m_type)
			 << " " << m_typeName
			 << " " << static_cast<int>(m_particles_type_)
			 << " " << static_cast<int>(m_spawn_color_.r) 
			 << " " << static_cast<int>(m_spawn_color_.g) 
			 << " " << static_cast<int>(m_spawn_color_.b)
			 << " " << static_cast<int>(m_spawn_color_.a)
			 << " " << m_texture_size_.x
			 << " " << m_texture_size_.y
			 << " " << m_particles_origin_.x
			 << " " << m_particles_origin_.y
			 << " " << m_spawn_point_extend_size_
			 << " " << m_spawn_cooldown_
			 << " " << m_despawn_cooldown_
			 << " " << m_rotation_speed_
			 << " " << m_spawn_rotation_
			 << " " << m_spawn_spread_
			 << " " << m_spawn_angle_
			 << " " << m_spawn_speed_
			 << " " << m_base_shape_radius_
			 << " " << m_life_time_time_
			 << " " << m_gravity_force_
			 << " " << m_base_shape_point_count_
			 << " " << m_spawn_count_
			 << " " << m_has_gravity_
			 << " " << (!m_particles_ressource_.expired() ? m_particles_ressource_.lock()->getName() : std::string("No_Ressource"));
	}

	void Particles::Export(std::ofstream& exportation)
	{
		exportation << static_cast<int>(m_type)
			<< " " << static_cast<int>(m_particles_type_)
			<< " " << static_cast<int>(m_spawn_color_.r)
			<< " " << static_cast<int>(m_spawn_color_.g)
			<< " " << static_cast<int>(m_spawn_color_.b)
			<< " " << static_cast<int>(m_spawn_color_.a)
			<< " " << m_texture_size_.x
			<< " " << m_texture_size_.y
			<< " " << m_particles_origin_.x
			<< " " << m_particles_origin_.y
			<< " " << m_spawn_point_extend_size_
			<< " " << m_spawn_cooldown_
			<< " " << m_despawn_cooldown_
			<< " " << m_rotation_speed_
			<< " " << m_spawn_rotation_
			<< " " << m_spawn_spread_
			<< " " << m_spawn_angle_
			<< " " << m_spawn_speed_
			<< " " << m_base_shape_radius_
			<< " " << m_life_time_time_
			<< " " << m_gravity_force_
			<< " " << m_base_shape_point_count_
			<< " " << m_spawn_count_
			<< " " << m_has_gravity_
			<< " " << (!m_particles_ressource_.expired() ? m_particles_ressource_.lock()->getName() : std::string("No_Ressource"));
	}

	void Particles::Load(std::ifstream& load)
	{
		int tmp_ParticlesSystemType(0);
		std::string tmp_textureName;
		int tmp_color[4]{ 0, 0, 0, 0 };

		load >> m_typeName
			 >> tmp_ParticlesSystemType
			 >> tmp_color[0]
			 >> tmp_color[1]
			 >> tmp_color[2]
			 >> tmp_color[3]
			 >> m_texture_size_.x
			 >> m_texture_size_.y
			 >> m_particles_origin_.x
			 >> m_particles_origin_.y
			 >> m_spawn_point_extend_size_
			 >> m_spawn_cooldown_
			 >> m_despawn_cooldown_
			 >> m_rotation_speed_
			 >> m_spawn_rotation_
			 >> m_spawn_spread_
			 >> m_spawn_angle_
			 >> m_spawn_speed_
			 >> m_base_shape_radius_
			 >> m_life_time_time_
			 >> m_gravity_force_
			 >> m_base_shape_point_count_
			 >> m_spawn_count_
			 >> m_has_gravity_
			 >> tmp_textureName;

		if (tmp_textureName != "No_Ressource")
			ressource_to_search_ = std::make_pair(true, tmp_textureName);
		
		m_particles_type_ = static_cast<ParticlesSystemType>(tmp_ParticlesSystemType);
		m_spawn_color_ = sf::Color(static_cast<sf::Uint8>(tmp_color[0]), static_cast<sf::Uint8>(tmp_color[1]),
			static_cast<sf::Uint8>(tmp_color[2]), static_cast<sf::Uint8>(tmp_color[3]));
	}

	void Particles::save_animation_file(const bool open_file_browser, std::string path) const
	{
		std::string tmp_extention_name("ptcl");

		if (open_file_browser)
			Tools::IG::SaveRessourcesFromFile(path, tmp_extention_name.c_str());

		if (!path.empty())
		{
			//This is to separate the path and name of the file.
			std::string tmp_file_name(Tools::split_path_and_file_name(path));

			//This is to erase the .anim if is in the tmp_file_name.
			Tools::remove_extention(tmp_file_name, tmp_extention_name);

			if (!fs::exists(path + tmp_file_name))
				fs::create_directory(path + tmp_file_name);

			//Then we write every information in the .anim file.
			std::ofstream tmp_save_particles(path + '\\' + tmp_file_name + '\\' + tmp_file_name + "." + tmp_extention_name, std::ios::out);
			{
				tmp_save_particles << m_typeName << '\n';
				tmp_save_particles << m_spawn_color_ << '\n';
				tmp_save_particles << m_texture_size_ << '\n';
				tmp_save_particles << m_particles_origin_ << '\n';
				tmp_save_particles << m_has_gravity_ << '\n';
				tmp_save_particles << m_spawn_angle_ << '\n';
				tmp_save_particles << m_spawn_timer_ << '\n';
				tmp_save_particles << m_spawn_speed_ << '\n';
				tmp_save_particles << m_spawn_spread_ << '\n';
				tmp_save_particles << m_gravity_force_ << '\n';
				tmp_save_particles << m_spawn_cooldown_ << '\n';
				tmp_save_particles << m_rotation_speed_ << '\n';
				tmp_save_particles << m_life_time_time_ << '\n';
				tmp_save_particles << m_spawn_rotation_ << '\n';
				tmp_save_particles << m_despawn_cooldown_ << '\n';
				tmp_save_particles << m_base_shape_radius_ << '\n';
				tmp_save_particles << m_spawn_point_extend_size_ << '\n';
				tmp_save_particles << m_spawn_count_ << '\n';
				tmp_save_particles << m_base_shape_point_count_ << '\n';
				tmp_save_particles << (m_particles_ressource_.expired() ? "No_Ressource" : m_particles_ressource_.lock()->getName()) << '\n';
			}
			tmp_save_particles.close();

			//And the image is register.
			if (!m_particles_ressource_.expired())
			{
				if (auto tmp_animation = std::dynamic_pointer_cast<lc::Animation>(m_particles_ressource_.lock()))
				{
					tmp_animation->save_animation_file(false, path + tmp_file_name);

					if (tmp_animation->get_texture())
						tmp_animation->get_texture()->getTexture().copyToImage().saveToFile(path + '\\' + tmp_file_name + '\\' + tmp_file_name + ".png");
				}
				else if (auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(m_particles_ressource_.lock()))
				{
					tmp_texture->getTexture().copyToImage().saveToFile(path + '\\' + tmp_file_name + '\\' + tmp_file_name + ".png");
				}
			}
		}
	}

	void Particles::texture_to_search()
	{
		if (ressource_to_search_.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				std::shared_ptr<lc::Ressource> tmp_ressource(std::dynamic_pointer_cast<lc::Texture>(component));
				if (!tmp_ressource)
					tmp_ressource = std::dynamic_pointer_cast<lc::Animation>(component);

				if (tmp_ressource)
				{
					if (ressource_to_search_.second == tmp_ressource->getName())
					{
						m_particles_ressource_ = tmp_ressource;
						tmp_ressource->isVisible() = true;
						break;
					}
				}
			}

			ressource_to_search_.first = false;
		}
	}

	void Particles::update_value()
	{
		if (m_particles_type_ == ParticlesSystemType::LIFE_TIME && this->particles_his_rendered())
			m_life_time_timer_ += Tools::getDeltaTime();

		m_particles_spawn_center_ = (getParent()->getTransform().getPosition() + m_relativePosition);

		m_spawn_timer_ += Tools::getDeltaTime();
	}

	void Particles::update_renderer_window()
	{
		if (m_is_window_test_is_open_)
		{
			ImGui::Begin(std::string("Window Particles Tester##" + std::to_string(m_ID) + std::to_string(getParent()->getID()) + "lc").c_str(), &m_is_window_test_is_open_);

			m_renderer_.Update();

			ImGui::End();
		}
	}

	void Particles::spawn_particles()
	{
		//Spawn of the particles with all the type of particles spawner.
		if (m_spawn_timer_ > m_spawn_cooldown_ && 
		(m_particles_type_ == ParticlesSystemType::NORMAL || 
		(m_particles_type_ == ParticlesSystemType::LIFE_TIME && m_life_time_timer_ < m_life_time_time_) ||
		(m_particles_type_ == ParticlesSystemType::ONE_TIME && !m_has_product_his_particles_)))
		{
			if (m_particles_type_ == ParticlesSystemType::ONE_TIME)
				m_has_product_his_particles_ = true;

			if (this->particles_his_rendered())
				for (int i = 0; i < m_spawn_count_; i++)
				{
					const auto tmp_texture = m_particles_ressource_.lock();

					//tmp_spawn_center is the position to spawn in the center of the spawn point,
					//because you can modify the origin,
					//so if we have an origin of 10, 10 and a shape of a size of 30, 30 we divide 30 by 2,
					//so we obtain 15, 15, and we subtract this by the origin,
					//and we obtain 5, 5 then the center of the shape will 5 from the origin and then 10 + 5 obtain the center of the shape (30, 30 / 2).
					const sf::Vector2f tmp_spawn_center = (tmp_texture ?
						(tmp_texture->getShape().getSize() / 2.f) - m_particles_origin_ :
						sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_) - m_particles_origin_);

					//tmp_spawn_position is the spawn position of the particle.
					//we subtract the center by the tmp_spawn_center, so the particle will always start from the center of the shape.
					//Example :
					//spawn_center (200, 200) shape size (300, 300) shape origin (20, 20)
					//then tmp_spawn_center (130, 130) so spawn_position (70, 70)
					const sf::Vector2f tmp_spawn_position = 
						(m_particles_spawn_center_ - tmp_spawn_center) +
						get_extend_spawn_point(sf::Vector2f(Tools::Rand(-m_spawn_point_extend_size_ / 2.f, m_spawn_point_extend_size_ / 2.f), 0.f));

					const sf::Vector2f tmp_spawn_position_in_renderer =
						sf::Vector2f(m_renderer_.get_render_texture()->getSize()) / 2.f +
						get_extend_spawn_point(sf::Vector2f(Tools::Rand(-m_spawn_point_extend_size_ / 2.f, m_spawn_point_extend_size_ / 2.f), 0.f));

					const auto tmp_particle = std::make_shared<Particle>(
						m_spawn_speed_, m_despawn_cooldown_,
						m_spawn_rotation_ + Tools::Rand(-m_spawn_spread_ / 2.f, m_spawn_spread_ / 2.f),
						m_spawn_angle_, m_rotation_speed_, m_gravity_force_,
						m_has_gravity_,
						tmp_spawn_position, tmp_spawn_position_in_renderer, m_particles_origin_);

					m_particles_.push_back(tmp_particle);
					Particles::s_thread_particles_.push_back(tmp_particle);
				}

			m_spawn_timer_ = 0.f;
		}
	}

	void Particles::spawn_point_draw(sf::RenderTexture& window)
	{
		m_spawn_point_particles_extend_.setRotation(m_spawn_rotation_ - 90.f);
		m_spawn_point_particles_extend_.setSize(sf::Vector2f(m_spawn_point_extend_size_, m_spawn_point_particles_extend_.getSize().y));
		m_spawn_point_particles_extend_.setOrigin(sf::Vector2f(m_spawn_point_extend_size_, m_spawn_point_particles_extend_.getOrigin().y) / 2.f);

		m_spawn_point_particles_.setPosition(m_particles_spawn_center_);
		m_spawn_point_particles_extend_.setPosition(m_particles_spawn_center_);
		window.draw(m_spawn_point_particles_);
		window.draw(m_spawn_point_particles_extend_);

		m_spawn_point_particles_.setPosition(sf::Vector2f(m_renderer_.get_size()) / 2.f);
		m_spawn_point_particles_extend_.setPosition(sf::Vector2f(m_renderer_.get_render_texture()->getSize()) / 2.f);
		m_renderer_.Draw(m_spawn_point_particles_);
		m_renderer_.Draw(m_spawn_point_particles_extend_);
	}

	void Particles::particles_draw(sf::RenderTexture& window)
	{
		for (auto particle = m_particles_.begin(); particle != m_particles_.end();)
		{
			this->particle_draw(*particle, window);

			if ((*particle)->need_to_be_deleted())
				particle = m_particles_.erase(particle);
			else
				++particle;
		}
	}

	void Particles::particle_draw(const std::shared_ptr<Particle>& particle, sf::RenderTexture& window)
	{
		const auto tmp_ressource = m_particles_ressource_.lock();

		if (this->particles_his_rendered())
		{
			tmp_ressource ? tmp_ressource->getShape().setFillColor(m_spawn_color_) : m_base_shape_.setFillColor(m_spawn_color_);
			tmp_ressource ? tmp_ressource->getShape().setOrigin(particle->get_transform().getOrigin()) : m_base_shape_.setOrigin(particle->get_transform().getOrigin());
		}

		if (m_is_particles_rendered_on_the_viewport_)
		{
			tmp_ressource ? 
				tmp_ressource->getShape().setPosition(particle->get_transform().getPosition()) : 
				m_base_shape_.setPosition(particle->get_transform().getPosition());

			tmp_ressource ? 
				tmp_ressource->getShape().setRotation(particle->get_transform().getRotation()) : 
				m_base_shape_.setRotation(particle->get_transform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					window.getView().getCenter() - window.getView().getSize() / 2.f,
					window.getView().getSize()
				},
				{
					particle->get_transform().getPosition() - particle->get_transform().getOrigin(),
					tmp_ressource ? m_texture_size_ : sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_)
				}))
			{
				tmp_ressource ? window.draw(tmp_ressource->getShape()) : window.draw(m_base_shape_);
			}
		}

		if (m_is_window_test_is_open_)
		{
			tmp_ressource ? 
				tmp_ressource->getShape().setPosition(particle->get_renderer_transform().getPosition()) : 
				m_base_shape_.setPosition(particle->get_renderer_transform().getPosition());

			tmp_ressource ? 
				tmp_ressource->getShape().setRotation(particle->get_renderer_transform().getRotation()) : 
				m_base_shape_.setRotation(particle->get_renderer_transform().getRotation());

			if (Tools::Collisions::rect_rect(
				{
					m_renderer_.get_view().getCenter() - m_renderer_.get_view().getSize() / 2.f,
					m_renderer_.get_view().getSize()
				},
				{
					particle->get_renderer_transform().getPosition() - particle->get_transform().getOrigin(),
					tmp_ressource ? m_texture_size_ : sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_)
				}))
			{
				tmp_ressource ? m_renderer_.Draw(tmp_ressource->getShape()) : m_renderer_.Draw(m_base_shape_);
			}
		}
	}

	sf::Vector2f Particles::get_extend_spawn_point(const sf::Vector2f& center_position) const
	{
		if (m_spawn_point_extend_size_ > 0.f)
		{
			const float tmp_angle_rad((m_spawn_rotation_ - 90.f) * DEG2RAD);

			return {
				center_position.x * std::cosf(tmp_angle_rad) - center_position.y * std::sinf(tmp_angle_rad), 
				center_position.x * std::sinf(tmp_angle_rad) + center_position.y * std::cosf(tmp_angle_rad) };
		}

		return {};
	}

	bool Particles::particles_his_rendered() const
	{
		return m_is_particles_rendered_on_the_viewport_ || m_is_window_test_is_open_;
	}

	void Particles::thread_update()
	{
		while (s_number_of_particle_system_)
		{
			Particles::restart_thread_clock();

			for (auto particle = Particles::s_thread_particles_.begin(); particle != Particles::s_thread_particles_.end();)
			{
				if ((*particle)->has_gravity())
					(*particle)->get_velocity().y += (*particle)->get_gravity_force() * Particles::get_thread_delta_time();

				(*particle)->get_transform().getPosition() += (*particle)->get_velocity() * Particles::get_thread_delta_time();
				(*particle)->get_transform().getRotation() += (*particle)->get_rotation_speed() * Particles::get_thread_delta_time();
				(*particle)->get_renderer_transform().getPosition() += (*particle)->get_velocity() * Particles::get_thread_delta_time();
				(*particle)->get_renderer_transform().getRotation() += (*particle)->get_rotation_speed() * Particles::get_thread_delta_time();

				(*particle)->get_despawn_time() += Particles::get_thread_delta_time();
				if ((*particle)->get_despawn_time() > (*particle)->get_despawn_cooldown() || (*particle)->need_to_be_deleted())
				{
					(*particle)->need_to_be_deleted() = true;
					particle = Particles::s_thread_particles_.erase(particle);
				}
				else
					++particle;
			}
		}
	}

	void Particles::restart_thread_clock()
	{
		Particles::s_update_time_ = Particles::s_update_clock_.restart();
	}

	float Particles::get_thread_delta_time()
	{
		return Particles::s_update_time_.asSeconds();
	}
}