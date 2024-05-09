#include "WaterShader.h"

lc::shader::water_shader::water_shader()
    : m_level_(0.5f)
{
    m_name = "Water Shader";
    m_typeName = "Water Shader";
    m_type = TYPE::SHADER;

    m_ressource_to_search_ = std::make_pair(true, "");
    
    water_shader::setup_shader_script_string();

    m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);

    m_texture_map_.loadFromFile("../Ressources/ALL/SHADERS/Water_map.png");
    m_texture_map_.setRepeated(true);
    m_texture_map_.setSmooth(true);

    m_shader_->setUniform("u_distortion_map_texture", m_texture_map_);
    m_shader_->setUniform("u_current_texture", sf::Shader::CurrentTexture);
}

lc::shader::water_shader::~water_shader()
{
    m_shader_.reset();
    if (!m_water_ressource_.expired())
        m_water_ressource_.lock()->isUsedByAComponent() = false;
}

void lc::shader::water_shader::UpdateEvent(sf::Event& event)
{
}

void lc::shader::water_shader::Update(WindowManager& window)
{
    this->texture_to_search();
}

void lc::shader::water_shader::Draw(WindowManager& window)
{
    m_time_ += Tools::getDeltaTime();

    m_shader_->setUniform("u_time", m_time_);
    m_shader_->setUniform("u_level", m_level_);
    m_shader_->setUniform("u_texture_rect", m_water_ressource_.expired() ? sf::Vector2f(0.f, 0.f) : sf::Vector2f(m_water_ressource_.lock()->getShape().getTextureRect().getSize()));
    
    if (!m_water_ressource_.expired())
        window.draw(m_water_ressource_.lock()->getShape(), m_shader_states_);
}
    
void lc::shader::water_shader::Draw(sf::RenderTexture& window)
{
    m_time_ += Tools::getDeltaTime();

    m_shader_->setUniform("u_time", m_time_);
    m_shader_->setUniform("u_level", m_level_);
    m_shader_->setUniform("u_texture_rect_size", (m_water_ressource_.expired() ? sf::Vector2f(0.f, 0.f) :
        sf::Vector2f(m_water_ressource_.lock()->getShape().getTextureRect().getSize())));
    m_shader_->setUniform("u_texture_rect_position", (m_water_ressource_.expired() ? sf::Vector2f(0.f, 0.f) :
        sf::Vector2f(m_water_ressource_.lock()->getShape().getTextureRect().getPosition())));
    m_shader_->setUniform("u_texture_size", (m_water_ressource_.expired() ? sf::Vector2f(0.f, 0.f) :
        sf::Vector2f(m_water_ressource_.lock()->getShape().getTexture()->getSize())));

    if (!m_water_ressource_.expired())
        window.draw(m_water_ressource_.lock()->getShape(), m_shader_states_);
}

void lc::shader::water_shader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
{
    save << static_cast<int>(m_type)
        << " " << m_level_
        << " " << (m_water_ressource_.expired() ? static_cast<std::string>("No_Texture") : m_water_ressource_.lock()->getName()) << '\n';
}

void lc::shader::water_shader::Load(std::ifstream& load)
{
    std::string tmp_texture_name;
    load >> m_level_ >> tmp_texture_name;

    m_ressource_to_search_ = std::make_pair(true, tmp_texture_name);
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
        const auto tmp_ressource = m_water_ressource_.lock();

        if (ImGui::BeginCombo("Selected Ressource", tmp_ressource ? tmp_ressource->getName().c_str() : "No Ressource Selected"))
        {
            const bool tmp_is_selected(false);

            if (tmp_ressource)
                if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_is_selected))
                {
                    if (tmp_ressource)
                        tmp_ressource->isUsedByAComponent() = false;
							
                    m_water_ressource_.reset();
                }

            for (const auto& component : getParent()->getComponents())
            {
                std::shared_ptr<lc::Ressource> tmp_ressource_component(std::dynamic_pointer_cast<lc::Texture>(component));
                if (!tmp_ressource_component)
                    tmp_ressource_component = std::dynamic_pointer_cast<lc::Animation>(component);

                if (tmp_ressource_component)
                {
                    //If the ressource is not already use by another component we use it.
                    if (!tmp_ressource_component->isUsedByAComponent())
                    {
                        if (ImGui::Selectable(std::string(tmp_ressource_component->getName() + "##" + std::to_string(tmp_ressource_component->getID())).c_str(), tmp_is_selected))
                        {
                            //If there were already a component used on the particles, we set it to un use,
                            //to replace by the new one.
                            if (tmp_ressource)
                                tmp_ressource->isUsedByAComponent() = false;

                            //The new ressource is set to use.
                            tmp_ressource_component->isUsedByAComponent() = true;
                            //The weak_ptr of the ressource is set to the new one
                            m_water_ressource_ = tmp_ressource_component;
                        }
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::SliderFloat("Water Level", &m_level_, 0.f, 1.f);
    };
}

void lc::shader::water_shader::setup_shader_script_string()
{
    m_shader_script_ = R"(#version 130

uniform sampler2D u_current_texture;
uniform sampler2D u_distortion_map_texture;
uniform vec2 u_texture_rect_size;
uniform vec2 u_texture_size;
uniform vec2 u_texture_rect_position;
uniform float u_time;
uniform float u_level;

void main()
{
    //PENSER QUE LE C'EST LE RECT QUI MANQUE ET NOM LA TAILLE DE LA TEXTURE TOTAL.
    vec2 tmp = vec2((gl_TexCoord[0].x / u_texture_rect_size.x) * u_texture_size.x, (gl_TexCoord[0].y / u_texture_rect_size.y) * u_texture_size.y);

    // Get the color of the noise texture at a position the current fragment position offset by the time
    vec4 noiseTexCol = texture2D(u_distortion_map_texture, vec2(gl_TexCoord[0].x + 0.025 * u_time, gl_TexCoord[0].y + 0.025 * u_time));
    
    // Reduce the offset
    float reducedOffset = noiseTexCol.r / 50;

    // Upper part is normal
    if (tmp.y + reducedOffset < u_level)
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

void lc::shader::water_shader::texture_to_search()
{
    if (m_ressource_to_search_.first)
    {
        for (auto& component : getParent()->getComponents())
        {
            if (const auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
            {
                if (m_ressource_to_search_.second == tmp_texture->getName())
                {
                    m_water_ressource_ = tmp_texture;
                    tmp_texture->isUsedByAComponent() = true;
                    break;
                }
            }
        }

        m_ressource_to_search_.first = false;
    }
}
