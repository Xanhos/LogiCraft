
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
#include "RigidBody.h"
#include "Viewport.h"

namespace lc
{
	class Particle
	{
	public:
		Particle();
		Particle(
			float _speed,
			float _despawnCooldown,
			float _spawnAngle,
			float _spawnRotation,
			float _rotationSpeed,
			float _gravityForce,
			bool _hasGravity,
			sf::Vector2f _spawnPositionInViewport,
			sf::Vector2f _spawnPositionInRender,
			sf::Vector2f _spawnOrigin);
		~Particle();

		lc::Transform& getTransform();
		lc::Transform& getRendererTransform();

		sf::Vector2f& getVelocity();

		float& getDespawnCooldown();
		float& getDespawnTime();
		float& getGravityForce();
		float& getRotationSpeed();

		bool& hasGravity();
		bool& needToBeDeleted();
	private:
		lc::Transform m_transform;
		lc::Transform m_rendererTransform;
		sf::Vector2f m_velocity;

		float m_despawnCooldown;
		float m_despawnTime;
		float m_gravityForce;
		float m_rotationSpeed;

		bool m_hasGravity;
		bool m_needToBeDeleted;
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
		Particles(ParticlesSystemType _type);
		virtual ~Particles() override;

		virtual void UpdateEvent(sf::Event& _event) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;

		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;
	private:
		/*
		* @brief When the particles system is load by a save it can have a texture,
		* @brief so this function found the texture component that he needs.
		*/
		void HasToFoundHisTexture();

		/*
		* @brief Function to update all the useful values.
		*/
		void UpdateValue();

		/*
		* @brief Function to update the particles tester window that use ImGui.
		*/
		void UpdateRendererWindow();

		/*
		* @brief Function and update and spawn the particle when the timer is good.
		*/
		void SpawnParticles();

		/*
		* @brief Function to draw the spawn point of the particles
		*/
		void SpawnPointDraw(sf::RenderTexture& _window);

		/*
		* @brief Function to draw particles.
		*/
		void particles_draw(sf::RenderTexture& _window);

		/*
		* @brief Function to draw Particle.
		*/
		void ParticleDraw(const std::shared_ptr<Particle>& _particle, sf::RenderTexture& _window);

		/*
		* @brief Function to know if how of the render option is on.
		* 
		* @return True if one of them is on then false.
		*/
		bool ParticlesHisRendered() const;

		/*
		* @brief Function to update all the particles, so it took less performance.
		*/
		void ThreadUpdate();

		/*
		* @brief Function to restart the clock of the thread.
		*/
		static void RestartThreadClock();

		/*
		* @brief Function to return delta time of the thread.
		* 
		* @return Return the delta time.
		*/
		static float GetThreadDeltaTime();

		/*
		* @brief Take the _centerPosition and Rotate it from the angle.
		* 
		* @return Return the Vector2f that as been modified. 
		*/
		sf::Vector2f GetExtendSpawnPoint(sf::Vector2f _centerPosition) const;
	private:
		static std::list<std::shared_ptr<Particle>> s_threadParticles;
		static bool s_threadUpdateIsOn;
		static int s_numberOfParticleSystem;
		static std::thread s_updateThread;
		static sf::Clock s_updateClock;
		static sf::Time s_updateTime;

		Tools::Renderer m_renderer_;

		ParticlesSystemType m_particlesType;

		std::list<std::shared_ptr<Particle>> m_particles;

		sf::CircleShape m_baseShape;
		sf::CircleShape m_spawnPointParticles;
		sf::RectangleShape m_spawnPointParticlesExtend;

		std::weak_ptr<lc::Texture> m_particlesTexture;

		sf::Color m_spawnColor;
		sf::Vector2f m_textureSize;
		sf::Vector2f m_particlesOrigin;
		sf::Vector2f m_particlesSpawnCenter;

		float m_spawnPointExtendSize;
		
		float m_spawnCooldown;
		float m_spawnTime;
		float m_despawnCooldown;
		float m_rotationSpeed;
		float m_spawnRotation;
		float m_spawnSpread;
		float m_spawnAngle;
		float m_spawnSpeed;
		float m_baseShapeRadius;

		float m_lifeTimeTimer;
		float m_lifeTimeTime;

		float m_gravityForce;

		int m_baseShapePointCount;
		int m_spawnCount;

		bool m_hasProductHisParticles;
		bool m_hasGravity;
		bool m_isParticlesRenderedOnTheViewport;
		bool m_isWindowTestIsOpen;
		std::pair<bool, std::string> m_hasToFoundHisTexture;
	};
}