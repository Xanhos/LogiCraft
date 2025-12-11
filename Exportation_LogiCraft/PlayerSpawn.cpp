#include "PlayerSpawn.h"

#include <memory>

void lc::PlayerSpawn::Load(std::ifstream& load)
{
    load >> m_relativePosition;
}

void lc::PlayerSpawn::UpdateEvent(sf::Event& _window)
{
}

void lc::PlayerSpawn::Update(WindowManager& _window)
{
}

void lc::PlayerSpawn::Draw(WindowManager& _window)
{
}

void lc::PlayerSpawn::Draw(sf::RenderTexture& _window)
{
}

std::shared_ptr<lc::GameComponent> lc::PlayerSpawn::Clone()
{
    return std::make_shared<lc::PlayerSpawn>(*this);
}
