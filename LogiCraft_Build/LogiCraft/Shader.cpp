#include "Shader.h"

lc::shader::shader::shader()
    : m_time_(0.f)
{
    m_shader_ = std::make_shared<sf::Shader>();

    m_shader_states_.shader = m_shader_.get();
    m_shader_states_.transform = sf::Transform::Identity;
    m_shader_states_.blendMode = sf::BlendAlpha;
    m_shader_states_.texture = nullptr;
}

lc::shader::shader::~shader()
{
    m_shader_.reset();
}
