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

bool lc::shader::shader::is_totally_in(const std::shared_ptr<lc::GameObject>& game_object)
{
    if (game_object->getTransform().getPosition().x >= getParent()->getTransform().getPosition().x &&
    game_object->getTransform().getPosition().y >= getParent()->getTransform().getPosition().y &&
    game_object->getTransform().getPosition().x + game_object->getTransform().getSize().x <= getParent()->getTransform().getPosition().x + getParent()->getTransform().getSize().x &&
    game_object->getTransform().getPosition().y + game_object->getTransform().getSize().y <= getParent()->getTransform().getPosition().y + getParent()->getTransform().getSize().y)
    {
        return true;
    }
    
    return false;
}
