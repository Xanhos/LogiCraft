
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

#include <list>
#include <memory>
#include <functional>
#include "GameObject.h"
#include "RigidBody.h"

#define SECURE_TASK(task) (task ? task->tick() : false)
/*	pushNode("SEQUENCE");
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
pushNode("WAIT");*/


namespace bt
{
	enum class node_type{
		SEQUENCE,
		SELECTOR,
		INVERSER,
		CONDITION,
		LOOP,
		COOLDOWN,
		FORCE_SUCCESS,
		KEEP_IN_CONE,
		WANDER,
		MOVE_TO,
		PLAY_ANIMATION,
		PLAY_SOUND,
		ROTATE_TO,
		WAIT,
		ATTACK,
		SHOT,
		JUMP
	};



	class Node
	{
		inline static unsigned int m_idCounter = 0;
	protected:
		unsigned int m_id;
		Node() : m_id(m_idCounter++) {}
		virtual ~Node() = default;
		std::weak_ptr<Node> m_parent;
		
		template<typename T>
		std::shared_ptr<T> getNode() { std::make_shared<T>(*this); }
	public:
		virtual bool tick() = 0;

		unsigned int getID() const { return m_id; }

		template<typename T>
		static std::shared_ptr<T> New(const T& node) { return std::make_shared<T>(node); }
	};

	typedef std::shared_ptr<Node> NodePtr;
	typedef std::list<std::shared_ptr<Node>> NodeList;


	NodePtr Factory(const node_type& type);

#pragma region COMPOSITE
	namespace Composite
	{
		
		class CompositeNode : public Node
		{
		protected:
			NodeList m_childList;
			std::weak_ptr<Node> m_wait_node_;
		public:
			CompositeNode() :m_childList() ,Node() {}

			NodeList& getChilds() { return m_childList; }

			NodePtr getChild(int id)
			{
				for(auto& child : m_childList)
					if(child->getID() == id)
						return child;

				auto tmp = NodePtr();
				for(auto& child : m_childList)
					if(auto composite = std::dynamic_pointer_cast<CompositeNode>(child))
					{
						tmp = composite->getChild(id);
						if(tmp)
							return tmp;
					}

				return tmp;
			}
			
			template <typename T>
			std::shared_ptr<T> addChild(std::shared_ptr<T> child) {
				m_childList.push_back(child);
				return std::dynamic_pointer_cast<T>(m_childList.back()); }

			std::weak_ptr<Node>& GetWaitingNode() { return m_wait_node_; }
		};

		class Selector : public CompositeNode
		{
		public:
			Selector() : CompositeNode() {}
			virtual bool tick();

		};

		class Sequence : public CompositeNode
		{
		public:
			Sequence() : CompositeNode() {}
			bool tick() override;
		};
	}


#pragma endregion

#pragma region DECORATOR

	namespace Decorator
	{
		class Decorator : public Node
		{
		protected:
			NodePtr m_task;
		public:
			Decorator() { m_task = NodePtr(); }
			Decorator(NodePtr task) { m_task = task; }

			NodePtr setTask(NodePtr task) { m_task = task; return m_task; }
			NodePtr& getTask() {return m_task;}
		};

		class Inverser : public Decorator
		{
		public:
			Inverser() : Decorator() {}
			Inverser(NodePtr task) : Decorator(task) {}
			virtual bool tick() { return  SECURE_TASK(m_task); }
		};

		class Loop : public Decorator
		{
			unsigned int m_loopNbr;
		public:
			Loop() : Decorator() { m_loopNbr = 1u; }
			Loop(NodePtr task, unsigned int loop) : Decorator(task) { m_loopNbr = loop; }

			void setLoop(unsigned int loop) { m_loopNbr = loop; }

			virtual bool tick() {
				if (m_task)
				{
					if (m_loopNbr)
						for (unsigned int i = 0; i < m_loopNbr; i++)
						{
							if (m_task->tick())
								return true;
						}
					else while (!m_task->tick()) {};

					return false;
				}
			}
		};

		class Cooldown : public Decorator
		{
			float m_executionTimer;
			float m_timer;
		public:
			Cooldown() : Decorator() { m_timer = 0.f; }
			Cooldown(NodePtr task, float timer) : Decorator(task) { m_timer = 0.f; m_executionTimer = timer; }

