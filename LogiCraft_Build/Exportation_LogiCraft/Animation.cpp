#include "Animation.h"


namespace lc
{
	AnimationKey::AnimationKey()
		: m_total_frame_(0), m_actual_frame_(0), m_frame_timer_(0.f), m_frame_time_(0.f)
	{
	}

	AnimationKey::AnimationKey(std::string name, int total_frame, sf::Vector2i max_frame, float frame_time, sf::IntRect frame_rect)
		: m_name_(name), m_base_frame_rect_(frame_rect), m_max_frame_(max_frame), m_total_frame_(total_frame), m_actual_frame_(0), m_frame_timer_(0.f),
		  m_frame_time_(frame_time)
	{
		this->create_frames_rect();
	}

	AnimationKey::~AnimationKey()
	{
	}

	std::string& AnimationKey::get_name()
	{
		return m_name_;
	}

	std::vector<sf::IntRect>& AnimationKey::get_frames_rects()
	{
		return m_frames_rects_;
	}

	sf::IntRect& AnimationKey::get_base_int_rect()
	{
		return m_base_frame_rect_;
	}

	sf::Vector2i& AnimationKey::get_max_frame()
	{
		return m_max_frame_;
	}

	int& AnimationKey::get_total_frame()
	{
		return m_total_frame_;
	}

	int& AnimationKey::get_actual_frame()
	{
		return m_actual_frame_;
	}

	float& AnimationKey::get_frame_timer()
	{
		return m_frame_timer_;
	}

	float& AnimationKey::get_frame_time()
	{
		return m_frame_time_;
	}

	void AnimationKey::create_frames_rect()
	{
		m_frames_rects_.clear();

		int tmp_x(0);
		int tmp_y(0);
		for (int i = 0; i < m_total_frame_; i++)
		{
			if (m_max_frame_.x != 0)
				if (i % m_max_frame_.x == 0 && i != 0)
				{
					tmp_x = 0;
					tmp_y++;
				}

			m_frames_rects_.emplace_back(
				m_base_frame_rect_.left + (m_base_frame_rect_.width * tmp_x),
				m_base_frame_rect_.top + (m_base_frame_rect_.height * tmp_y),
				m_base_frame_rect_.width, m_base_frame_rect_.height);

			tmp_x++;
		}
	}

	Animation::Animation()
		: m_base_total_frame_(0), m_base_frame_time_(0.f),
		 m_animation_is_paused_(false), m_animation_is_reversed_(false), m_stop_at_last_frame_(false)
	{
		m_name = "Animation";
		m_typeName = "Animation";
		m_type = TYPE::ANIMATION;
	}

	Animation::~Animation()
	{
		if (!m_texture_.expired())
			m_texture_.lock()->isUsedByAComponent() = false;
	}

	void Animation::UpdateEvent(sf::Event& event)
	{
	}

	void Animation::Update(WindowManager& window)
	{
		this->texture_to_search();
	}

	void Animation::Draw(WindowManager& window)
	{
		if (const auto tmp_texture = m_texture_.lock())
		{
			if (const auto tmp_animation_key = m_actual_animation_key_.lock())
			{
				if ((!m_animation_is_paused_ and !m_stop_at_last_frame_ and !m_animation_is_reversed_) or (!m_animation_is_paused_ and m_stop_at_last_frame_ and !m_animation_is_reversed_ and tmp_animation_key->get_actual_frame() != tmp_animation_key->get_total_frame() - 1) or (!m_animation_is_paused_ and m_stop_at_last_frame_ and m_animation_is_reversed_ and tmp_animation_key->get_actual_frame() > 0))
				{
					tmp_animation_key->get_frame_timer() += Tools::getDeltaTime();
					if (tmp_animation_key->get_frame_timer() >= tmp_animation_key->get_frame_time())
					{
						m_animation_is_reversed_ ? tmp_animation_key->get_actual_frame()-- : tmp_animation_key->get_actual_frame()++;

						if (tmp_animation_key->get_actual_frame() <= 0 && m_animation_is_reversed_ && !m_stop_at_last_frame_)
							tmp_animation_key->get_actual_frame() = tmp_animation_key->get_total_frame() - 1;
						else if (tmp_animation_key->get_actual_frame() > tmp_animation_key->get_total_frame() - 1 && !m_animation_is_reversed_)
							tmp_animation_key->get_actual_frame() = 0;

						tmp_animation_key->get_frame_timer() = 0.f;
					}

					tmp_texture->getShape().setTextureRect(tmp_animation_key->get_frames_rects()[tmp_animation_key->get_actual_frame()]);
					tmp_texture->getShape().setSize(sf::Vector2f(tmp_animation_key->get_frames_rects()[tmp_animation_key->get_actual_frame()].getSize()));
				}
			}

			if (!m_isUsedByAComponent)
			{
				tmp_texture->getShape().setOrigin(getParent()->getTransform().getOrigin());
				tmp_texture->getShape().setScale(getParent()->getTransform().getScale());
				tmp_texture->getShape().setRotation(getParent()->getTransform().getRotation());

				tmp_texture->getShape().setPosition(getParent()->getTransform().getPosition());
				window.draw(tmp_texture->getShape());
			}
		}
	}

