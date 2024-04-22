#pragma once
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

#include "tools_imgui.h"
#include "GameObject.h"
#include "Viewport.h"
 
typedef std::list<std::weak_ptr<lc::GameObject>> WeakPtrList;
typedef std::list<std::shared_ptr<lc::GameObject>> SharedPtrList;

class Hierarchie
{
public:
	Hierarchie();
	~Hierarchie();

	void Update(std::shared_ptr<lc::GameObject> _scene, WindowManager& _window, Viewports& _viewports);

	WeakPtrList& getSelectedGameObject();
private:
	/*
	* @brief This function is here to display all the Object in the world in the hierarchie window.
	*/
	void GameObjectsDisplay(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene, std::list<std::shared_ptr<lc::GameObject>>* _gameObjectList);

	/*
	* @brief This function is here to display all the Object that as been selected in the selected object window.
	*/
	void SelectedObjectsDisplay(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewports);

	void CreateNewObjectBehavior();

	/*
	* @brief A function to create a copy of the selected object when CTRL+C are press.
	* @brief Then it will be create when CTRL+V are press.
	*/
	void CopyPaste(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewport);

	/*
	* @brief A function to display arrow to move up or down the object.
	* @brief To change the diplay order.
	* 
	* @param _gameObject : Actual Object that will be moved.
	* @param _gameObjectList : Is the object list of the parent of _gameObject to move _gameObject move up or down in it. 
	*/
	void MoveDownUpBehavior(std::shared_ptr<lc::GameObject> _gameObject, std::list<std::shared_ptr<lc::GameObject>>* _gameObjectList);

	/*
	* @brief A function to move object in other object by drag it and drop it in the object.
	* 
	* @param _gameObject : The object that will be update to know if dragged or something need to be drop in.
	* @param _scene : The scene is here to know if the actual _gameObject is the scene so it can't be drag and drop.
	*/
	void DragAndDropBehavior(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene);

	/*
	* @brief A function to select a gameObject.
	* @brief by pressing CTRL+LeftClick the user can select multiple objects at once.
	* 
	* @param _gameObject : Its the object that is actually updated and if its selected or not.
	*/
	void SelectionBehavior(std::shared_ptr<lc::GameObject> _gameObject);

	/*
	* @brief A function to know if the _gameObject is a child of a selected object.
	* @brief Its to prevent double copy of an object.
	* 
	* @param _gameObject : The object that will be verified.
	* @param _scene : The object that is the WORLD its here to know if _gameObject is a child of it.
	*/
	bool IsAChldOfASltObj(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene);

private:
	WeakPtrList m_selectedGameObjects;
	SharedPtrList m_copyPasteObjects;

	std::weak_ptr<lc::GameObject> m_clickedObject;

	bool m_hasMoveAnObject;
	bool m_wantToCreateAGameObject;

	float m_copyPasteTimer;
};