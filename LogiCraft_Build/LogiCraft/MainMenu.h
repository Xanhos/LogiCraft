#pragma once
#include "ToolsBar.h"
#include "BrowserContent.h"
#include "Hierarchie.h"


class MainMenu
{
public:
	MainMenu() {};
	~MainMenu() {};
	void Update(std::shared_ptr<lc::GameObject> _gameObject, Viewports& _viewport, WindowManager& _window);
	void Draw(std::shared_ptr<lc::GameObject> _gameObject, Viewports& _viewport, WindowManager& _window);

	ToolsBar& getToolsBar() { return m_toolsBar; }
	BrowserContent& getBrowserContent() { return m_browser; }
	Hierarchie& getHierarchie() { return m_hierarchie; }
private: 
	ToolsBar m_toolsBar;
	BrowserContent m_browser;
	Hierarchie m_hierarchie;
};