			void setTimer(float timer) { m_executionTimer = timer; }
			virtual bool tick() {
				if (m_timer > m_executionTimer)
				{
					m_timer = 0.f;
					return SECURE_TASK(m_task);
				}
				m_timer += Tools::getDeltaTime();
				return false;
			}
		};

		class ForceSuccess : public Decorator
		{
			public:
			ForceSuccess() : Decorator() {}
			ForceSuccess(NodePtr task) : Decorator(task) {}
			virtual bool tick() {SECURE_TASK(m_task) ; return true; }
		};

		class Condition : public Decorator
		{
			std::function<bool()> m_condition;
		public:
			Condition() : Decorator() {}
			Condition(NodePtr task, std::function<bool()> condition) : Decorator(task) { m_condition = condition; }

			void setCondition( std::function<bool()> condition) {m_condition = condition;}
			virtual bool tick() {
				if (m_condition)
					if (m_condition())
						return SECURE_TASK(m_task);
				return false;
			}
		};

	}

#pragma endregion

	namespace ActionNode
	{
		

		class NodeFunc : public Node
		{
			std::function<bool()> m_tick;
		public:
			NodeFunc(std::function<bool()> tick) { m_tick = tick; }
			virtual bool tick() { return (m_tick ? m_tick() : false); }
		};

		class MoveTo : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			float m_speed_;
			bool m_bool_go_to_;
		public:
			MoveTo() : m_agent_(), m_target_(), m_speed_(0.f), m_bool_go_to_(false) {}
			MoveTo(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_, const float& speed_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class DamageBox : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			DamageBox() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			DamageBox(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class CrazyHit : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			CrazyHit() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			CrazyHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class LanceHit : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			LanceHit() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			LanceHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class PickHit : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			PickHit() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			PickHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class Hit : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			Hit() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			Hit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};


		class lanceSpawn : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			std::weak_ptr<lc::GameObject> m_target_;
			bool m_bool_damage_;
		public:
			lanceSpawn() : m_agent_(), m_target_(), m_bool_damage_(false) {}
			lanceSpawn(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class TestController : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			bool m_bool_controller_;
		public:
			TestController() : m_agent_(), m_bool_controller_(false) {}
			TestController(const std::shared_ptr<lc::GameObject>& agent_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class Wander : public Node
		{
			std::weak_ptr<lc::GameObject> m_agent_;
			bool m_bool_wander_;
			bool m_bool_direction_; //false mean left, true mean right;
			float m_speed_;
		public:
			Wander() : m_agent_(), m_bool_wander_(false), m_bool_direction_(false), m_speed_(0.f) {}
			Wander(const std::shared_ptr<lc::GameObject>& agent_);
			void Setup(NodePtr node);
			bool tick() override;
		};

		class Wait : public Node
		{
			float m_timer_;
			float m_time_;
			std::weak_ptr<Node> m_root_;
			std::weak_ptr<Node> m_node_;
			std::weak_ptr<Node> m_parent_;
		public:
			Wait() : Node(), m_timer_(0.f), m_time_(0.f) {}
			void Setup(NodePtr node, NodePtr parent, NodePtr root);
			void setTimer(float timer) { m_time_ = timer; }
			bool tick();

			std::weak_ptr<Node> getParent() {return m_parent_;};
		};

		class Play_Sound : public Node
		{
			std::string m_sound_name_;
			bool m_start_new_sound_;
			inline static unsigned int s_general_sound_id_ = 50000u;
			unsigned int m_sound_id_;
			std::weak_ptr<lc::GameObject> m_owner;
			float m_attenuation_, m_min_distance_;
		public:
			Play_Sound() : m_start_new_sound_(false), m_sound_id_(s_general_sound_id_++), m_attenuation_(0.f), m_min_distance_(0.f){}
			Play_Sound(std::string sound, bool new_sound, std::weak_ptr<lc::GameObject> owner, float attenuation = 0.f, float min_distance = 0.f);
			void Setup(NodePtr node);
			bool tick();
		};

		class Jump : public Node
		{
			std::weak_ptr<lc::GameObject> m_owner_;
			float m_jump_height_;
			bool m_need_to_jump_;
		public:
			Jump() : m_jump_height_(0.f), m_need_to_jump_(0.f){}
			Jump(float jump_height, std::weak_ptr<lc::GameObject> owner);
			void Setup(NodePtr node);
			bool tick();
			
		};
	}


}

