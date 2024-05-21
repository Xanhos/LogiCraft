#pragma once
#include "Shader.h"
#include "Animation.h"

namespace lc
{
    namespace Shader
    {
        class WaterShader : public Shader
       {
        public:
            WaterShader();
            virtual ~WaterShader() override;

            virtual void UpdateEvent(sf::Event& event) override;
            virtual void Update(WindowManager& window) override;
            virtual void Draw(WindowManager& window) override;
            virtual void Draw(sf::RenderTexture& window) override;

            virtual void Load(std::ifstream& load) override;

            virtual std::shared_ptr<lc::GameComponent> Clone() override;
        private:
            virtual void setup_shader_script_string() override;

            void draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, sf::RenderTexture& window);
            void draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, WindowManager& window);
        private:
            float m_level_;
            int m_distortion_level_;

            std::shared_ptr<sf::RenderTexture> m_render_texture_;
            sf::View m_render_view_;
            sf::Sprite m_render_sprite_;
            sf::Vector2u m_render_size_;

            bool m_is_in_view_;
       };
    }
}