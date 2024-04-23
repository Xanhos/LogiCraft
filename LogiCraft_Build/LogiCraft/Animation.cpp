#include "Animation.h"

namespace lc
{
	AnimationKey::AnimationKey()
		: m_frameTimer(0.f), m_maxFrame(0)
	{
	}

	AnimationKey::AnimationKey(std::string _name, int _maxFrame, float _frameTimer, sf::IntRect _frameRect)
		: m_maxFrame(_maxFrame), m_actualFrame(0), m_frameTimer(_frameTimer), m_name(_name), m_baseFrameRect(_frameRect)
	{
		for (int i = 0; i < m_maxFrame; i++)
			m_framesRects.push_back({ _frameRect.left * i, _frameRect.top, _frameRect.width, _frameRect.height });
	}

	AnimationKey::~AnimationKey()
	{
	}

	std::string& AnimationKey::getName()
	{
		return m_name;
	}

	std::vector<sf::IntRect>& AnimationKey::getFramesRects()
	{
		return m_framesRects;
	}

	sf::IntRect& AnimationKey::getBaseIntRect()
	{
		return m_baseFrameRect;
	}

	int& AnimationKey::getMaxFrame()
	{
		return m_maxFrame;
	}

	int& AnimationKey::getActualFrame()
	{
		return m_actualFrame;
	}

	float& AnimationKey::getFrameTimer()
	{
		return m_frameTimer;
	}

	float& AnimationKey::getFrameTime()
	{
		return m_frameTime;
	}

	Animation::Animation()
		: m_baseFrameTimer(0.f), m_baseMaxFrame(0)
	{
		m_name = "Animation";
		m_typeName = "Animation";
		m_type = TYPE::ANIMATION;
	}

	Animation::~Animation()
	{
	}

	void Animation::UpdateEvent(sf::Event& _window)
	{
	}

	void Animation::Update(WindowManager& _window)
	{
	}

	void Animation::Draw(WindowManager& _window)
	{
		if (auto tmp_texture = m_texture.lock())
		{
			if (auto tmp_animationKey = m_actualAnimationKey.lock())
			{
				tmp_animationKey->getFrameTimer() += Tools::getDeltaTime();
				if (tmp_animationKey->getFrameTimer() >= tmp_animationKey->getFrameTime())
				{
					tmp_animationKey->getActualFrame()++;
					if (tmp_animationKey->getActualFrame() > tmp_animationKey->getMaxFrame())
						tmp_animationKey->getActualFrame() = 0;

					tmp_animationKey->getFrameTimer() = 0.f;
				}

				tmp_texture->getShape().setTextureRect(tmp_animationKey->getFramesRects()[tmp_animationKey->getActualFrame()]);
			}
		}
	}

	void Animation::Draw(sf::RenderTexture& _window)
	{
	}

	void Animation::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
	{
	}

	void Animation::Load(std::ifstream& load)
	{
	}

	std::shared_ptr<lc::GameComponent> Animation::Clone()
	{
		return std::make_shared<lc::Animation>(*this);
	}

	void Animation::setHierarchieFunc()
	{
		m_hierarchieInformation = [this]()
			{
				bool tmp_isSelected(false);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (ImGui::BeginCombo("Selected Texture", m_texture.expired() ? "No Texture Selected" : m_texture.lock()->getName().c_str()))
				{
					if (!m_texture.expired())
					{
						if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_isSelected))
						{
							if (!m_texture.expired())
								m_texture.lock()->isUsedByAComponent() = false;

							m_texture.reset();
						}
					}

					for (auto& component : getParent()->getComponents())
					{
						if (auto tmp_textureConponent = std::dynamic_pointer_cast<lc::Texture>(component))
						{
							if (!tmp_textureConponent->isUsedByAComponent())
							{
								if (ImGui::Selectable(std::string(tmp_textureConponent->getName() + "##" + std::to_string(tmp_textureConponent->getID())).c_str(), tmp_isSelected))
								{
									if (!m_texture.expired())
										m_texture.lock()->isUsedByAComponent() = false;

									m_texture = tmp_textureConponent;
									tmp_textureConponent->isUsedByAComponent() = true;
								}
							}
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				ImGui::InputText("Key Name", m_baseName, 150);
				ImGui::DragInt4("Base Rect", m_baseIntRect);
				ImGui::DragInt("Base Max Frame", &m_baseMaxFrame);
				ImGui::DragFloat("Base Frame Timer", &m_baseFrameTimer, 0.001f);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (!m_texture.expired())
					if (ImGui::Button("Create Animation Key"))
					{
						if (m_animationKeys.find((m_baseName == "" ? "No_Name" : m_baseName)) == m_animationKeys.end())
							m_animationKeys.emplace((m_baseName == "" ? "No_Name" : m_baseName), lc::AnimationKey((m_baseName == "" ? "No_Name" : m_baseName), m_baseMaxFrame, m_baseFrameTimer, m_baseIntRect));
						else
						{
							auto& tmp_anim = m_animationKeys[(m_baseName == "" ? "No_Name" : m_baseName)];
							tmp_anim->getBaseIntRect() = m_baseIntRect;
							tmp_anim->getFrameTimer() = m_baseFrameTimer;
							tmp_anim->getMaxFrame() = m_baseMaxFrame;
							tmp_anim->getFramesRects().clear();
							for (int i = 0; i < tmp_anim->getMaxFrame(); i++)
							{
								tmp_anim->getFramesRects().push_back({ tmp_anim->getBaseIntRect().left * i, tmp_anim->getBaseIntRect().top,
																	  tmp_anim->getBaseIntRect().width, tmp_anim->getBaseIntRect().height });
							}
						}
					}

				for (auto& animationKey : m_animationKeys)
				{
					if (ImGui::TreeNodeEx(std::string(animationKey.first + "##" + std::to_string(m_ID)).c_str(), ImGuiTreeNodeFlags_Selected))
					{
						ImGui::Text(std::string(animationKey.second->getName() + " : Name").c_str());
						ImGui::Text(std::string(std::to_string(animationKey.second->getMaxFrame()) + " : Max Frame").c_str());
						ImGui::Text(std::string(std::to_string(animationKey.second->getFrameTimer()) + " : Frame Timer").c_str());
						if (ImGui::TreeNodeEx("All Frame Rect", ImGuiTreeNodeFlags_Selected))
						{
							for (auto& frameRect : animationKey.second->getFramesRects())
								ImGui::Text(std::string(std::to_string(frameRect.left) + ", " + std::to_string(frameRect.top) + ", " + 
														std::to_string(frameRect.width) + ", " + std::to_string(frameRect.height)).c_str());
	
							ImGui::TreePop();
						}

						ImGui::TreePop();
					}
				}
			};
	}
}