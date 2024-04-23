
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

#include "Hierarchie.h"
#include "ToolsBar.h"
#include "RigidBody.h"
#include "Particule.h"
#include "Button.h"
#include "Texture.h"

Hierarchie::Hierarchie()
	: m_hasMoveAnObject(false), m_wantToCreateAGameObject(false), m_copyPasteTimer(0.f)
{
}

Hierarchie::~Hierarchie()
{
}

void Hierarchie::Update(std::shared_ptr<lc::GameObject> _scene, WindowManager& _window, Viewports& _viewports)
{
	m_hasMoveAnObject = false;

	m_selectedGameObjects.remove_if([](std::weak_ptr<lc::GameObject> _selectedGameObject)
		{
			return _selectedGameObject.expired();
		});

	this->CreateNewObjectBehavior();

	ImGui::Begin("Hierarchie##lc");
	this->GameObjectsDisplay(_scene, _scene, nullptr);
	ImGui::End();

	ImGui::Begin("Selected Object Parameter##lc");
	this->SelectedObjectsDisplay(_scene, _viewports);
	ImGui::End();

	this->CopyPaste(_scene, _viewports);
}

ObjWeakPtrList& Hierarchie::getSelectedGameObject()
{
	return m_selectedGameObjects;
}

void Hierarchie::GameObjectsDisplay(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene, std::list<std::shared_ptr<lc::GameObject>>* _gameObjectList)
{
	if (ImGui::TreeNodeEx(std::string(_gameObject->getName() + " <ID:" + std::to_string(_gameObject->getID()) + ">").c_str(),
		ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap,
		_gameObject->getName().c_str()))
	{
		this->SelectionBehavior(_gameObject);

		this->DragAndDropBehavior(_gameObject, _scene);

		this->MoveDownUpBehavior(_gameObject, _gameObjectList);

		for (auto& gameObject : _gameObject->getObjects())
		{
			GameObjectsDisplay(gameObject, _scene, &_gameObject->getObjects());

			if (m_hasMoveAnObject)
				break;
		}

		ImGui::TreePop();
	}
	else
	{
		this->SelectionBehavior(_gameObject);

		this->DragAndDropBehavior(_gameObject, _scene);

		this->MoveDownUpBehavior(_gameObject, _gameObjectList);
	}

	if (_gameObject->getName() == "")
		_gameObject->getName() = "No_Name";
	else
		Tools::ReplaceCharacter(_gameObject->getName(), ' ', '_');
}

