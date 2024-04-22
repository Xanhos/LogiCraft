#include "MainMenu.h"
#include "tools_imgui.h"


void MainMenu::Update(std::shared_ptr<lc::GameObject> _gameObject, Viewports& _viewports, WindowManager& _window)
{
	m_hierarchie.Update(_gameObject, _window, _viewports);
	m_browser.Update(_gameObject, _window, _viewports);
	m_toolsBar.Update(_gameObject, _window, _viewports);
}

void MainMenu::Draw(std::shared_ptr<lc::GameObject> _gameObject, Viewports& _viewport, WindowManager& _window)
{
	m_browser.Draw(_gameObject, _window, _viewport);
}
