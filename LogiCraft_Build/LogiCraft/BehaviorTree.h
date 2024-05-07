
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

#pragma once
#include "tools_imgui.h" 
#include <any>
#include <functional> 

class PatronNode;

typedef std::unordered_map<std::string, std::function<void(PatronNode*, std::ofstream& file)>> DecoratorSaveMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*, std::ifstream& file)>> DecoratorLoadMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*)>> DecoratorMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*, PatronNode*)>> DecoratorCopyMethod;

/*
 * @brief Class that represent a node of a behavior tree
*/


class PatronNode
{
	int m_type;
	int m_conditionType;
	std::list <PatronNode*> m_child;
	PatronNode* m_parent;
	int m_id;
	bool m_isOpen = false;
	bool m_newNodeIsAdded = false;
	std::any m_decoratorData;
	std::weak_ptr<lc::GameObject> m_game_object_;

	static DecoratorSaveMethod s_DecoratorSaveMethod;
	static DecoratorLoadMethod s_DecoratorLoadMethod;
	static DecoratorMethod s_DecoratorInitMethod;
	static DecoratorMethod s_DecoratorUpdateMethod;
	static DecoratorCopyMethod s_DecoratorCopyMethod;
	static std::unordered_map<std::string, int> s_ConditionContainer;
	static std::unordered_map<std::string, int> s_NodeContainer;
	inline static int s_id = 0;
	inline static int s_actionNodeStart = 0;
	inline static int s_decoratorNodeStart = 0;	
public:
	/**
	 * @brief Static method, MUST BE CALLED AT PROGRAM INIT
	 * Setup all the node identification and their method
	 * 
	 */
	static void SetupAllNode();

	/**
	 * @brief Static method, return the name of the node
	 * 
	 * \param type Type of the node asociated with the name
	 * \return the node name
	 */
	static std::string getNodeName(int type);

	/**
	*@brief Static method, return the name of the condition
	* 
	* \param type Type of the condition associed with the name
	* \return the condition name
	*/
	static std::string getConditionName(int type);

	/**
	 * @brief Static method, return the type of the node
	 * 
	 * \param name Name of the node asociated with the type
	 * \return the node type
	 */
	static int getNodeType(std::string name) { return s_NodeContainer[name]; }

	/**
	*@brief Static method, return the container of all the nodes
	* 
	*/
	static const std::unordered_map< std::string, int>& getNodeContainer() { return s_NodeContainer; } ;

	/**
	 * @brief Static method, return the container of all the decorator update method
	 */
	static const DecoratorMethod& getNodeUpdateMethod() { return s_DecoratorUpdateMethod; };


	static int getActionNodeStart() { return s_actionNodeStart; }
	static int getDecoratorNodeStart() { return s_decoratorNodeStart; }

	/**
	 * @brief Clone a node and all its children
	 * 
	 * \return 
	 */
	PatronNode Clone();


	PatronNode(int type);
	PatronNode(const PatronNode& node);
	PatronNode(PatronNode&& node);

	PatronNode& operator=(const PatronNode& node) = default;


	/**
	 * @brief Create a new node and add it to the current node
	 * 
	 * \param node Node to add
	 * \return Node added
	 */
	PatronNode* Add(const PatronNode& node);
	/**
	 * @brief Add a existing node to the current node
	 * 
	 * \param node Node to add
	 * \return Node added
	 */
	PatronNode* Add(PatronNode* node);

	int getType() { return m_type; }

	std::list<PatronNode*>& getChildrens() { return m_child; }

	PatronNode* getChild(int index) { return *std::next(m_child.begin(), index); }

	PatronNode* getParent() { return m_parent; }

	void SetParent(PatronNode* parent) { m_parent = parent; }

	void removeChild(PatronNode* node);

	void removeChild(int node_id);

	/**
	 * @brief Set the node as open
	 * 
	 * \param isAdded true if you want to open the tree
	 */
	void setNewNodeIsAdded(bool isAdded) { m_newNodeIsAdded = isAdded; }

	/**
	 * @brief Return if the node is a child of the current node
	 * 
	 * \param id Id of the node to check
	 * \return nullptr if the node does not exist, the node if it exist
	 */
	PatronNode* CheckIfNodeIsAChild(int id);

	void Save(std::ofstream& save);
	void Load(std::ifstream& load);

	/**
	 * @brief Display the node and all its children using ImGUI TreeNode and window.
	 * This function must be called inside a ImGUI window
	 * 
	 * \param selectedNode Node selected by the user in the tree
	 * \param executionOrder Execution order of the node, 0 mean its the root node
	 * \return 
	 */
	ImRect Display(PatronNode** selectedNode, std::weak_ptr<lc::GameObject> game_object,int executionOrder = 0);
};
