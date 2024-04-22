#pragma once
#include "EditorState.h"

class Game
{
	WindowManager m_windowManager;
	StateList m_state;

	void update();
	void render();
public:
	Game();
	Game(int width, int height, std::string title, bool fullscreen);
	~Game();

	void runGame();

};

 