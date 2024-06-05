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
	// bt::Node::init_custom_condition();
	// WindowManager window(1920, 1080, "test", false);
	// lc::Button::SetupFunctionPool(window);
	//
	// sf::Listener::setPosition(0,10,0);
	// sf::Listener::setDirection(0,0,-1);
	// sf::Listener::setUpVector(0,1,0);
	// sf::Listener::setGlobalVolume(90);
	// 	
	// auto world = load(window);
	// auto player = lc::GameObject::CreateGameObject("Player", 8);
	// const sf::Vector2f viewSize(1920 * 2, 1080 * 2);
	// sf::View view(sf::Vector2f(0.f, 1560.f), viewSize);
	// window.getWindow().setView(view);
	// sf::Vector2f Viewposition = viewSize/ 2.f;
	//
	// Tools::restartClock();
	// while (!window.isDone())
	// {
	// 	Tools::restartClock();
	// 	window.EventUpdate();
	//
	// 	world->Update(window);
	// 	//window.getWindow().setTitle(std::to_string(Tools::frameRate()));
	// 	if (KEY(Add))
	// 	{
	// 		view.zoom(0.999f);
	// 	}
	// 	if (KEY(Subtract))
	// 	{
	// 		view.zoom(1.001f);
	// 	}
	//
	// 	if(KEY(D))
	// 		Viewposition.x += 500.f * Tools::getDeltaTime();
	// 	if(KEY(Q))
	// 		Viewposition.x -= 500.f * Tools::getDeltaTime();
	// 	if(KEY(Z))
	// 		Viewposition.y -= 500.f *Tools::getDeltaTime();
	// 	if(KEY(S))
	// 		Viewposition.y += 500.f *Tools::getDeltaTime();
	// 	
	// 	view.setCenter(Viewposition);
	//
	// 	window.getWindow().setView(view);
	// 	window.clear(sf::Color::Black);
	// 	world->Draw(window);
	// 	window.display();
	// }
	//
	//
	// return 1;


	while(1)
	{
		
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_PickFolder(nullptr, &outPath);
		fs::path path_ = std::string(outPath);

		std::ifstream exportIn(path_.string() + "/export.lcg");
		std::string string, actualLine;

		while (!exportIn.eof())
		{
			std::getline(exportIn,actualLine);
			if(actualLine.find("6 Animation") != std::string::npos)
				string += actualLine + " 0 0\n";
			else
				string += actualLine + "\n";			
		}
		exportIn.close();
		
		std::ofstream exportOut(path_.string() + "/export.lcg");
		exportOut << string;
		
		std::ifstream saveIn(path_.string() + "/save.lcp");
		string = ""; actualLine = "";

		while (!saveIn.eof())
		{
			std::getline(saveIn,actualLine);
			if(actualLine.find("6 Animation") != std::string::npos)
				string += actualLine + " 0 0\n";
			else
				string += actualLine + "\n";			
		}
		saveIn.close();
		
		std::ofstream saveOut(path_.string() + "/save.lcp");
		saveOut << string;

		
		// std::list<std::pair<std::string,sf::Image>> image_list;
		// for(auto& it : fs::directory_iterator(path_))
		// {
		// 	if(it.path().extension() == ".png")
		// 	{
		// 		std::pair<std::string,sf::Image> pair;
		// 		pair.first = it.path().string();
		// 		std::cout << "Loading " << pair.first << "\n";
		// 		pair.second.loadFromFile(it.path().string());
		// 		image_list.push_back(pair);
		// 	}
		// }
		//
		// for(auto& it : image_list)
		// {
		// 	std::cout << "Checking " << it.first << "\n";
		// 	bool need_to_delete = true;
		// 	for(int y = 0;y < it.second.getSize().y;y++)
		// 	{
		// 		for(int x = 0; x < it.second.getSize().x; x++)
		// 		{
		// 			if(it.second.getPixel(x,y) != sf::Color::Transparent)
		// 			{
		// 				need_to_delete = false;
		// 				break;
		// 			}
		// 		}
		// 	}
		// 	if(need_to_delete)
		// 		fs::remove(it.first);
		// }		
	}
}
