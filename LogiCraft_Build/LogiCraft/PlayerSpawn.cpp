#include "PlayerSpawn.h"

#include <memory>

lc::PlayerSpawn::PlayerSpawn()
{
    m_type = TYPE::DISPLAY_COLLIDER;
    m_typeName = "Player Spawn";
}

void lc::PlayerSpawn::Export(std::ofstream& exportation)
{
    Ressource::Export(exportation);
}

void lc::PlayerSpawn::setHierarchieFunc()
{
    m_hierarchieInformation = [&]
    {
        ImGui::DragFloat2("Spawn Position",m_relativePosition);
    };
}

void lc::PlayerSpawn::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << static_cast<int>(m_type)
            << " " << Tools::replaceSpace(m_typeName, true)
            << " " << m_relativePosition << std::endl;
}

void lc::PlayerSpawn::Load(std::ifstream& load)
{
    load >> m_typeName
    >> m_relativePosition;
    Tools::replaceSpace(m_typeName,false);
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
    return std::make_shared<PlayerSpawn>(*this);
}
