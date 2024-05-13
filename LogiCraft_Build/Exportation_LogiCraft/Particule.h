
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
#include "Ressource.h"
#include "Texture.h"
//#include "Animation.h"

namespace lc
{
	class Particle
	{
	public:
		Particle();
		Particle(
			float speed,
			float despawn_cooldown,
			float spawn_angle,
			float spawn_rotation,
			float rotation_speed,
			float gravity_force,
			bool has_gravity,
			sf::Vector2f spawn_position_in_viewport,
			sf::Vector2f spawn_origin);
		~Particle();

		lc::Transform& get_transform();

		sf::Vector2f& get_velocity();

		float& get_despawn_cooldown();
		float& get_despawn_time();
		float& get_gravity_force();
		float& get_rotation_speed();

		bool& has_gravity();
		bool& need_to_be_deleted();
	private:
		lc::Transform m_transform_;
		sf::Vector2f m_velocity_;

		float m_despawn_cooldown_;
		float m_despawn_time_;
		float m_gravity_force_;
		float m_rotation_speed_;

		bool m_has_gravity_;
		bool m_need_to_be_deleted_;
	};

	class Particles : public Ressource
	{
	public:
		enum class ParticlesSystemType
		{
			PSTNULL = -1,
			NORMAL, //Just the normal particles' system.
			ONE_TIME, //This type will do the particles and never do it again.
			LIFE_TIME //This type will do the particles for an amount of time.
		};
	public:

		Particles();
		Particles(const ParticlesSystemType type);
		virtual ~Particles() override;

		virtual void UpdateEvent(sf::Event& event) override;
		virtual void Update(WindowManager& window) override;
		virtual void Draw(WindowManager& window) override;
		virtual void Draw(sf::RenderTexture& window) override;

		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void Load(std::ifstream& load) override;
	private:
		/*
		* @brief When the particles system is load by a save it can have a texture,
		* @brief so this function found the texture component that he needs.
		*/
		void texture_to_search();

		/*
		* @brief Function to update all the useful values.
		*/
		void update_value();

		/*
		* @brief Function and update and spawn the particle when the timer is good.
		*/
		void spawn_particles();

		/*
		* @brief Function to draw the spawn point of the particles
		*/
		void spawn_point_draw(WindowManager& window);

		/*
		* @brief Function to draw particles.
		*/
		void particles_draw(WindowManager& window);

		/*
		* @brief Function to draw Particle.
		*/
		void particle_draw(const std::shared_ptr<Particle>& particle, WindowManager& window);
		
		/*
		* @brief Function to update all the particles, so it took less performance.
		*/
		void thread_update();

		/*
		* @brief Function to restart the clock of the thread.
		*/
		static void restart_thread_clock();

		/*
		* @brief Function to return delta time of the thread.
		* 
		* @return Return the delta time.
		*/
		static float get_thread_delta_time();

		/*
		* @brief Take the _centerPosition and Rotate it from the angle.
		* 
		* @return Return the Vector2f that as been modified. 
		*/
		sf::Vector2f get_extend_spawn_point(sf::Vector2f center_position) const;
	private:
		static std::list<std::shared_ptr<Particle>> s_thread_particles_;
		static bool s_thread_update_is_on_;
		static int s_number_of_particle_system_;
		static std::thread s_update_thread_;
		static sf::Clock s_update_clock_;
		static sf::Time s_update_time_;


		ParticlesSystemType m_particles_type_;

		std::list<std::shared_ptr<Particle>> m_particles_;

		sf::CircleShape m_base_shape_;
		sf::CircleShape m_spawn_point_particles_;
		sf::RectangleShape m_spawn_point_particles_extend_;

		std::weak_ptr<lc::Ressource> m_particles_ressource_;

		sf::Color m_spawn_color_;
		sf::Vector2f m_texture_size_;
		sf::Vector2f m_particles_origin_;
		sf::Vector2f m_particles_spawn_center_;

		float m_spawn_point_extend_size_;
		
		float m_spawn_cooldown_;
		float m_spawn_time_;
		float m_despawn_cooldown_;
		float m_rotation_speed_;
		float m_spawn_rotation_;
		float m_spawn_spread_;
		float m_spawn_angle_;
		float m_spawn_speed_;
		float m_base_shape_radius_;

		float m_life_time_timer_;
		float m_life_time_time_;

		float m_gravity_force_;

		int m_base_shape_point_count_;
		int m_spawn_count_;

		bool m_has_product_his_particles_;
		bool m_has_gravity_;

		RessourceToSearch m_ressource_to_search_;
	};
}