#include "WaterShader.h"

lc::shader::water_shader::water_shader()
    : m_level_(0.5f), m_distortion_level_(50)
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

    m_renderer_size_ = { 500u, 500u };
    
    m_render_texture_ = std::make_shared<sf::RenderTexture>();
    m_render_texture_->create(m_renderer_size_.x, m_renderer_size_.y);
    
    m_render_view_ = { sf::Vector2f(m_render_texture_->getSize() / 2u), sf::Vector2f(m_render_texture_->getSize()) };
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
    m_shader_->setUniform("u_distortion_level", m_distortion_level_);

    if (m_renderer_size_ != m_render_texture_->getSize())
        m_render_texture_->create(m_renderer_size_.x, m_renderer_size_.y);
    
    m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
    m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

    m_render_texture_->setView(m_render_view_);
    m_render_texture_->clear(sf::Color::Black);
    
    for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
        this->draw_in_shader(obj_element, window);
    
    m_render_texture_->display();
    
    m_renderer.setTexture(&m_render_texture_->getTexture());
    m_renderer.setTextureRect({{0, 0}, sf::Vector2i(m_render_texture_->getSize())});
    m_renderer.setSize(sf::Vector2f(m_render_texture_->getSize()));
    m_renderer.setPosition(getParent()->getTransform().getPosition());
    m_renderer.setScale(getParent()->getTransform().getScale());
    m_renderer.setOrigin(getParent()->getTransform().getOrigin());
    m_renderer.setRotation(getParent()->getTransform().getRotation());
    
    window.draw(m_renderer, m_shader_states_);
}
    
void lc::shader::water_shader::Draw(sf::RenderTexture& window)
{
    m_time_ += Tools::getDeltaTime();

    m_shader_->setUniform("u_time", m_time_);
    m_shader_->setUniform("u_level", m_level_);
    m_shader_->setUniform("u_distortion_level", m_distortion_level_);

    if (m_renderer_size_ != m_render_texture_->getSize())
        m_render_texture_->create(m_renderer_size_.x, m_renderer_size_.y);
    
    m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
    m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

    m_render_texture_->setView(m_render_view_);
    m_render_texture_->clear(sf::Color::Black);
    
    for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
        this->draw_in_shader(obj_element, window);
    
    m_render_texture_->display();
    
    m_renderer.setTexture(&m_render_texture_->getTexture());
    m_renderer.setTextureRect({{0, 0}, sf::Vector2i(m_render_texture_->getSize())});
    m_renderer.setSize(sf::Vector2f(m_render_texture_->getSize()));
    m_renderer.setPosition(getParent()->getTransform().getPosition());
    m_renderer.setScale(getParent()->getTransform().getScale());
    m_renderer.setOrigin(getParent()->getTransform().getOrigin());
    m_renderer.setRotation(getParent()->getTransform().getRotation());
    
    window.draw(m_renderer, m_shader_states_);
}

void lc::shader::water_shader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
{
    save << static_cast<int>(m_type)
        << " " << m_level_
        << " " << m_distortion_level_
        << " " << m_renderer_size_.x
        << " " << m_renderer_size_.y;
}

void lc::shader::water_shader::Load(std::ifstream& load)
{
    load >> m_level_ >> m_distortion_level_ >> m_renderer_size_.x >> m_renderer_size_.y;
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
        ImGui::DragInt("Distortion Level", &m_distortion_level_, 1.f);

        unsigned int tmp_tab[2]{ m_renderer_size_.x, m_renderer_size_.y };
        ImGui::DragScalarN("Water Size", ImGuiDataType_U32, &tmp_tab, 2);
        m_renderer_size_ = {tmp_tab[0], tmp_tab[1]};
    };
}

void lc::shader::water_shader::setup_shader_script_string()
{
    m_shader_script_ = R"(#version 130

uniform sampler2D u_current_texture;
uniform sampler2D u_distortion_map_texture;
uniform float u_time;
uniform float u_level;
uniform int u_distortion_level;

void main()
{
    // Get the color of the noise texture at a position the current fragment position offset by the time
    vec4 noiseTexCol = texture2D(u_distortion_map_texture, vec2(gl_TexCoord[0].x + 0.025 * u_time, gl_TexCoord[0].y + 0.025 * u_time));
    
    // Reduce the offset
    float reducedOffset = noiseTexCol.r / u_distortion_level;

    // Upper part is normal
    if ((1 - gl_TexCoord[0].y) + reducedOffset < u_level)
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

void lc::shader::water_shader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, sf::RenderTexture& window)
{
    if (game_object->getDepth() <= getParent()->getDepth() && !game_object->hasComponent("Water Shader"))
    {
        if (this->is_totally_in(game_object))
        {
            game_object->Draw(*m_render_texture_);
            game_object->isVisible() = false;
        }
        else
        {
            if (Tools::Collisions::rect_rect({getParent()->getTransform().getPosition(), getParent()->getTransform().getSize()},
            {game_object->getTransform().getPosition(), game_object->getTransform().getSize()}))
            {
                game_object->Draw(window);
                game_object->Draw(*m_render_texture_);
                game_object->isVisible() = false;
            }
            else
            {
                game_object->isVisible() = true;
            }
        }
    }
    
    for (const auto& obj_element : game_object->getObjects())
        this->draw_in_shader(obj_element, window);
}

void lc::shader::water_shader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, WindowManager& window)
{
    if (game_object->getDepth() <= getParent()->getDepth() && !game_object->hasComponent("Water Shader"))
    {
        if (this->is_totally_in(game_object))
        {
            game_object->Draw(*m_render_texture_);
            game_object->isVisible() = false;
        }
        else
        {
            if (Tools::Collisions::rect_rect({getParent()->getTransform().getPosition(), getParent()->getTransform().getSize()},
            {game_object->getTransform().getPosition(), game_object->getTransform().getSize()}))
            {
                game_object->Draw(window);
                game_object->Draw(*m_render_texture_);
                game_object->isVisible() = false;
            }
            else
            {
                game_object->isVisible() = true;
            }
        }
    }
    
    for (const auto& obj_element : game_object->getObjects())
        this->draw_in_shader(obj_element, window);
}

bool lc::shader::water_shader::is_totally_in(const std::shared_ptr<lc::GameObject>& game_object)
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
