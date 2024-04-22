
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
	class Particule
	{
	public:
		Particule();
		Particule(
			float _speed,
			float _despawnCooldown,
			float _spawnAngle,
			float _spawnRotation,
			float _rotationSpeed,
			float _gravityForce,
			bool _hasGravity,
			sf::Vector2f _spawnPositionInViewport,
			sf::Vector2f _spawnPositionInRender);
		~Particule();

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

	class Particules : public Ressource
	{
	public:
		enum class ParticulesSystemType
		{
			PSTNULL = -1,
			NORMAL, //Just the normal particules system.
			ONE_TIME, //This type will do the particules and never do it again.
			LIFE_TIME //This type will do the particules for an amount of time.
		};
	public:
		Particules();
		Particules(ParticulesSystemType _type);
		~Particules();

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
		* @brief When the particules system is load by a save it can have a texture,
		* @brief so this function found the texture component that he need.
		*/
		void HasToFoundHisTexture();

		/*
		* @brief Function to update all the useful values.
		*/
		void UpdateValue();

		/*
		* @brief Function to update the particules tester window that use ImGui.
		*/
		void UpdateRendererWindow();

		/*
		* @brief Function and update and spawn the particule when the timer is good.
		*/
		void SpawnParticules();

		/*
		* @brief Function to update value used by the renderer.
		*/
		void UpdateRenderer();

		/*
		* @brief Function to make the grab with the camera.
		*/
		void RendererMovement();

		/*
		* @brief Function to draw the spawn point of the particules
		*/
		void SpawnPointDraw(sf::RenderTexture& _window);

		/*
		* @brief Function to draw particules.
		*/
		void ParticulesDraw(sf::RenderTexture& _window);

		/*
		* @brief Function to draw particule.
		*/
		void ParticuleDraw(std::shared_ptr<Particule> _particule, sf::RenderTexture& _window, sf::RenderTexture& _renderer);

		/*
		* @brief Function to know if how of the render option is on.
		* 
		* @return True if one of them is on then false.
		*/
		bool ParticulesHisRendered() const;

		/*
		* @brief Function to update all the particules so it took less performance.
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
		sf::Vector2f GetExtendSpawnPoint(sf::Vector2f _centerPosition);
	private:
		static std::list<std::shared_ptr<Particule>> s_threadParticules;
		static bool s_threadUpdateIsOn;
		static int s_numberOfParticuleSystem;
		static std::thread s_updateThread;
		static sf::Clock s_updateClock;
		static sf::Time s_updateTime;

		std::shared_ptr<sf::RenderTexture> m_renderer;
		sf::Vector2f m_redererAddedPosition;
		sf::Vector2f m_redererLastAddedPosition;
		sf::Vector2f m_rendererSize;
		sf::View m_rendererView;
		bool m_renderderIsFocus;
		bool m_rendererIsGrabbed;
		float m_rendererZoom;

		ParticulesSystemType m_particulesType;

		std::list<std::shared_ptr<Particule>> m_particules;

		sf::CircleShape m_baseShape;
		sf::CircleShape m_spawnPointParticules;
		sf::RectangleShape m_spawnPointParticulesExtend;

		std::weak_ptr<lc::Texture> m_particulesTexture;

		sf::Color m_spawnColor;
		sf::Vector2f m_textureSize;
		sf::Vector2f m_particulesOrigin;
		sf::Vector2f m_particulesSpawnCenter;

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

		bool m_hasProductHisParticules;
		bool m_hasGravity;
		bool m_isParticulesRenderedOnTheViewport;
		bool m_isWindowTestIsOpen;
		std::pair<bool, std::string> m_hasToFoundHisTexture;
	};
}