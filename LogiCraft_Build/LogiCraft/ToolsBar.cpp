#include <sstream>

#include "ToolsBar.h"

Layers ToolsBar::s_layers = { {8, "BackGround 7"},{7,"BackGround 6"},{ 6, "BackGround 5"},{5, "BackGround 4"},{4, "BackGround 3"},{ 3, "BackGround 2"},{2, "BackGround 1"},{1, "Player Plan"},{0, "Front Plan"} };
Layer ToolsBar::s_actualLayer = *ToolsBar::s_layers.begin();

ToolsBar::ToolsBar()
{}

void ToolsBar::Update(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport)
{
	InputCheck(object, _window, _viewport);

	if (ImGui::BeginMainMenuBar())
	{
		sf::Vector2f subMenuSize = { 0.f,20.f };

		Tools::IG::SetSpacing({ 25.f,0.f }, [&]
			{
				if (ImGui::BeginMenu("File"))
				{
					Tools::IG::SetSpacing(subMenuSize, [&]
						{
							if (ImGui::MenuItem("New", "Ctrl + N") || (KEY(LControl) && KEY(N))) { New(object, _viewport); }
							if (ImGui::MenuItem("Load", "Ctrl + O") || (KEY(LControl) && KEY(O))) { m_isLoading = true; }
							if (ImGui::MenuItem("Save As", "Ctrl + S") || (KEY(LControl) && KEY(S))) { m_isSaving = true; }
							if (ImGui::MenuItem("Exit", "Ctrl + W") ||(KEY(LControl) && KEY(W))) { Exit(_window); }
							ImGui::EndMenu();
						});

				}

				if (ImGui::BeginMenu(ToolsBar::s_actualLayer.second.c_str()))
				{
					Tools::IG::SetSpacing(subMenuSize, [&] 
						{
							for (auto& layer : ToolsBar::s_layers)
								if (ImGui::MenuItem(layer.second.c_str())) 
									ChangeLayer(layer);

							ImGui::EndMenu(); 
						});
				}

				if (ImGui::BeginMenu("Viewport"))
				{
					Tools::IG::SetSpacing(subMenuSize, [&] 
						{
							if (ImGui::MenuItem("Add viewport")) { AddViewport(_viewport); }
							if (ImGui::MenuItem("Viewport Option")) { OpenViewportOption(_viewport); }
							ImGui::EndMenu(); 
						});
				}

				if(ImGui::Button("Any Help ?")) { m_showingHelp = true; }
				if (m_showingHelp)
					ShowHelp();
				if (ImGui::BeginMenu(std::to_string(Tools::getDeltaTime()).c_str()))
				{
				}
				if (ImGui::BeginMenu(std::to_string(Tools::frameRate()).c_str()))
				{
				}
				ImGui::EndMainMenuBar();
			});
	}

	if (m_isSaving)
	{
		Tools::IG::CreateWindow("Save", m_isSaving, ImGuiWindowFlags_AlwaysAutoResize, [&] {
			InputText("Save Name", m_path, 100);
			if (Button("Save") && m_path[0] != '\0')
			{
				std::filesystem::create_directory(std::string("../Ressources/" + std::string(m_path)));
				Save(object, _viewport, _window.getWindow());
				m_isSaving = false;
				m_path.clear();
			}
			});
	}
	else if (m_isLoading)
	{
		Tools::IG::CreateWindow("Load", m_isLoading, ImGuiWindowFlags_AlwaysAutoResize, [&] {
			if (m_path[0] != '\0')
				Text(m_path.c_str());
			if (Button("Search .lcp file"))
			{
				Tools::IG::LoadRessourcesFromFile(m_path, "lcp");
			}
			if (Button("Load") && m_path[0] != '\0')
			{
				Load(object, m_path, _viewport);
				m_path.clear();
				m_isLoading = false;
			}
			});
	}
}

Layers ToolsBar::GetLayers()
{
	return s_layers;
}

Layer ToolsBar::GetActualLayer()
{
	return s_actualLayer;
}

void ToolsBar::InputCheck(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport)
{
	if (KEY(LControl) && KEY(N)) { New(object, _viewport); }
	if (KEY(LControl) && KEY(O)) { m_isLoading = true; }
	if (KEY(LControl) && KEY(S)) { m_isSaving = true; }
	if (KEY(LControl) && KEY(W)) { Exit(_window); }
}

