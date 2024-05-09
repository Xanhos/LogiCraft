#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

namespace lc
{
    namespace shader
    {
        class heat_shader : public shader
        {
        public:
            heat_shader();
            virtual ~heat_shader() override;

            virtual void UpdateEvent(sf::Event& event) override;
            virtual void Update(WindowManager& window) override;
            virtual void Draw(WindowManager& window) override;
            virtual void Draw(sf::RenderTexture& window) override;

            virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int depth) override;
            virtual void SaveRenderer(sf::RenderTexture& texture, int depth) override {}
            virtual void Export(std::ofstream& exportation) override {}
            virtual void Load(std::ifstream& load) override;

            virtual std::shared_ptr<lc::GameComponent> Clone() override;
            virtual void setHierarchieFunc() override;
            
        private:
            virtual void setup_shader_script_string() override;

            void texture_to_search();
        private:
            float m_distortion_factor_;
            float m_rise_factor_;

            std::weak_ptr<lc::Ressource> m_heat_ressource_;
            
            RessourceToSearch m_ressource_to_search_;
        };
    }
}