void Hierarchie::SelectedObjectsDisplay(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewports)
{
	static bool test = false;
	for (auto& selectedObject : m_selectedGameObjects)
	{
		if (!selectedObject.expired())
		{
			auto tmp_object = selectedObject.lock();

			ImGui::Text(std::string("Name : " + tmp_object->getName()).c_str());
			if (tmp_object != _scene)
			{
				if (ImGui::Button(std::string("Delete Object##<ID:" + std::to_string(tmp_object->getID())).c_str()))
				{
					auto tmp_parent = tmp_object->getParent();
					tmp_parent->removeObject(tmp_object->getName(), tmp_object->getID());
				}

				ImGui::Checkbox(std::string("is Visible##<ID:" + std::to_string(tmp_object->getID())).c_str(), &tmp_object->isVisible());
				ImGui::Checkbox(std::string("is Lock##<ID:" + std::to_string(tmp_object->getID())).c_str(), &tmp_object->isLock());
				
				if (ImGui::BeginCombo(std::string("Change Depth").c_str(), ToolsBar::GetLayers().find(tmp_object->getDepth())->second.c_str(), ImGuiComboFlags_WidthFitPreview))
				{
					bool isSelected(false);
					for (auto& layer : ToolsBar::GetLayers())
					{
						if (ImGui::Selectable(layer.second.c_str(), &layer))
						{
							tmp_object->setDepth(layer.first);
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Add Button"))
				{
					bool isButton = false;
					for (auto& components : tmp_object->getComponents())
					{
						if (components->getTypeName() == "Button")
						{
							isButton = true;
						}
					}
					if (!isButton)
						tmp_object->addComponent<lc::Button>();
				}

				ImGui::InputText("Change Name", tmp_object->getName(), 150, ImGuiInputTextFlags_EnterReturnsTrue);

				if (ImGui::TreeNodeEx(std::string("Transform##" + std::to_string(tmp_object->getID())).c_str(),
					ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					ImGui::Text(std::string("X " + std::to_string(tmp_object->getTransform().getSize().x) + ", Y " + std::to_string(tmp_object->getTransform().getSize().y) + " : Size").c_str());
					ImGui::PushItemWidth(200.f);
					ImGui::DragFloat2("Position", tmp_object->getTransform().getPosition());

					ImGui::DragFloat2("Origin", tmp_object->getTransform().getOrigin());

					ImGui::DragFloat2("Scale", tmp_object->getTransform().getScale(), 0.01f);

					ImGui::DragFloat("Rotation", &tmp_object->getTransform().getRotation());
					ImGui::PopItemWidth();

					if (ImGui::Button("Reset Position"))
						tmp_object->getTransform().getPosition() = sf::Vector2f(0.f, 0.f);

					if (ImGui::Button("Reset Origin"))
						tmp_object->getTransform().getOrigin() = sf::Vector2f(0.f, 0.f);

					if (ImGui::Button("Reset Scale"))
						tmp_object->getTransform().getScale() = sf::Vector2f(1.f, 1.f);

					if (ImGui::Button("Reset Rotation"))
						tmp_object->getTransform().getRotation() = 0.f;

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

					if (ImGui::Button("Add RigidBody"))
						tmp_object->addComponent<lc::RigidBody>();

					if (ImGui::Button("Add Particules System"))
						tmp_object->addComponent<lc::Particules>();

					if (tmp_object->hasComponent("RigidBody") and !tmp_object->hasComponent("AI"))
						if (ImGui::Button("Add AI"))
							tmp_object->addComponent<lc::AI>();
					
					

					ImGui::TreePop();
				}

				for (auto component = tmp_object->getComponents().begin(); component != tmp_object->getComponents().end(); component++)
				{
					(*component)->Hierarchie_Draw(tmp_object->getID(), component, tmp_object->getComponents());
				}

				if (KEY(Delete))
				{
					auto tmp_parent = tmp_object->getParent();
					tmp_parent->removeObject(tmp_object->getName(), tmp_object->getID());
				}
			}
		}
	}
}

void Hierarchie::CreateNewObjectBehavior()
{
	if (m_wantToCreateAGameObject)
	{
		if (ImGui::Begin("Create A GameObject##lc", &m_wantToCreateAGameObject))
		{
			static std::string tmp_NewGameObjectName("");
			static std::pair<unsigned char, std::string> tmp_DepthName = *ToolsBar::GetLayers().begin();
			bool isSeleted = false;
			
			ImGui::PushItemWidth(200.f);
			ImGui::InputText("New GameObject Name", tmp_NewGameObjectName, 40);
			ImGui::SameLine();

			if (ImGui::BeginCombo("New GameObject Depth", tmp_DepthName.second.c_str()))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				float w = ImGui::CalcItemWidth();
				float spacing = style.ItemInnerSpacing.x;
				float button_sz = ImGui::GetFrameHeight();
				ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
				for (auto& depth : ToolsBar::GetLayers())
					if (ImGui::Selectable(depth.second.c_str(), &isSeleted))
						tmp_DepthName = depth;
				ImGui::PopItemWidth();
				ImGui::EndCombo();
			}

			ImGui::PopItemWidth();

			ImGui::PushItemWidth(200.f);
			if (ImGui::Button("Create GameObject"))
			{
				if (!m_clickedObject.expired())
					m_clickedObject.lock()->addObject(lc::GameObject::CreateGameObject(tmp_NewGameObjectName, tmp_DepthName.first));

				m_wantToCreateAGameObject = false;
			}
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}
}

void Hierarchie::CopyPaste(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewport)
{
	m_copyPasteTimer += Tools::getDeltaTime();
	if (KEY(LControl) && KEY(C) && m_copyPasteTimer > 0.2f && !m_selectedGameObjects.empty())
	{
		m_copyPasteObjects.clear();
		for (auto& sltObject : m_selectedGameObjects)
		{
			if (!this->IsAChldOfASltObj(sltObject.lock(), _scene))
			{
				auto tmp_object = sltObject.lock()->Clone();
				tmp_object->getTransform().getPosition() = sltObject.lock()->getTransform().getPosition() + sf::Vector2f(10.f, 10.f);
				m_copyPasteObjects.push_back(tmp_object);
			}
		}

		m_copyPasteTimer = 0.f;
	}
	
	if (KEY(LControl) && KEY(V) && m_copyPasteTimer > 0.2f && !m_selectedGameObjects.empty())
	{
		m_selectedGameObjects.clear();
		for (auto& newObject : m_copyPasteObjects)
		{
			_scene->addObject(newObject);
			this->AddSelectedObject(newObject);

			newObject = newObject->Clone();
			this->VerifyThePasteObject(newObject);
		}

		_viewport.getActualSelectedObjectNumber() = 0u;

		m_copyPasteTimer = 0.f;
	}
}

void Hierarchie::AddSelectedObject(std::shared_ptr<lc::GameObject> _object)
{
	_object->getName() += "_copie_" + std::to_string(_object->getID());

	m_selectedGameObjects.push_back(_object);
	for (auto& object : _object->getObjects())
	{
		this->AddSelectedObject(object);
	}
}

void Hierarchie::VerifyThePasteObject(std::shared_ptr<lc::GameObject>& _object)
{
	_object->getName() = _object->getName().erase(_object->getName().find_last_of('_'));
	_object->getName() = _object->getName().erase(_object->getName().find_last_of('_'));
	for (auto& child : _object->getObjects())
	{
		this->VerifyThePasteObject(child);
	}
}



void Hierarchie::MoveDownUpBehavior(std::shared_ptr<lc::GameObject> _gameObject, ObjSharedPtrList* _gameObjectList)
{
	if (_gameObjectList != nullptr)
		if (!_gameObjectList->empty())
		{
			if (_gameObject != *std::prev(_gameObjectList->end()))
			{
				ImGui::SameLine();
				if (ImGui::ArrowButtonEx(std::string("Down" + std::to_string(_gameObject->getID())).c_str(), ImGuiDir_Down, sf::Vector2f(15.f, 13.f)))
				{
					auto tmp_gameObject = std::find_if(_gameObjectList->begin(), _gameObjectList->end(), [&_gameObject](std::shared_ptr<lc::GameObject>& _gameObjectComp)
						{
							return (_gameObjectComp->getName() == _gameObject->getName() && _gameObjectComp->getID() == _gameObject->getID());
						});

					auto tmp_changeGB = *tmp_gameObject;
					*tmp_gameObject = *std::next(tmp_gameObject);
					*std::next(tmp_gameObject) = tmp_changeGB;

					m_hasMoveAnObject = true;
				}
			}

			if (_gameObject != *_gameObjectList->begin())
			{
				ImGui::SameLine();
				if (ImGui::ArrowButtonEx(std::string("Up" + std::to_string(_gameObject->getID())).c_str(), ImGuiDir_Up, sf::Vector2f(15.f, 13.f)))
				{
					auto tmp_gameObject = std::find_if(_gameObjectList->begin(), _gameObjectList->end(), [&_gameObject](std::shared_ptr<lc::GameObject>& _gameObjectComp)
						{
							return (_gameObjectComp->getName() == _gameObject->getName() && _gameObjectComp->getID() == _gameObject->getID());
						});

					auto tmp_changeGB = *tmp_gameObject;
					*tmp_gameObject = *std::prev(tmp_gameObject);
					*std::prev(tmp_gameObject) = tmp_changeGB;

					m_hasMoveAnObject = true;
				}
			}
		}
}

void Hierarchie::DragAndDropBehavior(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene)
{
	if (ImGui::BeginDragDropSource() && _scene != _gameObject)
	{
		ImGui::SetDragDropPayload("Game Object Drag", &*_gameObject, sizeof(*_gameObject));
		ImGui::Text(std::string(_gameObject->getName() + "<ID:" + std::to_string(_gameObject->getID()) + "> is begin dragged").c_str());
		ImGui::EndDragDropSource();
	}

	if (BeginDragDropTarget())
	{
		if (auto tmp_playLoad = ImGui::AcceptDragDropPayload("Game Object Drag"))
			if (auto tmp_gameObject = reinterpret_cast<lc::GameObject*>(tmp_playLoad->Data))
			{
				//Get the parent of the Object that need to be move.
				auto tmp_objectParent = tmp_gameObject->getParent();
				//So if the user decide to move a parent in is child or grand-child...
				//First he need to be verify to see if the Object that as been move got moved in his child with the objectIsParent function.
				if (tmp_gameObject->objectIsParent(_gameObject->getName(), _gameObject->getID()))
				{
					//Then we set his child to be the child of his parent
					/*	+------------------------+
						|                        |
						|        His Parent>-----+
						|            X           |
						|            X           |
						|        GameObject      |-- Push into his New Parent
						|            |           |
						|            v           |
						|        His Child<----- +
						| _______________________| */
					for (auto& gameObject : tmp_gameObject->getObjects())
						tmp_objectParent->addObject(gameObject);

					//And then we take clear his child because they got moved in his parent.
					tmp_objectParent->getObject(tmp_gameObject->getName(), tmp_gameObject->getID())->getObjects().clear();
				}
				//And then if the Object that the user want to move his the child of the Object where he need to be move nothing happend.
				if (tmp_objectParent != _gameObject)
				{
					//Then we push the Object into his new parent. 
					_gameObject->addObject(tmp_objectParent->getObject(tmp_gameObject->getName(), tmp_gameObject->getID()));
					//And we remove it from his old parent.
					tmp_objectParent->removeObject(tmp_gameObject->getName(), tmp_gameObject->getID());
				}

				m_hasMoveAnObject = true;
			}
		ImGui::EndDragDropTarget();
	}
}

void Hierarchie::SelectionBehavior(std::shared_ptr<lc::GameObject> _gameObject)
{
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		if (!KEY(LControl))
			m_selectedGameObjects.clear();

		this->SelectionVerifyBehavior(_gameObject);
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		m_clickedObject = _gameObject;
		m_wantToCreateAGameObject = true;
	}
}

void Hierarchie::SelectionVerifyBehavior(std::shared_ptr<lc::GameObject> _gameObject)
{
	if (KEY(LShift))
	{
		if (std::find_if(m_selectedGameObjects.begin(), m_selectedGameObjects.end(), [&_gameObject](std::weak_ptr<lc::GameObject> _wptrObject)
			{
				if (!_wptrObject.expired())
					if (_gameObject == _wptrObject.lock())
						return true;

				return false;
			}) == m_selectedGameObjects.end())
			m_selectedGameObjects.push_back(_gameObject);

		for (auto& object : _gameObject->getObjects())
			this->SelectionVerifyBehavior(object);
	}
	else
	{
		auto tmp_selected = std::find_if(m_selectedGameObjects.begin(), m_selectedGameObjects.end(), [&_gameObject](std::weak_ptr<lc::GameObject> _wptrObject)
			{
				if (!_wptrObject.expired())
					if (_gameObject == _wptrObject.lock())
						return true;

				return false;
			});

		if (tmp_selected == m_selectedGameObjects.end())
			m_selectedGameObjects.push_back(_gameObject);
		else
			m_selectedGameObjects.erase(tmp_selected);
	}
}

bool Hierarchie::IsAChldOfASltObj(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene)
{
	if (_gameObject->getParent() == _scene)
		return false;

	for (auto& sltObject : m_selectedGameObjects)
	{
		if (sltObject.lock() == _gameObject->getParent())
			return true;
	}

	if (this->IsAChldOfASltObj(_gameObject->getParent(), _scene))
		return true;

	return false;
}