#include "DisplayCollider.h"

#include <memory>

lc::DisplayCollider::~DisplayCollider()
{
}

void lc::DisplayCollider::Load(std::ifstream& load)
{
    sf::Vector2f size;
    load
        >> size
        >> m_relativePosition.x
        >> m_relativePosition.y
        >> m_trigger_name_
        >> m_triggered_name_;
}

void lc::DisplayCollider::UpdateEvent(sf::Event& _window)
{
}

void lc::DisplayCollider::Update(WindowManager& _window)
{
    if(m_trigger_.expired())
        if(lc::GameObject::GetRoot(getParent())->hasObject(m_trigger_name_))
            m_trigger_ = lc::GameObject::GetRoot(getParent())->getObject(m_trigger_name_);
    
    if(m_triggered_.expired())
        if(lc::GameObject::GetRoot(getParent())->hasObject(m_triggered_name_))
            m_triggered_ = lc::GameObject::GetRoot(getParent())->getObject(m_triggered_name_);

    if(!m_trigger_.expired() && !m_triggered_.expired())
    {
        auto trigger = m_trigger_.lock(), triggered = m_triggered_.lock();
        if(Tools::Collisions::rect_rect({trigger->getTransform().getPosition() - trigger->getTransform().getOrigin(),trigger->getTransform().getSize()},
            {triggered->getTransform().getPosition() - triggered->getTransform().getOrigin(),triggered->getTransform().getSize()}))
            triggered->isVisible(true);
        else triggered->isVisible(false);
    }
}

void lc::DisplayCollider::Draw(WindowManager& _window)
{
}

void lc::DisplayCollider::Draw(sf::RenderTexture& _window)
{
}

std::shared_ptr<lc::GameComponent> lc::DisplayCollider::Clone()
{
    return std::make_shared<lc::DisplayCollider>(*this);
}
