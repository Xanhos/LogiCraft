#pragma once
#include "State.h"
#include "MainMenu.h"
#include "Viewport.h"

class EditorState : public State
{
private:
	MainMenu m_mainMenu;
	Viewports m_viewports;

	std::shared_ptr<lc::GameObject> m_scene;
public:
	EditorState(WindowManager& window, StateList* listState);
	~EditorState() {};

	void init();
	void updateEvent(sf::Event& _event);
	void update();
	void render();
	void pushState(char data);
};

