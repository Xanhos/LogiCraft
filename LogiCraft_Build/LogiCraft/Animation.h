
/*------------------------------------LICENSE------------------------------------
MIT License

Copyright (c) [2024] [GRALLAN Yann]
Copyright (c) [2024] [LESAGE Charles]
Copyright (c) [2024] [MENA-BOUR Samy]

This software utilizes code from the following GitHub repositories, which are also licensed under the MIT License:

- [SFML](https://github.com/SFML)
- [ImGUI](https://github.com/ocornut/imgui)
- [ImNodes](https://github.com/Nelarius/imnodes)
- [SFML-Manager](https://github.com/Xanhos/Cpp-Manager-for-SFML)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
---------------------------------------------------------------------------------*/

#pragma once
#include "Ressource.h"
#include "Texture.h"

namespace lc
{
	class AnimationKey
	{
	public:
		AnimationKey();
		AnimationKey(std::string _name, int _maxFrame, float _frameTimer, sf::IntRect _frameRect);
		~AnimationKey();

		std::string& getName();

		std::vector<sf::IntRect>& getFramesRects();

		sf::IntRect& getBaseIntRect();

		int& getMaxFrame();
		int& getActualFrame();

		float& getFrameTimer();
		float& getFrameTime();
	private:
		std::string m_name;

		std::vector<sf::IntRect> m_framesRects;

		sf::IntRect m_baseFrameRect;

		int m_maxFrame;
		int m_actualFrame;

		float m_frameTimer;
		float m_frameTime;
	};

	class Animation : public lc::Ressource
	{
	public:
		Animation();
		~Animation();

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;

		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;
	private:
		sf::IntRect m_baseIntRect;
		float m_baseFrameTimer;
		std::string m_baseName;
		int m_baseMaxFrame;

		std::weak_ptr<lc::Texture> m_texture;

		std::weak_ptr<AnimationKey> m_actualAnimationKey;
		std::unordered_map<std::string, std::shared_ptr<AnimationKey>> m_animationKeys;
	};
}