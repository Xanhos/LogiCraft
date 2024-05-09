
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
#include "SFML_ENGINE/Tools.h"

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
					if (!node_cast->m_target_.expired())
					{
						rigid_body->getVelocity().x = Tools::Vector::normalize(node_cast->m_target_.lock()->getTransform().getPosition() - node_cast->m_agent_.lock()->getTransform().getPosition()).x * node_cast->m_speed_;
					}
					node_cast->m_bool_go_to_ = false;
				}
				else rigid_body->getVelocity().x = 0.f;
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
				bool is_on_a_platform = rigid_body->getIsFlying();
				bool has_hit_a_wall = false;
				std::function<bool(std::shared_ptr<lc::GameObject>&)> CheckSideCollision = [&](std::shared_ptr<lc::GameObject>& game_object) ->bool
					{
						if (game_object->hasComponent("RigidBody") and game_object != rigid_body->getParent())
						{
							auto rb = game_object->getComponent<lc::RigidBody>("RigidBody");
							auto rect = rb->getCollider();
							auto side_collision = !node_cast->m_bool_direction_ ?
								sf::FloatRect{ {rigid_body->getCollider().getPosition()}, {rigid_body->getCollider().getPosition() + sf::Vector2f{0,rigid_body->getCollider().height}} } :
								sf::FloatRect{ {rigid_body->getCollider().getPosition() + sf::Vector2f{rigid_body->getCollider().width,0.f}},{rigid_body->getCollider().getPosition() + rigid_body->getCollider().getSize()} };
							side_collision.left += rigid_body->getVelocity().x * Tools::getDeltaTime();
							side_collision.width += rigid_body->getVelocity().x * Tools::getDeltaTime();

							side_collision.height += -0.1f;
							if (Tools::Collisions::lineRect(side_collision, rect))
							{
								has_hit_a_wall = true;
								return true;
							}
						}
						for (auto& i : game_object->getObjects())
						{
							if (CheckSideCollision(i))
								return true;
						}
						return false;
					};
				std::function<bool(std::shared_ptr<lc::GameObject>&)>  CheckIsOnAPlatform = [&](std::shared_ptr<lc::GameObject>& game_object)
					{
						if (game_object->hasComponent("RigidBody") and game_object != rigid_body->getParent())
						{
							auto rb = game_object->getComponent<lc::RigidBody>("RigidBody");
							auto point = (node_cast->m_bool_direction_ ?
								rigid_body->getCollider().getPosition() + rigid_body->getCollider().getSize() :
								rigid_body->getCollider().getPosition() + sf::Vector2f(0, rigid_body->getCollider().getSize().y))
								+ sf::Vector2f(0, rigid_body->getVelocity().y * Tools::getDeltaTime());

							auto rect = rb->getCollider();
							if (Tools::Collisions::point_rect(point, rect))
							{
								is_on_a_platform = true;
								return true;
							}
						}
						for (auto& i : game_object->getObjects())
						{
							if (CheckIsOnAPlatform(i))
								return true;
						}
						return false;
					};
				auto CheckCollision = [&](std::shared_ptr<lc::GameObject>& object)
					{
						for (auto& i : object->getObjects())
						{
							if (i->hasComponent("RigidBody"))
							{
								CheckSideCollision(i);
								CheckIsOnAPlatform(i);
							}
						}

					};
				CheckCollision(scene);
				if (!is_on_a_platform or has_hit_a_wall)
				{
					node_cast->m_bool_direction_ = !node_cast->m_bool_direction_;
				}
			}
			else rigid_body->getVelocity().x = 0.f;

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
		return Node::New(ActionNode::Play_Animation());
		break;
	case node_type::PLAY_SOUND:
		return Node::New(ActionNode::Play_Sound());
		break;
	case node_type::ROTATE_TO:
		break;
	case node_type::WAIT:
		return Node::New(ActionNode::Wait());
		break;
	default:
		return Node::New(Composite::Sequence());
	}
	
}

