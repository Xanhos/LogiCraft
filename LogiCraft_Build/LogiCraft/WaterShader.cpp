#include "WaterShader.h"

lc::shader::water_shader::water_shader()
    : m_level_(0.5f)
{
    m_name = "Water Shader";
    m_typeName = "Water Shader";
    m_type = TYPE::SHADER;
    
    water_shader::setup_shader_script_string();

    m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);

    m_texture_map_.loadFromFile("../Ressources/ALL/SHADERS/Water_map.png");
    m_texture_map_.setRepeated(true);
    m_texture_map_.setSmooth(true);

    m_shader_->setUniform("u_distortion_map_texture", m_texture_map_);
    m_shader_->setUniform("u_current_texture", sf::Shader::CurrentTexture);

    m_shader_renderer_ = std::make_shared<sf::RenderTexture>();
    
    m_shader_renderer_->create(500u, 250u);
}

lc::shader::water_shader::~water_shader()
{
    m_shader_.reset();
    
}

void lc::shader::water_shader::UpdateEvent(sf::Event& event)
{
}

void lc::shader::water_shader::Update(WindowManager& window)
{
}

void lc::shader::water_shader::Draw(WindowManager& window)
{
    m_time_ += Tools::getDeltaTime();

    m_shader_->setUniform("u_time", m_time_);
    m_shader_->setUniform("u_level", m_level_);
}
    
void lc::shader::water_shader::Draw(sf::RenderTexture& window)
{
    m_time_ += Tools::getDeltaTime();

    m_shader_->setUniform("u_time", m_time_);
    m_shader_->setUniform("u_level", m_level_);

    m_shader_renderer_->clear(sf::Color::Transparent);
    
    for (auto obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
        this->draw_in_shader(obj_element);
    
    m_shader_renderer_->display();

    m_shader_renderer_sprite_.setTexture(m_shader_renderer_->getTexture());

    window.draw(m_shader_renderer_sprite_, m_shader_states_);
}

void lc::shader::water_shader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
{
    save << static_cast<int>(m_type)
        << " " << m_level_;
}

void lc::shader::water_shader::Load(std::ifstream& load)
{
    load >> m_level_;
}

std::shared_ptr<lc::GameComponent> lc::shader::water_shader::Clone()
{
    auto tmp_component = std::make_shared<water_shader>(*this);
    tmp_component->m_shader_ = std::make_shared<sf::Shader>();
    tmp_component->m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
    tmp_component->m_shader_states_.shader = tmp_component->m_shader_.get();
    return tmp_component;
}

void lc::shader::water_shader::setHierarchieFunc()
{
    m_hierarchieInformation = [this]()
    {
        ImGui::SliderFloat("Water Level", &m_level_, 0.f, 1.f);
    };
}

void lc::shader::water_shader::setup_shader_script_string()
{
    m_shader_script_ = R"(#version 130

uniform sampler2D u_current_texture;
uniform sampler2D u_distortion_map_texture;
uniform float u_time;
uniform float u_level;

void main()
{
    // Get the color of the noise texture at a position the current fragment position offset by the time
    vec4 noiseTexCol = texture2D(u_distortion_map_texture, vec2(gl_TexCoord[0].x + 0.025 * u_time, gl_TexCoord[0].y + 0.025 * u_time));
    
    // Reduce the offset
    float reducedOffset = noiseTexCol.r / 10;

    // Upper part is normal
    if (gl_TexCoord[0].y + reducedOffset < u_level)
    {
        // multiply it by the color
        gl_FragColor = texture2D(u_current_texture, gl_TexCoord[0].xy);
    }
    else
    {
        // Get the color of the screen at the offset location
        vec4 col = texture2D(u_current_texture, gl_TexCoord[0].xy + vec2(reducedOffset, reducedOffset));

        // Set the fragment color
        gl_FragColor = vec4(col.r / 3.44594, col.g, col.b / 1.024, col.a);
    }
}
)";
}

void lc::shader::water_shader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object)
{
    if (game_object->getDepth() <= getParent()->getDepth() && std::dynamic_pointer_cast<lc::shader::water_shader>(game_object).get() != this)
    {
        game_object->isVisible() = true;
        
        game_object->Draw(*m_shader_renderer_);

        if (Tools::Collisions::rect_rect({getParent()->getTransform().getPosition(), sf::Vector2f(m_shader_renderer_->getSize())},
            {game_object->getTransform().getPosition(), game_object->getTransform().getSize()}))
            game_object->isVisible() = false;
    }
    
    for (const auto& obj_element : game_object->getObjects())
        this->draw_in_shader(obj_element);
}
