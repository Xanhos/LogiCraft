#include "Transform.h"

lc::Transform::Transform() :
	m_rotation(0.f), m_scale(sf::Vector2f(1.f, 1.f))
{
}

lc::Transform::Transform(sf::Vector2f _position, sf::Vector2f _size, sf::Vector2f _origin, sf::Vector2f _scale, float _rotation)
	: m_position(_position), m_size(_size), m_origin(_origin), m_scale(_scale), m_rotation(_rotation)
{}

lc::Transform::~Transform()
{}