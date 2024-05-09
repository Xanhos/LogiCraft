#include "HeatShader.h"

namespace lc
{
    namespace shader
    {
        heat_shader::heat_shader()
            : m_distortion_factor_(0.01f), m_rise_factor_(0.5f)
        {
			m_name = "Heat Shader";
        	m_typeName = "Heat Shader";
        	m_type = TYPE::SHADER;

        	m_ressource_to_search_ = std::make_pair(true, "");
        	
            heat_shader::setup_shader_script_string();
            
            m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	
        	m_texture_map_.loadFromFile("../Ressources/ALL/SHADERS/Heat_map.png");
            m_texture_map_.setRepeated(true);
            m_texture_map_.setSmooth(true);

            m_shader_->setUniform("u_distortion_map_texture", m_texture_map_);
            m_shader_->setUniform("u_current_texture", sf::Shader::CurrentTexture);
        }

        heat_shader::~heat_shader()
        {
            m_shader_.reset();

        	if (!m_heat_ressource_.expired())
        		m_heat_ressource_.lock()->isUsedByAComponent() = false;
        }

        void heat_shader::UpdateEvent(sf::Event& event)
        {
        }

        void heat_shader::Update(WindowManager& window)
        {
        	this->texture_to_search();
        }

        void heat_shader::Draw(WindowManager& window)
        {
            m_time_ += Tools::getDeltaTime();

            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	if (!m_heat_ressource_.expired())
        		window.draw(m_heat_ressource_.lock()->getShape(), m_shader_states_);
        }

        void heat_shader::Draw(sf::RenderTexture& window)
        {
            m_time_ += Tools::getDeltaTime();

            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	if (!m_heat_ressource_.expired())
				window.draw(m_heat_ressource_.lock()->getShape(), m_shader_states_);
        }

        void heat_shader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
        {
        	save << static_cast<int>(m_type)
			 << " " << m_typeName
        	 << " " << m_rise_factor_
        	 << " " << m_distortion_factor_
			 << " " << (m_heat_ressource_.expired() ? static_cast<std::string>("No_Texture") : m_heat_ressource_.lock()->getName()) << '\n';
        }

        void heat_shader::Load(std::ifstream& load)
        {
        	std::string tmp_texture_name;
        	load >> m_rise_factor_ >> m_distortion_factor_ >> tmp_texture_name;

        	m_ressource_to_search_ = std::make_pair(true, tmp_texture_name);
        }

        std::shared_ptr<lc::GameComponent> heat_shader::Clone()
        {
        	auto tmp_component = std::make_shared<heat_shader>(*this);
        	tmp_component->m_shader_ = std::make_shared<sf::Shader>();
        	tmp_component->m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	tmp_component->m_shader_states_.shader = tmp_component->m_shader_.get();
            return tmp_component;
        }

        void heat_shader::setHierarchieFunc()
        {
            m_hierarchieInformation = [this]()
            {
				const auto tmp_ressource = m_heat_ressource_.lock();

				if (ImGui::BeginCombo("Selected Ressource", tmp_ressource ? tmp_ressource->getName().c_str() : "No Ressource Selected"))
				{
					const bool tmp_is_selected(false);

					if (tmp_ressource)
						if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_is_selected))
						{
							if (tmp_ressource)
								tmp_ressource->isUsedByAComponent() = false;
							
							m_heat_ressource_.reset();
						}

					for (const auto& component : getParent()->getComponents())
					{
						if (auto tmp_ressource_component = std::dynamic_pointer_cast<lc::Texture>(component))
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
									m_heat_ressource_ = tmp_ressource_component;
								}
							}
						}
					}

					ImGui::EndCombo();
				}

            	ImGui::DragFloat("Distortion Factor", &m_distortion_factor_, 0.001f);
            	ImGui::DragFloat("Rise Factor", &m_rise_factor_, 0.01f);
            };
        }

        void heat_shader::setup_shader_script_string()
        {
            m_shader_script_ = R"(#version 130

uniform sampler2D u_current_texture; // Our render texture 
uniform sampler2D u_distortion_map_texture; // Our heat distortion map texture

uniform float u_distortion_factor; // Factor used to control severity of the effect
uniform float u_rise_factor; // Factor used to control how fast air rises
uniform float u_time; // Time used to scroll the distortion map

void main()
{
    vec2 distortionMapCoordinate = gl_TexCoord[0].st;

    // We use the time value to scroll our distortion texture upwards
    // Since we enabled texture repeating, OpenGL takes care of
    // coordinates that lie outside of [0, 1] by discarding
    // the integer part and keeping the fractional part
    // Basically performing a floating point modulo 1
    // 1.1 = 0.1, 2.4 = 0.4, 10.3 = 0.3 etc.

    distortionMapCoordinate.t += u_time * u_rise_factor;

    vec4 distortionMapValue = texture2D(u_distortion_map_texture, distortionMapCoordinate);

    // The values are normalized by OpenGL to lie in the range [0, 1]
    // We want negative offsets too, so we subtract 0.5 and multiply by 2
    // We end up with values in the range [-1, 1]

    vec2 distortionPositionOffset = distortionMapValue.xy;
    distortionPositionOffset -= vec2(0.5f, 0.5f);
    distortionPositionOffset *= 2.f;

    // The factor scales the offset and thus controls the severity
    distortionPositionOffset *= u_distortion_factor;

    // The latter 2 channels of the texture are unused... be creative
    vec2 distortionUnused = distortionMapValue.zw;

    // Since we all know that hot air rises and cools,
    // the effect loses its severity the higher up we get
    // We use the t (a.k.a. y) texture coordinate of the original texture
    // to tell us how high up we are and damp accordingly
    // Remember, OpenGL 0 is at the bottom

    distortionPositionOffset *= gl_TexCoord[0].t; //Do 1.f - gl_TexCoord[0].t to reverse it and made the heat start upward.

    vec2 distortedTextureCoordinate = gl_TexCoord[0].st + distortionPositionOffset;

    gl_FragColor = gl_Color * texture2D(u_current_texture, distortedTextureCoordinate);
}
)";
        }

        void heat_shader::texture_to_search()
        {
        	if (m_ressource_to_search_.first)
        	{
        		for (auto& component : getParent()->getComponents())
        		{
        			if (const auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
        			{
        				if (m_ressource_to_search_.second == tmp_texture->getName())
        				{
        					m_heat_ressource_ = tmp_texture;
        					tmp_texture->isUsedByAComponent() = true;
        					break;
        				}
        			}
        		}

        		m_ressource_to_search_.first = false;
        	}
        }
    }
}