	void Animation::Draw(sf::RenderTexture& window)
	{
	}

	void Animation::Load(std::ifstream& load)
	{
		int tmp_animation_key_cout(0);
		std::string tmp_texture_name;
		std::string tmp_actual_key_name;

		load 
			 >> m_name
			 >> tmp_texture_name
			 >> tmp_actual_key_name
			 >> tmp_animation_key_cout;

		std::shared_ptr<AnimationKey> tmp_animation_key;
		for (int i = 0; i < tmp_animation_key_cout; i++)
		{
			tmp_animation_key = std::make_shared<AnimationKey>();
			
			load >> tmp_animation_key->get_name()
				 >> tmp_animation_key->get_base_int_rect()
				 >> tmp_animation_key->get_max_frame()
				 >> tmp_animation_key->get_frame_time()
				 >> tmp_animation_key->get_total_frame();

			tmp_animation_key->create_frames_rect();

			if (tmp_actual_key_name != "No_Actual_Key")
				if (tmp_animation_key->get_name() == tmp_actual_key_name)
					m_actual_animation_key_ = tmp_animation_key;

			m_animation_keys_.emplace(tmp_animation_key->get_name(), tmp_animation_key);
		}
		
		m_texture_to_search_ = std::make_pair(true, tmp_texture_name);
	}

	std::shared_ptr<lc::GameComponent> Animation::Clone()
	{
		auto tmp_clone = std::make_shared<lc::Animation>(*this);
		if (!tmp_clone->m_texture_.expired())
			tmp_clone->m_texture_to_search_ = std::make_pair(true, tmp_clone->m_texture_.lock()->getName());
		tmp_clone->m_texture_.reset();

		tmp_clone->m_animation_keys_.clear();
		for (const auto& animation_key_pair : m_animation_keys_)
		{
			tmp_clone->m_animation_keys_.emplace(animation_key_pair.first, std::make_shared<AnimationKey>(*animation_key_pair.second));
			tmp_clone->m_actual_animation_key_ = tmp_clone->m_animation_keys_[animation_key_pair.first];
		}

		return tmp_clone;
	}


	sf::RectangleShape& Animation::getShape()
	{
		return (m_texture_.expired() ? m_renderer : m_texture_.lock()->getShape());
	}

	void Animation::select_animation_key(const std::string& name, const bool& reset_last_anim_key)
	{
		const auto tmp_act_anim_key = m_actual_animation_key_.lock();

		if (tmp_act_anim_key && reset_last_anim_key)
		{
			tmp_act_anim_key->get_actual_frame() = 0;
			tmp_act_anim_key->get_frame_timer() = 0.f;
		}
	
		
		m_actual_animation_key_ = m_animation_keys_.at(name);

		const auto tmp_act_anim_key_second = m_actual_animation_key_.lock();
		const auto tmp_texture = m_texture_.lock();
		tmp_texture->getShape().setTextureRect(tmp_act_anim_key_second->get_frames_rects()[tmp_act_anim_key_second->get_actual_frame()]);
		tmp_texture->getShape().setSize(sf::Vector2f(tmp_act_anim_key_second->get_frames_rects()[tmp_act_anim_key_second->get_actual_frame()].getSize()));
		
	}

	void Animation::current_animation_is_paused(const bool& paused)
	{
		m_animation_is_paused_ = paused;
	}

	void Animation::current_animation_is_reversed(const bool& reversed)
	{
		m_animation_is_reversed_ = reversed;
	}

	void Animation::set_stop_at_last_frame(const bool& stop_at_last_frame)
	{
		m_stop_at_last_frame_ = stop_at_last_frame;		
	}

	std::weak_ptr<AnimationKey> Animation::get_current_animation_key()
	{
		return m_actual_animation_key_;
	}


	void Animation::texture_to_search()
	{
		if (m_texture_to_search_.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				if (auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
				{
					if (m_texture_to_search_.second + ".png" == tmp_texture->getName())
					{
						m_texture_ = tmp_texture;
						tmp_texture->isUsedByAComponent() = true;
						break;
					}
				}
			}

			m_texture_to_search_.first = false;
		}	
	}
}
