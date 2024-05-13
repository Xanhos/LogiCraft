#include "AI.h"
#include "GameObject.h"
#include "Tools_LC.h"
#include "SFML_ENGINE/WindowManager.h"
#include "Button.h"
#include "imgui-SFML.h"
#include "nfd.h"

std::shared_ptr<lc::GameObject> load(WindowManager& window_)
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("lcg",nullptr, &outPath);
	fs::path path_ = (std::string(outPath));
	return lc::GameObject::LoadScene(path_.parent_path().filename().string(),window_);
}


int main()
{

	WindowManager window(1920, 1080, "test", false);
	lc::Button::SetupFunctionPool(window);

	sf::Listener::setPosition(0,10,0);
	sf::Listener::setDirection(0,0,-1);
	sf::Listener::setUpVector(0,1,0);
	sf::Listener::setGlobalVolume(90);
		
	auto world = load(window);
	auto player = lc::GameObject::CreateGameObject("Player", 8);
	const sf::Vector2f viewSize(1920 * 2, 1080 * 2);
	sf::View view(viewSize / 2.f, viewSize);
	window.getWindow().setView(view);
	sf::Vector2f Viewposition = viewSize/ 2.f;
	
	Tools::restartClock();
	while (!window.isDone())
	{
		Tools::restartClock();
		window.EventUpdate();

		world->Update(window);
		window.getWindow().setTitle(std::to_string(Tools::frameRate()));
		if (KEY(Add))
		{
			view.zoom(0.999f);
		}
		if (KEY(Subtract))
		{
			view.zoom(1.001f);
		}

		if(KEY(D))
			Viewposition.x += 500.f * Tools::getDeltaTime();
		if(KEY(Q))
			Viewposition.x -= 500.f * Tools::getDeltaTime();
		if(KEY(Z))
			Viewposition.y -= 500.f *Tools::getDeltaTime();
		if(KEY(S))
			Viewposition.y += 500.f *Tools::getDeltaTime();
		
		view.setCenter(Viewposition);

		window.getWindow().setView(view);
		window.clear(sf::Color::Black);
		world->Draw(window);
		window.display();
	}


	return 1;
}
