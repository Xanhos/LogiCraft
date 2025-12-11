#pragma once
#include "Ressource.h"

namespace lc
{
    class PlayerSpawn : public lc::Ressource
    {
    public:
        PlayerSpawn() = default;
        void Load(std::ifstream& load) override;
        void UpdateEvent(sf::Event& _window) override;
        void Update(WindowManager& _window) override;
        void Draw(WindowManager& _window) override;
        void Draw(sf::RenderTexture& _window) override;
        std::shared_ptr<lc::GameComponent> Clone() override;

        const sf::Vector2f& GetSpawnPosition() const {return m_relativePosition;}
    };
}

