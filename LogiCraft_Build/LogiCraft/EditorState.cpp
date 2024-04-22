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