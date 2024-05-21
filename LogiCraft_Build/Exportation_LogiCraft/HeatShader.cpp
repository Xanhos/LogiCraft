#include "HeatShader.h"

namespace lc
{
    namespace Shader
    {
        HeatShader::HeatShader()
            : m_distortion_factor_(0.01f), m_rise_factor_(0.5f)
        {
			m_name = "Heat_Shader";
        	m_typeName = "Heat_Shader";
        	m_type = TYPE::SHADER;
        	
            HeatShader::setup_shader_script_string();
            
            m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	
        	m_texture_map_.loadFromFile("../Ressources/ALL/SHADERS/Heat_map.png");
            m_texture_map_.setRepeated(true);
            m_texture_map_.setSmooth(true);

            m_shader_->setUniform("u_distortion_map_texture", m_texture_map_);
            m_shader_->setUniform("u_current_texture", sf::Shader::CurrentTexture);

        	m_render_size_ = { 500u, 500u };
    
        	m_render_texture_ = std::make_shared<sf::RenderTexture>();
        	m_render_texture_->create(m_render_size_.x, m_render_size_.y);
    
        	m_render_view_ = { sf::Vector2f(m_render_texture_->getSize() / 2u), sf::Vector2f(m_render_texture_->getSize()) };
        }

        HeatShader::~HeatShader()
        {
        	m_shader_.reset();
        	m_render_texture_.reset();
        }

        void HeatShader::UpdateEvent(sf::Event& event)
        {
        }

        void HeatShader::Update(WindowManager& window)
        {
        }

        void HeatShader::Draw(WindowManager& window)
        {
        	m_is_in_view_ = getParent()->is_in_window_view(window);
        	
            m_time_ += Tools::getDeltaTime();

            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	if (m_render_size_ != m_render_texture_->getSize())
        		m_render_texture_->create(m_render_size_.x, m_render_size_.y);

        	m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
        	m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

        	if (m_is_in_view_)
        	{
        		m_render_texture_->setView(m_render_view_);
        		m_render_texture_->clear(sf::Color::Black);   
        	}

	        for (int i = 12 - 1; i >= 0; --i)
	        {
	        	for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
	        		this->draw_in_shader(obj_element, window, i);   
	        }
        	
        	if (m_is_in_view_)
        	{
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
        }

        void HeatShader::Draw(sf::RenderTexture& window)
        {
        	m_is_in_view_ = getParent()->is_in_window_view(window);
        	
            m_time_ += Tools::getDeltaTime();

        	//Set the shader uniform values.
            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	//If the zone where the shader is applied changed, the render texture is recreated with the good size.
        	if (m_render_size_ != m_render_texture_->getSize())
        		m_render_texture_->create(m_render_size_.x, m_render_size_.y);

        	//Set the view center and size.
        	m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
        	m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

        	//Set the view a clear the render texture.
        	if (m_is_in_view_)
        	{
        		m_render_texture_->setView(m_render_view_);
        		m_render_texture_->clear(sf::Color::Black);   
        	}

        	//Display of the object in the shader.
	        for (int i = 12 - 1; i >= 0; --i)
	        {
	        	for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
	        		this->draw_in_shader(obj_element, window, i); 
	        }

        	//Then set the texture of the render texture on the renderer and draw it with the shader.
        	if (m_is_in_view_)
        	{
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
        }

        void HeatShader::Load(std::ifstream& load)
        {
        	std::string tmp_texture_name;
        	load >> m_render_size_ >> m_rise_factor_ >> m_distortion_factor_;
        }

        std::shared_ptr<lc::GameComponent> HeatShader::Clone()
        {
        	auto tmp_component = std::make_shared<HeatShader>(*this);
        	tmp_component->m_shader_ = std::make_shared<sf::Shader>();
        	tmp_component->m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	tmp_component->m_shader_states_.shader = tmp_component->m_shader_.get();
        	tmp_component->m_render_texture_ = std::make_shared<sf::RenderTexture>();
        	tmp_component->m_render_texture_->create(m_render_size_.x, m_render_size_.y);
            return tmp_component;
        }

        void HeatShader::setup_shader_script_string()
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

    distortionMapCoordinate.t -= u_time * u_rise_factor;

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

    distortionPositionOffset *= 1.f - gl_TexCoord[0].t; //Do 1.f - gl_TexCoord[0].t to reverse it and made the heat start upward.

    vec2 distortedTextureCoordinate = gl_TexCoord[0].st + distortionPositionOffset;

    gl_FragColor = gl_Color * texture2D(u_current_texture, distortedTextureCoordinate);
}
)";
        }

        void HeatShader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, WindowManager& window, const unsigned char& depth)
        {
        	//The shader will affect only the objects who are under or on the same depth.
        	if (game_object->getDepth() >= getParent()->getDepth() && game_object->getDepth() == depth && !game_object->hasComponent("Heat_Shader"))
        	{
        		//If the object is totally in the zone of the shader, then one draw is done.
        		if (this->is_totally_in(game_object) && m_is_in_view_)
        		{
        			game_object->Draw(*m_render_texture_);
        			game_object->isVisible(false); //The object is made invisible so is not drawn two times.
        		}
        		else
        		{
        			//If the object is not totally in the zone of the shader,
        			//then he needs to be drawn one time in the window and a second time in the render texture.
        			if (Tools::Collisions::rect_rect({getParent()->getTransform().getPosition(), getParent()->getTransform().getSize()},
					{game_object->getTransform().getPosition(), game_object->getTransform().getSize()}))
        			{
        				game_object->Draw(window);
        				if (m_is_in_view_)
        					game_object->Draw(*m_render_texture_);
        				game_object->isVisible(false);
        			}
        			else
        			{
        				game_object->isVisible(true); //And if the object is totally out of the bound of the zone,
        											  //he just will be drawn as normal.
        			}
        		}
        	}

        	//We do the same if the object has children.
        	for (const auto& obj_element : game_object->getObjects())
        		this->draw_in_shader(obj_element, window, depth);
        }

        void HeatShader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, sf::RenderTexture& window, const unsigned char& depth)
        {
        	//The shader will affect only the objects who are under or on the same depth.
        	if (game_object->getDepth() >= getParent()->getDepth() && game_object->getDepth() == depth && !game_object->hasComponent("Heat_Shader"))
        	{
        		//If the object is totally in the zone of the shader, then one draw is done.
        		if (this->is_totally_in(game_object) && m_is_in_view_)
        		{
        			game_object->Draw(*m_render_texture_);
        			game_object->isVisible(false); //The object is made invisible so is not drawn two times.
        		}
        		else
        		{
        			//If the object is not totally in the zone of the shader,
        			//then he needs to be drawn one time in the window and a second time in the render texture.
        			if (Tools::Collisions::rect_rect({getParent()->getTransform().getPosition(), getParent()->getTransform().getSize()},
					{game_object->getTransform().getPosition(), game_object->getTransform().getSize()}))
        			{
        				game_object->Draw(window);
        				if (m_is_in_view_)
        					game_object->Draw(*m_render_texture_);
        				game_object->isVisible(false);
        			}
        			else
        			{
        				game_object->isVisible(true); //And if the object is totally out of the bound of the zone,
        												 //he just will be drawn as normal.
        			}
        		}
        	}

        	//We do the same if the object has children.
        	for (const auto& obj_element : game_object->getObjects())
        		this->draw_in_shader(obj_element, window, depth);
        }
    }
}