
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


bt::ActionNode::MoveTo::MoveTo(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_, const float& speed_) :m_agent_(agent_), m_target_(target_), m_speed_(speed_), m_bool_go_to_(false)
{	
}

void bt::ActionNode::MoveTo::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rb = agent->getComponent<lc::RigidBody>("RigidBody");

		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
			{
				auto node_cast = std::dynamic_pointer_cast<MoveTo>(node);
				if (node_cast->m_bool_go_to_)
				{
					rigid_body->getVelocity() = Tools::Vector::normalize(node_cast->m_target_.lock()->getTransform().getPosition() - node_cast->m_agent_.lock()->getTransform().getPosition()) * node_cast->m_speed_;
					node_cast->m_bool_go_to_ = false;
				}
			});
	}
}


bool bt::ActionNode::MoveTo::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_go_to_ = true;
	}
	return m_bool_go_to_;
}

bt::ActionNode::Wander::Wander(const std::shared_ptr<lc::GameObject>& agent_) : m_agent_(agent_), m_bool_wander_(false), m_bool_direction_(false), m_speed_(100.f)
{	
}

void bt::ActionNode::Wander::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rb = agent->getComponent<lc::RigidBody>("RigidBody");
		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
		{
			auto node_cast = std::dynamic_pointer_cast<Wander>(node);
			if (node_cast->m_bool_wander_)
			{
				rigid_body->getVelocity().x = (node_cast->m_bool_direction_ ? +1 : -1) * node_cast->m_speed_;
				node_cast->m_bool_wander_ = false;

				
				auto scene = node_cast->m_agent_.lock()->getParent();
				while (scene->getParent())
				{
					scene = scene->getParent();
				}
				std::function<void(std::shared_ptr<lc::GameObject>&)> CheckCollision = [&](std::shared_ptr<lc::GameObject>& game_object)
				{
					if (game_object->hasComponent("RigidBody") and game_object != rigid_body->getParent())
					{
						auto rb = game_object->getComponent<lc::RigidBody>("RigidBody");
						auto point = (node_cast->m_bool_direction_ ? rigid_body->getCollider().getPosition() + rigid_body->getCollider().getSize() : rigid_body->getCollider().getPosition() + sf::Vector2f(0, rigid_body->getCollider().getSize().y)) + sf::Vector2f(0, 1);
						auto rect = rb->getCollider();
						if (!Tools::Collisions::point_rect(point,rect))
						{
							node_cast->m_bool_direction_ = !node_cast->m_bool_direction_;
							return;
						}
					}
					for (auto& i : game_object->getObjects())
					{
						CheckCollision(i);
					}
				};
				CheckCollision(scene);
				std::cout << (node_cast->m_bool_direction_ ? "right" : "left") << std::endl;
			}
		});
	}
}

bool bt::ActionNode::Wander::tick()
{
	if(!m_agent_.expired())
	{
		m_bool_wander_ = true;
	}
	return m_bool_wander_;

}

bt::NodePtr bt::Factory(const node_type& type)
{
	switch (type) {
	case node_type::SEQUENCE:
		return Node::New(Composite::Sequence());
		break;
	case node_type::SELECTOR:
		return Node::New(Composite::Selector());
		break;
	case node_type::INVERSER:
		return Node::New(Decorator::Inverser());
		break;
	case node_type::CONDITION:
		return Node::New(Decorator::Condition());
		break;
	case node_type::LOOP:
		return Node::New(Decorator::Loop());
		break;
	case node_type::COOLDOWN:
		return Node::New(Decorator::Cooldown());
		break;
	case node_type::FORCE_SUCCESS:
		return Node::New(Decorator::ForceSuccess());
		break;
	case node_type::KEEP_IN_CONE:
		break;
	case node_type::WANDER:
		return Node::New(ActionNode::Wander());
		break;
	case node_type::MOVE_TO:
		return Node::New(ActionNode::MoveTo());
		break;
	case node_type::PLAY_ANIMATION:
		break;
	case node_type::PLAY_SOUND:
		break;
	case node_type::ROTATE_TO:
		break;
	case node_type::WAIT:
		break;
	default:
		return Node::New(Composite::Sequence());
	}
	
}
