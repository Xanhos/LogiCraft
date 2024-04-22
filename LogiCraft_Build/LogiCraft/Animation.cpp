#include "Animation.h"

namespace lc
{
	Animation::Animation()
		: m_baseFrameTimer(0.f)
	{
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
	}
}