bool bt::Composite::Selector::tick()
{	
	if(m_wait_node_.lock())
	{
		auto parent = std::dynamic_pointer_cast<ActionNode::Wait>(m_wait_node_.lock())->getParent();
		if(!m_wait_node_.lock()->tick())
			return false;
		bool next_is_found = false;
		for(auto& i : std::dynamic_pointer_cast<CompositeNode>(parent.lock())->getChilds())
		{
			if(!next_is_found)
				if(m_id == i->getID())
				{
					next_is_found = true;
					continue;
				}
			if(i->tick())
				return true;			
		}
		return false;				
	}

	for (auto& child : m_childList)
	{
		if (child->tick())
			return true;
	}
	return false;	
}

bool bt::Composite::Sequence::tick()
{
	if(m_wait_node_.lock())
	{
		auto parent = std::dynamic_pointer_cast<ActionNode::Wait>(m_wait_node_.lock())->getParent();
		auto wait_node_id = m_wait_node_.lock()->getID();
		if(!m_wait_node_.lock()->tick())
			return false;
		bool next_is_found = false;
		for(auto& i : std::dynamic_pointer_cast<CompositeNode>(parent.lock())->getChilds())
		{
			if(!next_is_found)
			{
				if(wait_node_id == i->getID())
				{
					next_is_found = true;					
				}
			}
			else
			if (!i->tick())
				return false;
						
		}
		return true;				
	}
	
	for (auto& child : m_childList)
	{
		if (!child->tick())
			return false;
	}
	return true;
}

void bt::ActionNode::Wait::Setup(NodePtr node, NodePtr parent, NodePtr root)
{
	m_root_ = root;
	m_node_ = node;
	m_parent_ = parent;
}

bool bt::ActionNode::Wait::tick()
{
	auto root = std::dynamic_pointer_cast<Composite::CompositeNode>(m_root_.lock());
	if (root->GetWaitingNode().expired())
	{
		root->GetWaitingNode() = m_node_;
		m_timer_ = 0.f;
		return false;
	}
	m_timer_ += Tools::getDeltaTime();
	if(m_timer_ >= m_time_)
	{		
		root->GetWaitingNode().reset();
		m_timer_ = 0.f;
		return true;
	}

	return false;
}

bt::ActionNode::Play_Sound::Play_Sound(std::string sound, bool new_sound, std::weak_ptr<lc::GameObject> owner,float attenuation, float min_distance)
{
	m_sound_name_ = sound;
	m_start_new_sound_ = new_sound;
	m_sound_id_ = s_general_sound_id_;
	m_owner = owner;
	m_attenuation_ = attenuation;
	m_min_distance_ = min_distance;

	
}

void bt::ActionNode::Play_Sound::Setup(NodePtr node)
{
	if(m_owner.lock()->hasComponent("RigidBody"))
	{
		auto rb = m_owner.lock()->getComponent<lc::RigidBody>("RigidBody");
		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
		{
			auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Play_Sound>(node);
			auto pos = rigid_body->getParent()->getTransform().getPosition();
			GET_MANAGER->updateSoundPosition(node_cast->m_sound_name_,node_cast->m_sound_id_,sf::Vector3f(pos.x,pos.y,10.f));
		});
	}
}

bool bt::ActionNode::Play_Sound::tick()
{
	GET_MANAGER->playSound(m_sound_name_,m_sound_id_,m_start_new_sound_, false, m_attenuation_, m_min_distance_);	
	return true;
}

bt::ActionNode::Jump::Jump(float jump_height, std::weak_ptr<lc::GameObject> owner) : m_jump_height_(jump_height), m_owner_(owner), m_need_to_jump_(0.f)
{
}

