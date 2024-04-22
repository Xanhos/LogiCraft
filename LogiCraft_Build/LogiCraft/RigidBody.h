#pragma once
#include "Ressource.h"

namespace lc
{
	class RigidBody : public lc::Ressource
	{
	public:
		RigidBody();
		RigidBody(sf::FloatRect _collider, sf::Vector2f _velocity, sf::Vector2f _relative, sf::Vector2f posBeforeSimulate, bool showCollide, bool kinetic);
		~RigidBody();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;
		
		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;
		virtual void AddInputFunction(std::function<void(lc::RigidBody*)> func);

		sf::Vector2f& getVelocity() { return m_velocity; }
	private:
		bool CheckAllObject(std::shared_ptr<lc::GameObject> _object);

		sf::RectangleShape m_shape;
		sf::FloatRect m_collider;
		sf::Vector2f m_velocity;
		sf::Vector2f m_relativePosition;
		sf::Vector2f m_posBeforeSimulate;
		bool m_showCollider = false;
		bool m_simulate = false;
		bool m_isKinetic = false;
		std::list<std::function<void(lc::RigidBody*)>> m_inputFunc;
	};

}