void ToolsBar::AddViewport(Viewports& _viewport)
{
	_viewport.addViewport(std::make_shared<Viewport>(sf::Vector2f(250.f, 20.f), sf::Vector2f(1460.f, 560.f), sf::Vector2u(1920u, 1080u), " "));
}

void ToolsBar::OpenViewportOption(Viewports& _viewport)
{
	_viewport.isOptionOpen() = true;
}

void ToolsBar::New(std::shared_ptr<lc::GameObject> _game_object, Viewports& _viewport)
{
	lc::GameObject::getGeneralID() = 0u;

	_viewport.clearScreenZone();
	*_game_object = *lc::GameObject::CreateGameObject("WORLD");
	_viewport.clearViewport();

	AddViewport(_viewport);
	ChangeLayer(*ToolsBar::s_layers.begin());
}

void ToolsBar::Load(std::shared_ptr <lc::GameObject> _game_object, std::string path, Viewports& _viewport)
{
	New(_game_object, _viewport);
	std::ifstream load(path);
	_game_object->Load(load);

	for (auto& object : _game_object->getObjects())
	{
		for (auto& screen : _viewport.getAllScreenzone())
		{
			if (Tools::Collisions::point_rect(object->getTransform().getPosition(), screen.getScreenShape().getGlobalBounds()))
			{
				if(!screen.isUsed())
					_viewport.setScreenZoneToUse(screen);
			}
		}
	}
}

void ToolsBar::ShowHelp()
{
	ImGui::Begin("Here we are !", &m_showingHelp, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Here is the list of possible inputs :");
	ImGui::BulletText("(Z | Arrow UP) | (S | Arrow DOWN) | (Q | Arrow LEFT) | (D | Arrow RIGHT) | (Mouse Right + Drag) = > Scene's view deplacement.");
	ImGui::BulletText("F11 => Toggle fulscreen.");
	ImGui::BulletText("Delete => Delete all selected objects.");
	ImGui::BulletText("LControl + N => Clear and create a new scene.");
	ImGui::BulletText("LControl + O => Open the load menu.");
	ImGui::BulletText("LControl + S => Open the save menu.");
	ImGui::BulletText("LControl + W => Close the program.");
	ImGui::BulletText("LControl + Release Left Click => Add component to a hovered object.");
	ImGui::BulletText("LControl + Left Click on unused screen zone => Allow the screen zone.");
	ImGui::BulletText("LControl + Right Click on used screen zone => Free the screen zone.");
	ImGui::BulletText("LControl + Left Click on multiple objects => Multiple Selection.");
	ImGui::BulletText("LControl + C => Copy all selected objects.");
	ImGui::BulletText("LControl + V => Paste all selected objects.");

	ImGui::End();
}

void ToolsBar::Save(std::shared_ptr <lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window)
{
	std::ofstream save("../Ressources/" + std::string(m_path) + "/save.lcp");
	sf::RenderTexture render_texture;
	_game_object->Save(save, render_texture, s_actualLayer.first);

	render_texture.create(1920u * 2, 1080u * 2);
	for (auto& screen : _viewports.getAllScreenzone())
	{
		if (screen.isUsed())
		{
			render_texture.setView(sf::View(sf::Vector2f(screenSize.x + (screenSize.x * screen.getScreenIndex().x), screenSize.y + (screenSize.y * screen.getScreenIndex().y)), sf::Vector2f(screenSize.x * 2.f, screenSize.y * 2.f)));
			for (int depth = 0; depth <= 8; depth++)
			{
				render_texture.clear(sf::Color::Transparent);
				_game_object->SaveRenderer(render_texture, depth);
				sf::Image image;
				image = render_texture.getTexture().copyToImage();
				image.flipVertically();
				image.saveToFile("../Ressources/" + std::string(m_path) + "/" + std::string(std::to_string(screen.getScreenIndex().x) + "_" + std::to_string(screen.getScreenIndex().y)) + "_" + std::string(m_path) + "_layer" + std::to_string(depth) + ".png");
			}
		}
	}
}

void ToolsBar::Exit( WindowManager& _window)
{
	_window.getWindow().close();
}

void ToolsBar::ChangeLayer(Layer _newLayer)
{
	s_actualLayer = _newLayer;
}
