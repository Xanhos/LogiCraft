
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

#include "EditorState.h"
#include "Button.h"
#include "RigidBody.h"
#include "AI.h"

EditorState::EditorState(WindowManager& window, StateList* listState) : State(window, listState)
{
}

void EditorState::init()
{
	PatronNode::SetupAllNode();
	m_viewports = Viewports();
	m_viewports.addViewport(std::make_shared<Viewport>(sf::Vector2f(250.f, 20.f), sf::Vector2f(1420.f, 560.f), (sf::Vector2u)m_windowManager.getSize(), " "));
	m_scene = lc::GameObject::CreateGameObject("WORLD");
	//auto AI = m_scene->addObject(lc::GameObject::CreateGameObject("OBJECT", 0));
	//AI->addComponent(std::make_shared<lc::AI>());
	
	m_isReady = true;
}

void EditorState::updateEvent(sf::Event& _event)
{
	m_viewports.UpdateEvent(_event);

	if (m_scene)
		m_scene->UpdateEvent(_event);
}

void EditorState::update()
{
	m_viewports.Update(m_mainMenu.getHierarchie().getSelectedGameObject(), m_scene, m_windowManager);
	m_scene->Update(m_windowManager);
	m_mainMenu.Update(m_scene, m_viewports, m_windowManager);
}

void EditorState::render()
{
	m_viewports.Draw(m_mainMenu.getHierarchie().getSelectedGameObject(), m_scene, m_windowManager);
	m_mainMenu.Draw(m_scene, m_viewports, m_windowManager);
}

void EditorState::pushState(char data)
{
}