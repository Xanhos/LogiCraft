
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

#include "Animation.h"

namespace lc
{
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
		sf::Vector2f spawn_origin)
		: m_despawn_cooldown_(despawn_cooldown), m_despawn_time_(0.f), m_gravity_force_(gravity_force),
		  m_rotation_speed_(rotation_speed),
		m_has_gravity_(has_gravity), m_need_to_be_deleted_(false)
	{
		m_transform_.getRotation() = spawn_rotation;
		m_transform_.getPosition() = spawn_position_in_viewport;
		m_transform_.getOrigin() = spawn_origin;

		m_velocity_ = sf::Vector2f(std::cosf(spawn_angle * (3.14159265358f / 180.f)) * speed, std::sinf(spawn_angle * (3.14159265358f / 180.f)) * speed);
	}

	Particle::~Particle()
	{
	}

	lc::Transform& Particle::get_transform()
	{
		return m_transform_;
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
		  m_ressource_to_search_(std::make_pair(false, ""))
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
		  m_ressource_to_search_(std::make_pair(false, ""))
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
	}

	Particles::~Particles()
	{
		if (!m_particles_ressource_.expired())
			m_particles_ressource_.lock()->isVisible(true);

		m_particles_.clear();
	}

	void Particles::UpdateEvent(sf::Event& event)
	{
	}

	void Particles::Update(WindowManager& window)
	{
		this->texture_to_search();

		this->update_value();
		
		this->spawn_particles();
	}

	void Particles::Draw(WindowManager& window)
	{
		this->particles_draw(window);
	}

	void Particles::Draw(sf::RenderTexture& window)
	{
		this->particles_draw(window);
	}

	std::shared_ptr<lc::GameComponent> Particles::Clone()
	{
		auto tmp_clone = std::make_shared<lc::Particles>(*this);
		if (!tmp_clone->m_particles_ressource_.expired())
			tmp_clone->m_ressource_to_search_ = std::make_pair(true, tmp_clone->m_particles_ressource_.lock()->getName());
		tmp_clone->m_particles_ressource_.reset();
		tmp_clone->m_particles_.clear();
		return tmp_clone;
	}


	void Particles::Load(std::ifstream& load)
	{
		int tmp_ParticlesSystemType(0);
		std::string tmp_textureName;

		load >> tmp_ParticlesSystemType
			 >> m_spawn_color_
			 >> m_texture_size_
			 >> m_particles_origin_
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
			>> m_relativePosition
			 >> tmp_textureName;

		if (tmp_textureName != "No_Ressource")
			m_ressource_to_search_ = std::make_pair(true, tmp_textureName);
		
		m_particles_type_ = static_cast<ParticlesSystemType>(tmp_ParticlesSystemType);
	}

	void Particles::load_particles_file(std::string path)
	{
		std::ifstream file(path);
		if(file.is_open() and fs::path(path).extension() == ".ptcl")
		{
			std::string tmp_resources_type, tmp_resources_name;
			
			file >> m_typeName >> m_spawn_color_ >> m_texture_size_
				 >> m_particles_origin_ >> m_has_gravity_ >> m_spawn_angle_
				 >> m_spawn_timer_ >> m_spawn_speed_ >> m_spawn_spread_
				 >> m_gravity_force_ >> m_spawn_cooldown_ >> m_rotation_speed_
				 >> m_life_time_time_ >> m_spawn_rotation_ >> m_despawn_cooldown_
				 >> m_base_shape_radius_ >> m_spawn_point_extend_size_
				 >> m_spawn_count_ >> m_base_shape_point_count_;
			
			file >> tmp_resources_type >> tmp_resources_name;

			if (tmp_resources_type == "Animation")
			{
				std::string tmp_actual_key_name, tmp_animation_texture_name;
				
				file >> tmp_actual_key_name >> tmp_animation_texture_name;
				
				path = fs::path(path).parent_path().string();
				path += "/" + tmp_resources_name + ".anim";
				
				auto tmp_animation = getParent()->addComponent<lc::Animation>();
				m_particles_ressource_ = tmp_animation;
				tmp_animation->load_animation_file(path);
				tmp_animation->isVisible(false);
				if (getParent()->hasComponent(tmp_animation_texture_name))
				{
					tmp_animation->get_texture() = getParent()->getComponent<lc::Texture>(tmp_animation_texture_name);
					if (tmp_actual_key_name != "No_Actual_Key")
						tmp_animation->select_animation_key(tmp_actual_key_name, false);
				}
			}
			else if (tmp_resources_type == "Texture")
			{
				m_ressource_to_search_.first = true;
				m_ressource_to_search_.second = tmp_resources_name;
			}
		}
	}

	void Particles::texture_to_search()
	{
		if (m_ressource_to_search_.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				std::shared_ptr<lc::Ressource> tmp_ressource(std::dynamic_pointer_cast<lc::Texture>(component));
				if (!tmp_ressource)
					tmp_ressource = std::dynamic_pointer_cast<lc::Animation>(component);

				if (tmp_ressource)
				{
					if (m_ressource_to_search_.second == fs::path(tmp_ressource->getName()).filename().stem().string())
					{
						m_particles_ressource_ = tmp_ressource;
						tmp_ressource->isVisible(false);
						break;
					}
				}
			}

			m_ressource_to_search_.first = false;
		}
	}

	void Particles::update_value()
	{
		if (m_particles_type_ == ParticlesSystemType::LIFE_TIME)
			m_life_time_timer_ += Tools::getDeltaTime();

		m_particles_spawn_center_ = (getParent()->getTransform().getPosition() + m_relativePosition);

		m_spawn_timer_ += Tools::getDeltaTime();
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


				const auto tmp_particle = std::make_shared<Particle>(
					m_spawn_speed_, m_despawn_cooldown_,
					m_spawn_rotation_ + Tools::Rand(-m_spawn_spread_ / 2.f, m_spawn_spread_ / 2.f),
					m_spawn_angle_, m_rotation_speed_, m_gravity_force_,
					m_has_gravity_,
					tmp_spawn_position, m_particles_origin_);

				m_particles_.push_back(tmp_particle);
			}

			m_spawn_timer_ = 0.f;
		}
	}

	void Particles::spawn_point_draw(WindowManager& window)
	{
		m_spawn_point_particles_extend_.setRotation(m_spawn_rotation_ - 90.f);
		m_spawn_point_particles_extend_.setSize(sf::Vector2f(m_spawn_point_extend_size_, m_spawn_point_particles_extend_.getSize().y));
		m_spawn_point_particles_extend_.setOrigin(sf::Vector2f(m_spawn_point_extend_size_, m_spawn_point_particles_extend_.getOrigin().y) / 2.f);

		m_spawn_point_particles_.setPosition(m_particles_spawn_center_);
		m_spawn_point_particles_extend_.setPosition(m_particles_spawn_center_);
		window.draw(m_spawn_point_particles_);
		window.draw(m_spawn_point_particles_extend_);
	}

	void Particles::particles_draw(WindowManager& window)
	{
		for (auto particle = m_particles_.begin(); particle != m_particles_.end();)
		{
			this->particle_draw(*particle, window);

			if ((*particle)->get_despawn_time() > (*particle)->get_despawn_cooldown())
				particle = m_particles_.erase(particle);
			else
				++particle;
		}
	}

	void Particles::particles_draw(sf::RenderTexture& window)
	{
		for (auto particle = m_particles_.begin(); particle != m_particles_.end();)
		{
			this->particle_draw(*particle, window);

			if ((*particle)->get_despawn_time() > (*particle)->get_despawn_cooldown())
				particle = m_particles_.erase(particle);
			else
				++particle;
		}
	}

	void Particles::particle_draw(const std::shared_ptr<Particle>& particle, WindowManager& window)
	{
		const auto tmp_ressource = m_particles_ressource_.lock();

		if (particle->has_gravity())
			particle->get_velocity().y += particle->get_gravity_force() * Tools::getDeltaTime();

		particle->get_transform().getPosition() += particle->get_velocity() * Tools::getDeltaTime();
		particle->get_transform().getRotation() += particle->get_rotation_speed() * Tools::getDeltaTime();

		particle->get_despawn_time() += Tools::getDeltaTime();

		tmp_ressource ? tmp_ressource->getShape().setFillColor(m_spawn_color_) : m_base_shape_.setFillColor(m_spawn_color_);
		tmp_ressource ? tmp_ressource->getShape().setOrigin(particle->get_transform().getOrigin()) : m_base_shape_.setOrigin(particle->get_transform().getOrigin());
		tmp_ressource ? tmp_ressource->getShape().setScale(particle->get_transform().getScale()) : m_base_shape_.setScale(particle->get_transform().getScale());
		
		tmp_ressource ? 
			tmp_ressource->getShape().setPosition(particle->get_transform().getPosition()) : 
			m_base_shape_.setPosition(particle->get_transform().getPosition());

		tmp_ressource ? 
			tmp_ressource->getShape().setRotation(particle->get_transform().getRotation()) : 
			m_base_shape_.setRotation(particle->get_transform().getRotation());

		if (Tools::Collisions::rect_rect(
			{
				window.getWindow().getView().getCenter() - window.getWindow().getView().getSize() / 2.f,
				window.getWindow().getView().getSize()
			},
			{
				particle->get_transform().getPosition() - particle->get_transform().getOrigin(),
				tmp_ressource ? m_texture_size_ : sf::Vector2f(m_base_shape_radius_, m_base_shape_radius_)
			}))
		{
			tmp_ressource ? window.draw(tmp_ressource->getShape()) : window.draw(m_base_shape_);
		}
	}

	void Particles::particle_draw(const std::shared_ptr<Particle>& particle, sf::RenderTexture& window)
	{
		const auto tmp_ressource = m_particles_ressource_.lock();

		if (particle->has_gravity())
			particle->get_velocity().y += particle->get_gravity_force() * Tools::getDeltaTime();

		particle->get_transform().getPosition() += particle->get_velocity() * Tools::getDeltaTime();
		particle->get_transform().getRotation() += particle->get_rotation_speed() * Tools::getDeltaTime();

		particle->get_despawn_time() += Tools::getDeltaTime();

		tmp_ressource ? tmp_ressource->getShape().setFillColor(m_spawn_color_) : m_base_shape_.setFillColor(m_spawn_color_);
		tmp_ressource ? tmp_ressource->getShape().setOrigin(particle->get_transform().getOrigin()) : m_base_shape_.setOrigin(particle->get_transform().getOrigin());
		tmp_ressource ? tmp_ressource->getShape().setScale(particle->get_transform().getScale()) : m_base_shape_.setScale(particle->get_transform().getScale());
		
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

	sf::Vector2f Particles::get_extend_spawn_point(sf::Vector2f center_position) const
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
}
