#include "GameObject.h"
#include "Tools_LC.h"
#include "SFML_ENGINE/WindowManager.h"

int main()
{
	auto world = lc::GameObject::LoadScene("Test");
	WindowManager window(1920, 1080, "test", false);
	const sf::Vector2f viewSize(1920 * 2, 1080 * 2);
	sf::View view(viewSize / 2.f, viewSize);
	window.getWindow().setView(view);

	Tools::restartClock();
	while (!window.isDone())
	{
		Tools::restartClock();
		window.EventUpdate();

		world->Update(window);

		if (KEY(Add))
		{
			view.zoom(0.99f);
		}
		if (KEY(Subtract))
		{
			view.zoom(1.01f);
		}



		window.getWindow().setView(view);
		window.clear(sf::Color::Black);
		world->Draw(window);
		window.display();
	}


	return 1;
}
