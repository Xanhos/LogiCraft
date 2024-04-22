#pragma once
#include "Viewport.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Button.h"
#include "AI.h"
#include "Event.h"
#include "Font.h"
#include "tools_imgui.h"
#include "BrowserContent.h"

typedef std::map<unsigned char, std::string> Layers;
typedef std::pair<unsigned char, std::string> Layer;

class ToolsBar
{
public:
	ToolsBar();
	~ToolsBar() {};

	void Update(std::shared_ptr <lc::GameObject> _game_object, WindowManager& _window, Viewports& _viewport);

	static Layers GetLayers();
	static Layer GetActualLayer();
private:
	void InputCheck(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport);
	void AddViewport(Viewports& _viewport);
	void OpenViewportOption(Viewports& _viewport);
	void New(std::shared_ptr<lc::GameObject> _game_object, Viewports& _viewport);
	void Load(std::shared_ptr <lc::GameObject> _game_object, std::string path, Viewports& _viewport);
	void ShowHelp();

	void Save(std::shared_ptr <lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window);

	void Exit(WindowManager& _window);
	void ChangeLayer(Layer _newLayer);

	bool m_isSaving = false;
	bool m_isLoading = false;
	bool m_showingHelp = false;

	std::string m_path {'\0'};

	static Layer s_actualLayer;

	static Layers s_layers;
};

