#pragma once
#include "SFML_ENGINE/Tools.h"

namespace lc
{
	class Transform
	{
	public:
		Transform();
		Transform(sf::Vector2f _position, sf::Vector2f _size = sf::Vector2f(), sf::Vector2f _origin = sf::Vector2f(), sf::Vector2f _scale = sf::Vector2f(1.f, 1.f), float _rotation = 0.f);
		~Transform();

		sf::Vector2f& getPosition() { return m_position; }
		sf::Vector2f& getOrigin() { return m_origin; }
		sf::Vector2f& getScale() { return m_scale; }
		sf::Vector2f& getSize() { return m_size; }

		sf::Vector2f& getDistance() { return m_distance; }

		float& getRotation() { return m_rotation; }
	private:
		sf::Vector2f m_position;
		sf::Vector2f m_origin;
		sf::Vector2f m_scale;
		sf::Vector2f m_size;

		sf::Vector2f m_distance;

		float m_rotation;
	};
}