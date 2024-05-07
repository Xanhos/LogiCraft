
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

#include "BehaviorTree.h"

#include <tuple>
#include <tuple>
#include <tuple>

#include "Animation.h"

std::unordered_map<std::string, int> PatronNode::s_NodeContainer = {};
std::unordered_map<std::string, int> PatronNode::s_ConditionContainer = {};
DecoratorSaveMethod PatronNode::s_DecoratorSaveMethod = {};
DecoratorLoadMethod PatronNode::s_DecoratorLoadMethod = {};
DecoratorMethod PatronNode::s_DecoratorInitMethod = {};
DecoratorMethod PatronNode::s_DecoratorUpdateMethod = {};
DecoratorCopyMethod PatronNode::s_DecoratorCopyMethod = {};

void PatronNode::SetupAllNode()
{
	auto pushNode = [&](std::string name) { s_NodeContainer[name] = s_NodeContainer.size(); };
	auto pushCondition = [&](std::string name) { s_ConditionContainer[name] = s_ConditionContainer.size(); };

	//Here, we declare all the node possible in the behavior tree
	pushNode("SEQUENCE");
	pushNode("SELECTOR");
	pushNode("INVERSER");
	pushNode("CONDITION");
	pushNode("LOOP");
	pushNode("COOLDOWN");
	pushNode("FORCE SUCCESS");
	pushNode("KEEP IN CONE");
	pushNode("WANDER");
	pushNode("MOVE TO");
	pushNode("PLAY ANIMATION");
	pushNode("PLAY SOUND");
	pushNode("ROTATE TO");
	pushNode("WAIT");
	pushNode("ATTACK");
	pushNode("SHOT");
	pushNode("JUMP");
	

	s_decoratorNodeStart = s_NodeContainer["INVERSER"];//Were the decorator node start
	s_actionNodeStart = s_NodeContainer["WANDER"];//Were the action node start and were the decorator node stop (leaf node)

	/*Different condition for the Condition decorator Node*/
	pushCondition("IN_RANGE_OF_PLAYER");
	pushCondition("IS_PLAYER_IN_SIGHT");


	//Here, we declare all the method for the node who need more data (like loop number, cooldown timer, etc...)
	//LOOP
	{
		//Init method set the std::any to the default value
		s_DecoratorInitMethod["LOOP"] = [](PatronNode* node) {
			node->m_decoratorData = 0;
			};
		//Update method display the data and set the std::any to the new value
		s_DecoratorUpdateMethod["LOOP"] = [](PatronNode* node) {
			int loopNumber = std::any_cast<int>(node->m_decoratorData);
			ImGui::InputInt("Loop Number", &loopNumber, 1,100);
			(loopNumber <= 0) ? loopNumber = 1 : loopNumber;
			node->m_decoratorData = loopNumber;
			};
		//Save method save the data in the file
		s_DecoratorSaveMethod["LOOP"] = [](PatronNode* node, std::ofstream& file) {
			int loopNumber = std::any_cast<int>(node->m_decoratorData);
			file << loopNumber << std::endl;
			};
		//Load method load the data from the file
		s_DecoratorLoadMethod["LOOP"] = [](PatronNode* node, std::ifstream& file) {
			int loopNumber;
			file >> loopNumber;
			node->m_decoratorData = loopNumber;
			};
	}

	//COOLDOWN
	{
		s_DecoratorInitMethod["COOLDOWN"] = [](PatronNode* node) {
			node->m_decoratorData = 0.f;
			};
		s_DecoratorUpdateMethod["COOLDOWN"] = [](PatronNode* node) {
			float timer = std::any_cast<float>(node->m_decoratorData);
			ImGui::DragFloat("Timer of cooldown", &timer, 0.1f);
			node->m_decoratorData = timer;
			};
		s_DecoratorSaveMethod["COOLDOWN"] = [](PatronNode* node, std::ofstream& file) {
			float timer = std::any_cast<float>(node->m_decoratorData);
			file << timer << std::endl;
			};
		s_DecoratorLoadMethod["COOLDOWN"] = [](PatronNode* node, std::ifstream& file) {
			float timer;
			file >> timer;
			node->m_decoratorData = timer;
			};
	}

	//CONDITION
	{
		s_DecoratorInitMethod["CONDITION"] = [](PatronNode* node) {
			node->m_conditionType = -1;
			};
		s_DecoratorUpdateMethod["CONDITION"] = [](PatronNode* node) {

			if(ImGui::BeginCombo("Condition Type", PatronNode::getConditionName(node->m_conditionType).c_str()))
			{
				for (auto& i : s_ConditionContainer)
				{
					bool isSelected = false;
					if (ImGui::Selectable(i.first.c_str(), isSelected))
					{
						node->m_conditionType = i.second;
						if (s_DecoratorInitMethod[getConditionName(i.second)])
							s_DecoratorInitMethod[getConditionName(i.second)](node);
					}
				}
				ImGui::EndCombo();
			}

			if (s_DecoratorUpdateMethod[getConditionName(node->m_conditionType)])
				s_DecoratorUpdateMethod[getConditionName(node->m_conditionType)](node);

			};
		s_DecoratorSaveMethod["CONDITION"] = [](PatronNode* node, std::ofstream& file) {
			file << node->m_conditionType << " ";
			if (s_DecoratorSaveMethod[getConditionName(node->m_conditionType)])
				s_DecoratorSaveMethod[getConditionName(node->m_conditionType)](node, file);
			};
		s_DecoratorLoadMethod["CONDITION"] = [](PatronNode* node, std::ifstream& file) {
			file >> node->m_conditionType;
			if (s_DecoratorLoadMethod[getConditionName(node->m_conditionType)])
				s_DecoratorLoadMethod[getConditionName(node->m_conditionType)](node, file);
			};
	}

	//MOVE TO
	{
		s_DecoratorInitMethod["MOVE TO"] = [](PatronNode* node) {
			node->m_decoratorData = std::string();
		};
		s_DecoratorCopyMethod["MOVE TO"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decoratorData = std::any_cast<std::string>(node_copied->m_decoratorData);
		};
		s_DecoratorUpdateMethod["MOVE TO"] = [](PatronNode* node) {
			auto TAG = std::any_cast<std::string>(node->m_decoratorData);
			ImGui::InputText("TAG", TAG, 100);
			node->m_decoratorData = TAG;
		};
		s_DecoratorSaveMethod["MOVE TO"] = [](PatronNode* node, std::ofstream& file) {
			auto tag = std::any_cast<std::string>(node->m_decoratorData);
			file << tag;
		};
		s_DecoratorLoadMethod["MOVE TO"] = [](PatronNode* node, std::ifstream& file){
			std::string tag;
			file >> tag;
			node->m_decoratorData = tag;
		};
	}

	//IN_RANGE_OF_PLAYER
    	{
    		s_DecoratorInitMethod["IN_RANGE_OF_PLAYER"] = [](PatronNode* node) {
    			node->m_decoratorData = 100;
    			};
    		s_DecoratorUpdateMethod["IN_RANGE_OF_PLAYER"] = [](PatronNode* node) {
    			int range = std::any_cast<int>(node->m_decoratorData);
    			ImGui::SliderInt("Range", &range, 100, 1500);
    			node->m_decoratorData = range;
    			};
    		s_DecoratorSaveMethod["IN_RANGE_OF_PLAYER"] = [](PatronNode* node, std::ofstream& file) {
    			int range = std::any_cast<int>(node->m_decoratorData);
    			file << range;
    
    			};
    		s_DecoratorLoadMethod["IN_RANGE_OF_PLAYER"] = [](PatronNode* node, std::ifstream& file) {
    			int range;
    			file >> range;
    			node->m_decoratorData = range;
    			};
    	}

	//JUMP
	{
		s_DecoratorInitMethod["JUMP"] = [](PatronNode* node) {
			node->m_decoratorData = 100.f;
			};
		s_DecoratorUpdateMethod["JUMP"] = [](PatronNode* node) {
			auto jump_height = std::any_cast<float>(node->m_decoratorData);
			ImGui::DragFloat("Jump Height", &jump_height);
			node->m_decoratorData = jump_height;
			};
		s_DecoratorSaveMethod["JUMP"] = [](PatronNode* node, std::ofstream& file) {
			auto jump_height = std::any_cast<float>(node->m_decoratorData);
			file << jump_height;

			};
		s_DecoratorLoadMethod["JUMP"] = [](PatronNode* node, std::ifstream& file) {
			float jump_height;
			file >> jump_height;
			node->m_decoratorData = jump_height;
			};
	}


	//PLAY_ANIMATION
	{
		s_DecoratorInitMethod["PLAY ANIMATION"] = [](PatronNode* node) {
			node->m_decoratorData = std::tuple<std::weak_ptr<lc::Animation>,std::string>();
			};
		s_DecoratorCopyMethod["PLAY ANIMATION"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decoratorData = std::tuple<std::weak_ptr<lc::Animation>,std::string>();
			};
		s_DecoratorUpdateMethod["PLAY ANIMATION"] = [](PatronNode* node) {			
			auto tuple = std::any_cast<std::tuple<std::weak_ptr<lc::Animation>,std::string>>(node->m_decoratorData);
			if(!node->m_game_object_.expired())
			{
				auto object = node->m_game_object_.lock();
				if(ImGui::BeginCombo("Animation Chosen", std::get<0>(tuple).expired() ? "Select Animation" : std::get<0>(tuple).lock()->getName().c_str()))
				{
					bool is_selected = false;
					for (const auto& component : object->getComponents())
					{
						if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
						{
							if(ImGui::Selectable(anim->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<0>(tuple) = anim;
							}
						}					
					}
					ImGui::EndCombo();
				}

				if(!std::get<0>(tuple).expired())
				{
					

					
				}
				
				
				
			}



			
			ImGui::InputText("TAG", TAG, 100);
			node->m_decoratorData = TAG;
			};
		s_DecoratorSaveMethod["PLAY ANIMATION"] = [](PatronNode* node, std::ofstream& file) {
			auto tag = std::any_cast<std::string>(node->m_decoratorData);
			file << tag;

			};
		s_DecoratorLoadMethod["PLAY ANIMATION"] = [](PatronNode* node, std::ifstream& file) {
			std::string tag;
			file >> tag;
			node->m_decoratorData = tag;
			};
	}

	//PLAY_SOUND
	{
		s_DecoratorInitMethod["PLAY SOUND"] = [](PatronNode* node) {
			node->m_decoratorData = std::tuple<std::string,bool,float,float>();
			};
		s_DecoratorCopyMethod["PLAY SOUND"] = [](PatronNode* node, PatronNode* node_copied) {
				node->m_decoratorData = std::any_cast<std::tuple<std::string,bool,float,float>>(node_copied->m_decoratorData);
				};
		s_DecoratorUpdateMethod["PLAY SOUND"] = [](PatronNode* node) {
			auto tuple = std::any_cast<std::tuple<std::string,bool,float,float>>(node->m_decoratorData);
			ImGui::InputText("Sound Name", std::get<0>(tuple), 100);
			ImGui::Checkbox("Start New Sound", &std::get<1>(tuple));
			ImGui::DragFloat("Attenuation", &std::get<2>(tuple));
			ImGui::DragFloat("Minimum Distance", &std::get<3>(tuple));
			node->m_decoratorData = tuple;
			};
		s_DecoratorSaveMethod["PLAY SOUND"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<std::tuple<std::string,bool,float,float>>(node->m_decoratorData);
			file << std::get<0>(tuple) << " " << std::get<1>(tuple) << " " << std::get<2>(tuple) << " " << std::get<3>(tuple);

			};
		s_DecoratorLoadMethod["PLAY SOUND"] = [](PatronNode* node, std::ifstream& file) {
			std::tuple<std::string,bool,float,float> tuple;
			file >> std::get<0>(tuple) >> std::get<1>(tuple) >> std::get<2>(tuple) >> std::get<3>(tuple);
			node->m_decoratorData = tuple;
			};
	}

	//ROTATE_TO
	{
		s_DecoratorInitMethod["ROTATE TO"] = [](PatronNode* node) {
			node->m_decoratorData = std::string();
			};
		s_DecoratorCopyMethod["ROTATE TO"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decoratorData = std::any_cast<std::string>(node_copied->m_decoratorData);
			};
		s_DecoratorUpdateMethod["ROTATE TO"] = [](PatronNode* node) {
			auto TAG = std::any_cast<std::string>(node->m_decoratorData);
			ImGui::InputText("TAG", TAG, 100);
			node->m_decoratorData = TAG;
			};
		s_DecoratorSaveMethod["ROTATE TO"] = [](PatronNode* node, std::ofstream& file) {
			auto tag = std::any_cast<std::string>(node->m_decoratorData);
			file << tag;

			};
		s_DecoratorLoadMethod["ROTATE TO"] = [](PatronNode* node, std::ifstream& file) {
			std::string tag;
			file >> tag;
			node->m_decoratorData = tag;
			};
	}

	//WAIT
	{
		s_DecoratorInitMethod["WAIT"] = [](PatronNode* node) {
			node->m_decoratorData = float(0.f);
			};
		s_DecoratorUpdateMethod["WAIT"] = [](PatronNode* node) {
			float timer = std::any_cast<float>(node->m_decoratorData);
			ImGui::DragFloat("Timer of wait", &timer, 0.1f);
			node->m_decoratorData = timer;
			};
		s_DecoratorSaveMethod["WAIT"] = [](PatronNode* node, std::ofstream& file) {
			float timer = std::any_cast<float>(node->m_decoratorData);
			file << timer << std::endl;
			};
		s_DecoratorLoadMethod["WAIT"] = [](PatronNode* node, std::ifstream& file) {
			float timer;
			file >> timer;
			node->m_decoratorData = timer;
			};
	}

	//ATTACK
	{
		s_DecoratorInitMethod["ATTACK"] = [](PatronNode* node) {
			node->m_decoratorData = std::string();
		};
		s_DecoratorCopyMethod["ATTACK"] = [](PatronNode* node, PatronNode* nodeCopied){
			node->m_decoratorData = std::any_cast<std::string>(nodeCopied->m_decoratorData);
		};
		s_DecoratorUpdateMethod["ATTACK"] = [](PatronNode* node) {
			std::string attack_name = std::any_cast<std::string>(node->m_decoratorData);
			ImGui::InputText("Attack Name", attack_name, 150);
			node->m_decoratorData = attack_name;
		};
		s_DecoratorSaveMethod["ATTACK"] = [](PatronNode* node, std::ofstream& file) {
			std::string	attack_name = std::any_cast<std::string>(node->m_decoratorData);
			file << attack_name << std::endl;
		};
		s_DecoratorLoadMethod["ATTACK"] = [](PatronNode* node, std::ifstream& file) {
			std::string attack_name;
			file >> attack_name;
			node->m_decoratorData = attack_name;
		};
	}

	//SHOT
	{
		s_DecoratorInitMethod["SHOT"] = [](PatronNode* node) {
			node->m_decoratorData = std::string();
		};
		s_DecoratorCopyMethod["SHOT"] = [](PatronNode* node, PatronNode* nodeCopied){
			node->m_decoratorData = std::any_cast<std::string>(nodeCopied->m_decoratorData);
		};
		s_DecoratorUpdateMethod["SHOT"] = [](PatronNode* node) {
			std::string attack_name = std::any_cast<std::string>(node->m_decoratorData);
			ImGui::InputText("Attack Name", attack_name, 150);
			node->m_decoratorData = attack_name;
		};
		s_DecoratorSaveMethod["SHOT"] = [](PatronNode* node, std::ofstream& file) {
			std::string	attack_name = std::any_cast<std::string>(node->m_decoratorData);
			file << attack_name << std::endl;
		};
		s_DecoratorLoadMethod["SHOT"] = [](PatronNode* node, std::ifstream& file) {
			std::string attack_name;
			file >> attack_name;
			node->m_decoratorData = attack_name;
		};
	}
}

std::string PatronNode::getNodeName(int type)
{
	 for (auto& i : s_NodeContainer)
	 { 
		 if (i.second == type)
			 return i.first; 
	 } return "NONE";
}

std::string PatronNode::getConditionName(int type)
{
	for (auto& i : s_ConditionContainer)
	{
		if (i.second == type)
			return i.first;
	} return "NONE";
}

PatronNode PatronNode::Clone()
{
	return  PatronNode(*this);
}

PatronNode::PatronNode(int type) : m_type(type), m_id(s_id++), m_parent(nullptr)
{
	m_conditionType = -1;
	if (s_DecoratorInitMethod[getNodeName(m_type)])
		s_DecoratorInitMethod[getNodeName(m_type)](this);
	
}

PatronNode::PatronNode(const PatronNode& node)
{
	m_type = node.m_type;
	m_conditionType = node.m_conditionType;
	m_decoratorData = node.m_decoratorData;
	m_id = s_id++;
	m_isOpen = node.m_isOpen;
	m_newNodeIsAdded = node.m_newNodeIsAdded;
	m_parent = nullptr;
	m_child.clear();
	for (auto& i : node.m_child)
	{
		Add(*i);
	}

	if (s_DecoratorCopyMethod[getNodeName(m_type)])
		s_DecoratorCopyMethod[getNodeName(m_type)](this, const_cast<PatronNode*>(&node));
}

PatronNode::PatronNode(PatronNode&& node)
{
	m_type = node.m_type;
	m_conditionType = node.m_conditionType;
	m_decoratorData = node.m_decoratorData;
	m_id = s_id++;
	m_isOpen = node.m_isOpen;
	m_newNodeIsAdded = node.m_newNodeIsAdded;
	m_parent = nullptr;
	m_child.clear();
	for (auto& i : node.m_child)
	{
		Add(*i);
	}

	if (s_DecoratorCopyMethod[getNodeName(m_type)])
		s_DecoratorCopyMethod[getNodeName(m_type)](this, const_cast<PatronNode*>(&node));
}

PatronNode* PatronNode::Add(const PatronNode& node)
{

	if (node.m_type >= 0 and this->m_type < s_actionNodeStart)
	{
		m_child.push_back(new PatronNode(node));
		m_child.back()->m_parent = this;
		return m_child.back();
	}
	else
	{
		std::cout << "Invalid Node Type" << std::endl;
		return this;
	}
}

PatronNode* PatronNode::Add(PatronNode* node)
{
	if (node->m_type >= 0 and this->m_type < s_actionNodeStart)
	{
		m_child.push_back(node);
		m_child.back()->m_parent = this;
		return m_child.back();
	}
	else
	{
		std::cout << "Invalid Node Type" << std::endl;
		return this;
	}
}


void PatronNode::removeChild(PatronNode* node)
{
	m_child.remove_if([&](PatronNode*& node_) {return node_ == node; });	
}

void PatronNode::removeChild(int node_id)
{
	m_child.remove_if([&](PatronNode*& node_) {return node_->m_id == node_id; });
}



PatronNode* PatronNode::CheckIfNodeIsAChild(int id)
{
	for (auto& i : m_child)
	{
		if (i->m_id == id)
			return i;
	}

	for (auto& i : m_child)
	{
		auto tmp = i->CheckIfNodeIsAChild(id);
		if (tmp)
			return tmp;
	}

	return nullptr;
}

void PatronNode::Save(std::ofstream& save)
{
	save << m_type << " " << static_cast<int>(m_child.size());
	if(s_DecoratorSaveMethod[getNodeName(m_type)])
	{
		save << " ";
		s_DecoratorSaveMethod[getNodeName(m_type)](this, save);
	}
	save << std::endl;
	for (auto& i : m_child)
	{
		i->Save(save);
	}
}

void PatronNode::Load(std::ifstream& load)
{
	int size;
	load >> m_type >> size;
	if (s_DecoratorLoadMethod[getNodeName(m_type)])
		s_DecoratorLoadMethod[getNodeName(m_type)](this, load);
	for (int i = 0; i < size; i++)
	{
		PatronNode tmp(0);
		tmp.Load(load);
		m_child.push_back(new PatronNode(tmp));
		m_child.back()->m_parent = this;
	}
}

ImRect PatronNode::Display(PatronNode** selectedNode, std::weak_ptr<lc::GameObject> game_object,int executionOrder)
{
	if(m_game_object_.expired())
	{
		m_game_object_ = game_object;		
	}
	if (*selectedNode == this)//If the node is selected, change the color of the node
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31 / 255.f, 57 / 255.f, 88.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31 / 255.f, 57 / 255.f, 88.f / 255.f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.f));
	}


	if (ImGui::TreeNodeEx((PatronNode::getNodeName(m_type) + (executionOrder ? " " + std::to_string(executionOrder) : "") + "## " + std::to_string(m_id)).c_str(),
		ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth  |ImGuiTreeNodeFlags_OpenOnArrow))
	{
		m_isOpen = true;
	} else m_isOpen = false;

	if(m_newNodeIsAdded)//Automatically open the tree if a new node is added
	{
		ImGui::TreeNodeSetOpen(ImGui::GetID((PatronNode::getNodeName(m_type) + (executionOrder ? " " + std::to_string(executionOrder) : "") + "## " + std::to_string(m_id)).c_str()), true);
		m_newNodeIsAdded = false;
	}

	const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		*selectedNode = this;
	}

	if(executionOrder != 0)//If the node is not the root node, the user can drag and drop the node
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			auto tmp = this;
			ImGui::SetDragDropPayload("TREE_NODE", &tmp, sizeof(this));
			ImGui::Text(PatronNode::getNodeName(m_type).c_str());
			ImGui::EndDragDropSource();
		}

		
	}
	if (m_type < s_decoratorNodeStart or (m_type >= PatronNode::getDecoratorNodeStart() and m_type < PatronNode::getActionNodeStart() and getChildrens().empty())) //if the node if not a leaf node or the decorator node is empty, we can drop the node dragged into it
		if (BeginDragDropTarget())
		{
			if (auto tmp_playLoad = ImGui::AcceptDragDropPayload("TREE_NODE"))
				if (auto node = reinterpret_cast<PatronNode**>(tmp_playLoad->Data))
				{
					auto parent = (*node)->m_parent;
					if (this->CheckIfNodeIsAChild(m_id))
					{
						if(m_isOpen)
						{
							ImGui::TreePop();
						}
						PopStyleColor(2);
						return nodeRect;

					}

					if(this != parent)
					{
						parent->removeChild((*node)->m_id);
						this->Add((*node))->setNewNodeIsAdded(true);
						this->setNewNodeIsAdded(true);
						*selectedNode = this;
					}
				}
			ImGui::EndDragDropTarget();
		}


	if (m_isOpen)
	{
		//Used to display the line between the parent and the child node
		const ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
		const float SmallOffsetX = -10.0f; //for now, a hardcoded value; should take into account tree indent size
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
		verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
		ImVec2 verticalLineEnd = verticalLineStart;

		int index = 1;
		for (auto child : m_child)
		{
			const float HorizontalTreeLineSize = 10.0f; //chosen arbitrarily
			const ImRect childRect = child->Display(selectedNode, game_object,index);
			const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f - 1.f;
			drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
			verticalLineEnd.y = midpoint;
			index++;
		}

		drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);


		

		ImGui::TreePop();
	}
	PopStyleColor(2);

	return nodeRect;
}
