#pragma once
#include "Ressource.h"
#include "Texture.h"
#include "BehaviorTree.h" 

namespace lc
{
	class AI : public Ressource
	{
	public:
		AI();
		~AI();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;
	private:
		std::string m_aiName;
		PatronNode m_root;
		PatronNode* m_selectedNode = nullptr;
		std::pair<std::weak_ptr<GameObject>, PatronNode*> m_copiedNode = {};
		int m_newNodeType = 0;
		int m_newDecorateType = PatronNode::getDecoratorNodeStart();
		bool m_isEditionBehaviorTree = false;
		bool m_wantToCopyATreeNode = false;

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc();

		/**
		 * @brief Update the selected node in the behavior tree
		 * 
		 */
		void UpdateSelectedNode();

		/**
		 * @brief Add a node to the selected node in the behavior tree
		 * 
		 * \param type Type of the node to add
		 */
		void AddNode(int type);

		/**
		* @brief Remove the selected node in the behavior tree	
		*/
		void RemoveNode();
		/**
		 * @brief Add a decorate node to the selected node in the behavior tree
		 * 
		 * \param type Type of the node to add
		 */
		void AddDecorateNode(int type);
		/**
		* @brief Remove the decorate node in the behavior tree
		*/
		void RemoveDecorateNode();
		/**
		* @brief Copy the selected TreeNode in this TreeNode
		* 
		*/
		void CopyTreeNode(std::shared_ptr<lc::GameObject> _game_object);
		/**
		 * @brief Old function create to make a combo box in the imgui window
		 */
		int MakeCombo(int start, int end, int defaultValue ,std::string name);
	};
}