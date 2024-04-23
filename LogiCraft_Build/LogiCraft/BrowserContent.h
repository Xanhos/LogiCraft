#pragma once
#include "SFML_ENGINE/WindowManager.h"

#include "tools_imgui.h"
#include "Folder.h"
#include "Viewport.h"
#include "GameObject.h"

#undef CreateFont
#undef CreateEvent

class BrowserContent
{
public:
	BrowserContent() : m_mainFolder("../ASSETS", nullptr) { m_newFolderName = '\0'; m_RessourceName = '\0'; };
	~BrowserContent() { m_mainFolder.Save(); };
	/**
	 * @brief Update the BrowserContent
	 * 
	 * \param _object GameObejct to update (generally its the scene)
	 * \param _window SFML window
	 * \param _viewport Viewport to update
	 */
	void Update(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);
	/**
	* @brief Draw the BrowserContent
	* 
	* \param _object GameObejct to draw (generally its the scene)
	* \param _window SFML window
	* \param _viewport Viewport to draw
	*/

	void Draw(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);
private:
	/**
	 * @brief Create a imgui window to select the type and create a new ressource
	 * 
	 */
	void CreateRessources();

	/**
	 * @brief Create a imgui window to create a new Texture
	 *
	 */
	void CreateTexture();

	/**
	 * @brief Create a imgui window to create a new Font
	 * 
	 */
	void CreateFont();

	/**
	 * @brief Create a imgui window to create a new Event
	 * 
	 */
	void CreateEvent();

	/**
	 * @brief Put a new ressource into the scene
	 * 
	 * \param _object the gameobject to which we add the resource(generally its the scene)
	 * \param _window SFML window Manager
	 * \param _viewport All the viewports of the software
	 */
	void PutRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);

	/**
	 * @brief Add a component to the gameobject
	 * 
	 * \param _object the gameobject to which we add the resource(generally its the scene)
	 * \param _window SFML window Manager 
	 * \param _viewport All the viewports of the software
	 */
	void AddComponent(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewport& _viewport);

	/**
	* @brief Draw the sliding ressources
	* 
	* \param _object the gameobject to which we add the resource(generally its the scene)
	* \param _window SFML window Manager
	* \param _viewport All the viewports of the software
	*/
	void DrawSlidingRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);

	lc::Ressource::TYPE m_selectedRessourceType = lc::Ressource::TYPE::NONE;
	lc::Ressource::TYPE m_sortedRessources = lc::Ressource::TYPE::NONE;

	std::string m_sortString = {"ALL"};
	std::string m_selectedFolder = {};
	std::string m_selectedRessources = {};
	std::string m_RessourceName;
	std::string m_newFolderName;
	std::string m_newObjectName;

	sf::IntRect m_newTextureRect;

	bool m_creatingNewRessources = false;
	bool m_isSlidingRessources = false;
	bool m_selectingFilter = false;
	// put ressource
	bool m_choosingName = false;
	bool m_nameIsReady = false;
	bool m_showError = false;
	sf::Vector2f m_putPosition;

	Folder m_mainFolder;
	lc::Ressource* m_slidingRessources = nullptr;

};