void bt::ActionNode::Jump::Setup(NodePtr node)
{
	if(m_owner_.lock()->hasComponent("RigidBody"))
	{
		auto rb = m_owner_.lock()->getComponent<lc::RigidBody>("RigidBody");
		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
		{
			if(auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Jump>(node))
				if(node_cast->m_need_to_jump_ and rigid_body->getVelocity().y == 0.f)
				{
					rigid_body->getVelocity().y -= node_cast->m_jump_height_;
					node_cast->m_need_to_jump_ = false;
				}
		});
	}
}

bool bt::ActionNode::Jump::tick()
{
	if(!m_need_to_jump_)
	{
		m_need_to_jump_ = true;
	}
	return m_need_to_jump_;
}


bt::ActionNode::DamageBox::DamageBox(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::DamageBox::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{
				
				
				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::DamageBox>(node);


				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}

				
			});
	}
}

bool bt::ActionNode::DamageBox::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::CrazyHit::CrazyHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::CrazyHit::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::CrazyHit>(node);


				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}


			});
	}
}

bool bt::ActionNode::CrazyHit::tick()
{
	if(!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::LanceHit::LanceHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::LanceHit::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::LanceHit>(node);

				
				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}


			});
	}
}

bool bt::ActionNode::LanceHit::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::PickHit::PickHit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::PickHit::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::PickHit>(node);


				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}


			});
	}
}

bool bt::ActionNode::PickHit::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::Hit::Hit(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::Hit::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Hit>(node);


				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}


			});
	}
}

bool bt::ActionNode::Hit::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::lanceSpawn::lanceSpawn(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_damage_(false)
{
}

void bt::ActionNode::lanceSpawn::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::lanceSpawn>(node);


				if (node_cast->m_target_.lock() != nullptr)
				{
					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
					if (node_cast->m_bool_damage_)
					{
						if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(), rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(), rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()), rbt->getCollider()))
						{
							std::cout << "col damage" << std::endl;
							node_cast->m_target_.lock()->needToBeRemoved(true);
						}
						node_cast->m_bool_damage_ = false;
					}
				}


			});
	}
}

bool bt::ActionNode::lanceSpawn::tick()
{
	if (!m_agent_.expired() and !m_target_.expired())
	{
		m_bool_damage_ = true;
	}
	return m_bool_damage_;
}

bt::ActionNode::TestController::TestController(const std::shared_ptr<lc::GameObject>& agent_) : m_agent_(agent_), m_bool_controller_(false)
{
}

void bt::ActionNode::TestController::Setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::lanceSpawn>(node);

				if (KEY(Right))
				{
					rigid_body->getParent()->getTransform().getPosition().x += 500 * Tools::getDeltaTime();
				}
				if (KEY(Left))
				{
					rigid_body->getParent()->getTransform().getPosition().x -= 500 * Tools::getDeltaTime();
				}
				if (KEY(Up))
				{
					rigid_body->getVelocity().y = -500;
				}


			});
	}
}

bool bt::ActionNode::TestController::tick()
{
	if (!m_agent_.expired() )
	{
		m_bool_controller_ = true;
	}
	return m_bool_controller_;
}

bt::ActionNode::Play_Animation::Play_Animation(const std::weak_ptr<lc::GameObject>& owner,const std::string& anim_name,
	const std::string& key_anim_name) : m_owner_(owner), m_anim_name_(anim_name), m_key_anim_name_(key_anim_name)
{
}


bool bt::ActionNode::Play_Animation::tick()
{
	if(m_animation_.expired())
	{
		if(!m_owner_.expired())
		{
			auto object = m_owner_.lock();
			for(auto& component : object->getComponents())
			{
				if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
				{
					if(anim->getName() == this->m_anim_name_)
						this->m_animation_ = anim;
				}
			}
		}		
	}

	if(!m_animation_.expired())
	{
		auto anim = m_animation_.lock();
		anim->select_animation_key(m_key_anim_name_,true);
		return true;		
	}

	return false;
}


