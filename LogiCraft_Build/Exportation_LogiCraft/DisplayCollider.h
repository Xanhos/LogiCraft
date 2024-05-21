#pragma once
#include "Ressource.h"

namespace lc
{
    class DisplayCollider : public lc::Ressource
    {
        sf::FloatRect m_collider_;
        std::string m_trigger_name_, m_triggered_name_;
        std::weak_ptr<lc::GameObject> m_trigger_, m_triggered_;
    public:
        ~DisplayCollider() override;
        void Load(std::ifstream& load) override;
        void UpdateEvent(sf::Event& _window) override;
        void Update(WindowManager& _window) override;
        void Draw(WindowManager& _window) override;
        void Draw(sf::RenderTexture& _window) override;
        std::shared_ptr<lc::GameComponent> Clone() override;
    };

}
