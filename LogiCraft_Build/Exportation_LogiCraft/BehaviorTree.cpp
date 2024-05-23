
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

void bt::ActionNode::NodeFunc::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
}

std::shared_ptr<bt::Node> bt::ActionNode::NodeFunc::clone()
{
	return std::make_shared<NodeFunc>(*this);
}

bt::ActionNode::MoveTo::MoveTo(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_, const float& speed_) :m_agent_(agent_), m_target_(target_), m_speed_(speed_), m_bool_go_to_(false)
{	
}

void bt::ActionNode::MoveTo::setup(std::shared_ptr<Node> node)
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
						rigid_body->getVelocity().x = Tools::Vector::normalize((node_cast->m_target_.lock()->getTransform().getPosition() + node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize()/2.f)
							- (node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize()/2.f)).x * node_cast->m_speed_;
					}
					node_cast->m_bool_go_to_ = false;
				}
				//else rigid_body->getVelocity().x = 0.f;
			});
	}
}

void bt::ActionNode::MoveTo::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	std::string target;
	file >> target;
	auto scene = owner;
	while(scene->getParent())
	{
		scene = scene->getParent();
	}
	m_agent_ = owner;
	m_target_ = scene->getObject(target);
	m_speed_ = 100.f;
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

void bt::ActionNode::Wander::setup(NodePtr node)
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
							if (Tools::Collisions::lineRect(side_collision, rect) && rb->getCanCollide())
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
			//else rigid_body->getVelocity().x = 0.f;

		});
	}
}

void bt::ActionNode::Wander::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	m_agent_ = owner;
	m_speed_ = 100.f;
}


bool bt::ActionNode::Wander::tick()
{
	if(!m_agent_.expired())
	{
		m_bool_wander_ = true;
	}
	return m_bool_wander_;

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

void bt::Composite::Selector::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
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

void bt::Composite::Sequence::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
}

void bt::ActionNode::Wait::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_timer_;
}

void bt::ActionNode::Wait::setup(std::shared_ptr<Node> node)
{
	auto root = getParent();
	while (!root.lock()->getParent().expired())
	{
		root = root.lock()->getParent();
	}
	m_node_ = node;
	m_root_ = root;
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

bool bt::ActionNode::Play_Sound::tick()
{
	GET_MANAGER->playSound(m_sound_name_,m_sound_id_,m_start_new_sound_, false, m_attenuation_, m_min_distance_);	
	return true;
}

void bt::ActionNode::Play_Sound::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	std::string sound_name;
	bool new_sound = false;
	float attenuation(0.f), minDistance(0.f);
	file >> m_sound_name_ >> m_start_new_sound_ >> m_attenuation_ >> m_min_distance_;
	m_owner = owner;
}

void bt::ActionNode::Play_Sound::setup(std::shared_ptr<Node> node)
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

bt::ActionNode::Jump::Jump(float jump_height, std::weak_ptr<lc::GameObject> owner) : m_jump_height_(jump_height), m_owner_(owner), m_need_to_jump_(false)
{
}

void bt::ActionNode::Jump::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_jump_height_;
	m_owner_ = owner;     
}

void bt::ActionNode::Jump::setup(std::shared_ptr<Node> node)
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

void bt::ActionNode::DamageBox::setup(NodePtr node)
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

void bt::ActionNode::CrazyHit::setup(NodePtr node)
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

void bt::ActionNode::LanceHit::setup(NodePtr node)
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

void bt::ActionNode::PickHit::setup(NodePtr node)
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

void bt::ActionNode::Hit::setup(NodePtr node)
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

void bt::ActionNode::lanceSpawn::setup(NodePtr node)
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

void bt::ActionNode::TestController::setup(NodePtr node)
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
	const std::string& key_anim_name, const bool& stop_at_last_frame, const bool& reverse)
: m_owner_(owner), m_anim_name_(anim_name), m_key_anim_name_(key_anim_name), m_stop_at_last_frame_(stop_at_last_frame),m_reverse_(reverse)
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
		anim->select_animation_key(m_key_anim_name_, m_key_anim_name_ != anim->get_actual_animation_key().lock()->get_name());
		anim->set_stop_at_last_frame(m_stop_at_last_frame_);
		anim->current_animation_is_reversed(m_reverse_);
		return true;		
	}

	return false;
}

void bt::ActionNode::Play_Animation::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_anim_name_ >> m_key_anim_name_ >> m_stop_at_last_frame_ >> m_reverse_;
	m_owner_ = owner->weak_from_this();
}

void bt::Decorator::Inverser::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
}

void bt::Decorator::Loop::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_loopNbr;
}


void bt::Decorator::Cooldown::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_executionTimer;
}


void bt::Decorator::ForceSuccess::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
}

void bt::Decorator::Condition::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	int type;
	file >> type;
	condition_type condition_type_enum = static_cast<condition_type>(type);
	auto condition = Condition_Factory(condition_type_enum, file);
	condition->load(file,owner);
	m_condition = condition;
}

bool bt::Decorator::Direction::tick()
{
	if(!m_owner_.expired())
	{
		auto agent = m_owner_.lock();
		if(agent->hasComponent("RigidBody"))
		{
			auto rb = agent->getComponent<lc::RigidBody>("RigidBody");
			if(m_direction_ == 0 and rb->getVelocity().x < 0)
				return true;
			if (m_direction_ == 1 and rb->getVelocity().x > 0)
				return true;							
		}
	}
	return false;	
}

void bt::Decorator::Direction::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	int direction;
	file >> direction;
	m_owner_ = owner;
}

void bt::Node::setup(std::shared_ptr<Node> node)
{
}

bt::Decorator::Do_On_Anim_Frame::Do_On_Anim_Frame(const std::weak_ptr<lc::GameObject>& owner,
	const std::string& anim_name, const std::string& key_anim_name, const unsigned int& action_frame) :
	m_owner_(owner),m_anim_name_(anim_name),m_key_anim_name_(key_anim_name),m_action_frame_(action_frame), m_new_frame_(true)
{
}

void bt::Decorator::Do_On_Anim_Frame::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_key_anim_name_ >>m_anim_name_ >> m_action_frame_;
	m_owner_ = owner;
}

bool bt::Decorator::Do_On_Anim_Frame::tick()
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

	if(const auto anim = m_animation_.lock())	
		if(const auto anim_key = anim->get_actual_animation_key().lock())		
			if(anim_key->get_name() == m_key_anim_name_)
			{
				if(anim_key->get_actual_frame() == m_action_frame_ /*and m_new_frame_*/)
				{
					m_new_frame_ = false;
					return SECURE_TASK(m_task);
				}
				/*if (anim_key->get_actual_frame() != m_action_frame_ )
					m_new_frame_ = true;*/
			}
	return false;
}

bt::ActionNode::In_Range_Of_Player::In_Range_Of_Player(std::weak_ptr<lc::GameObject> owner, float range) : m_owner_(owner), m_range_()
{
}

bool bt::ActionNode::In_Range_Of_Player::tick()
{
	if(m_player_.expired() and !m_owner_.expired())
	{
		auto scene = lc::GameObject::GetRoot(m_owner_.lock());
		if(scene->hasObject("PLAYER"))
			m_player_ = scene->getObject("PLAYER");
	}

	if(!m_player_.expired())
	{
		if(Tools::Vector::getDistance(m_player_.lock()->getTransform().getPosition(),m_owner_.lock()->getTransform().getPosition()) < m_range_)
			return true;
	}

	return false;
}

void bt::ActionNode::In_Range_Of_Player::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	m_owner_ = owner;
	file >> m_range_;
}


void bt::Node::init_custom_condition()
{
	m_custom_condition_map_["isHiding"] = New(ActionNode::IsHiding());
	m_custom_condition_map_["FarEnough"] = New(ActionNode::FarEnough());
}

bool bt::ActionNode::Attack::tick()
{
	return m_attack_node_->tick();
}

void bt::ActionNode::Attack::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	std::string attack_name;

    file >> attack_name;

    auto scene = lc::GameObject::GetRoot(owner);

    if (attack_name == "damageBox")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::DamageBox(owner, scene->getObject("Player")));
        std::cout << "damageBox loaded" << std::endl;
    }
    else if (attack_name == "lanceHit")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::LanceHit(owner, scene->getObject("Player")));
        std::cout << "lanceHit loaded" << std::endl;
    }
    else if (attack_name == "CrazyHit")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::CrazyHit(owner, scene->getObject("PLAYER")));
        std::cout << "CrazyHit loaded" << std::endl;
    }
    else if (attack_name == "pickHit")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::PickHit(owner, scene->getObject("PLAYER")));
        std::cout << "pickHit loaded" << std::endl;
    }
    else if (attack_name == "protect")
    {
        std::cout << "protect loaded" << std::endl;
    	m_attack_node_ = bt::Node::New(bt::ActionNode::NodeFunc([]{return true;}));
    }
    else if (attack_name == "hit")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::Hit(owner, scene->getObject("PLAYER")));
        std::cout << "hit loaded" << std::endl;
    }
    else if (attack_name == "lanceSpawn")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::lanceSpawn(owner, scene->getObject("PLAYER")));
        std::cout << "lanceSpawn loaded" << std::endl;
    }
    else if (attack_name == "controller")
    {
        m_attack_node_ = bt::Node::New(bt::ActionNode::TestController(owner));
        std::cout << "controller loaded" << std::endl;
    }
	else if (attack_name == "StayAway")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::StayAway(owner, scene->getObject("PLAYER")));
		std::cout << "StayAway loaded" << std::endl;
	}
	else if (attack_name == "ResetDirection")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::ResetDirection(owner, scene->getObject("PLAYER")));
		std::cout << "ResetDirection loaded" << std::endl;
	}
	else if (attack_name == "Dash")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::Dash(owner, scene->getObject("PLAYER")));
		std::cout << "Dash loaded" << std::endl;
	}
	else if (attack_name == "Retract")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::Retract(owner, scene->getObject("PLAYER")));
		std::cout << "Retract loaded" << std::endl;
	}
    else
    {
    	m_attack_node_ = bt::Node::New(bt::ActionNode::NodeFunc([]{return true;}));
        std::cout << "no attack of this name exist" << std::endl;
    }
	
	m_attack_node_->load(file,owner);
}

void bt::ActionNode::Attack::setup(std::shared_ptr<Node> node)
{
	m_attack_node_->setup(m_attack_node_);
}

bool bt::ActionNode::Shot::tick()
{
	return m_attack_node_->tick();
}

void bt::ActionNode::Shot::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	std::string m_attack_name;
	file >> m_attack_name;

	auto scene = lc::GameObject::GetRoot(owner);

	if (m_attack_name == "shoot")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::Shoot(owner, scene->getObject("PLAYER")));
		std::cout << "shoot loaded" << std::endl;
	}
	else if (m_attack_name == "SentryShoot")
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::SentryShoot(owner, scene->getObject("PLAYER")));
		std::cout << "shoot loaded" << std::endl;
	}
	else
	{
		m_attack_node_ = bt::Node::New(bt::ActionNode::NodeFunc([] {return true; }));
		std::cout << "no attack of this name exist" << std::endl;
	}
	
	m_attack_node_->load(file, owner);
}

void bt::ActionNode::Shot::setup(std::shared_ptr<Node> node)
{
	m_attack_node_->setup(m_attack_node_);
}

bool bt::ActionNode::ChangeState::tick()
{
	GetRoot().lock()->getState() = m_new_state_;
	return true;
}

void bt::ActionNode::ChangeState::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_new_state_;
}

bool bt::Decorator::IfStateIs::tick()
{
	if(m_state_needed_ == GetRoot().lock()->getState())
		return m_task->tick();
	return false;
}

void bt::Decorator::IfStateIs::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	file >> m_state_needed_;
}

bt::NodePtr bt::Factory(const node_type& type)
{
	switch (type) {
	case node_type::SEQUENCE:
		return Node::New(Composite::Sequence());
	case node_type::SELECTOR:
		return Node::New(Composite::Selector());
	case node_type::INVERSER:
		return Node::New(Decorator::Inverser());
	case node_type::CONDITION:
		return Node::New(Decorator::Condition());
	case node_type::DIRECTION:
		return Node::New(Decorator::Direction());
	case node_type::LOOP:
		return Node::New(Decorator::Loop());
	case node_type::COOLDOWN:
		return Node::New(Decorator::Cooldown());
	case node_type::FORCE_SUCCESS:
		return Node::New(Decorator::ForceSuccess());
	case node_type::IF_STATE_IS:
		return Node::New(Decorator::IfStateIs());
	case node_type::WANDER:
		return Node::New(ActionNode::Wander());
	case node_type::MOVE_TO:
		return Node::New(ActionNode::MoveTo());
	case node_type::PLAY_ANIMATION:
		return Node::New(ActionNode::Play_Animation());
	case node_type::PLAY_SOUND:
		return Node::New(ActionNode::Play_Sound());
	case node_type::WAIT:
		return Node::New(ActionNode::Wait());
	case node_type::DO_ON_ANIM_FRAME:
		return Node::New(Decorator::Do_On_Anim_Frame());
	case node_type::ATTACK:
		return Node::New(ActionNode::Attack());
	case node_type::SHOT:
		return Node::New(ActionNode::Shot());
	case node_type::JUMP:
		return Node::New(ActionNode::Jump());	
	case node_type::CHANGE_STATE:
		return Node::New(ActionNode::ChangeState());		
	default:
		return Node::New(Composite::Sequence());
	}
}

bt::NodePtr bt::Condition_Factory(const condition_type& type, std::ifstream& file)
{
	std::string custom_condition;
	switch (type)
	{
	case condition_type::IN_RANGE_OF_PLAYER:
		return Node::New(ActionNode::In_Range_Of_Player());
	case condition_type::IS_PLAYER_IN_SIGHT:
		return Node::New(ActionNode::Is_Player_In_Sight());
	case condition_type::CUSTOM:
		file >> custom_condition;
		return Node::get_unordered_map_custom_condition()[custom_condition]->clone();
	default:
		return Node::New(ActionNode::NodeFunc([]{return true;}));
	}
}

bt::ActionNode::IsHiding::IsHiding(std::weak_ptr<lc::GameObject> owner) : m_owner_(owner)
{
}

bool bt::ActionNode::IsHiding::tick()
{
	if (!m_owner_.expired())
		if (m_owner_.lock()->getComponent<lc::Animation>("Animation")->get_actual_animation_key().lock()->get_name() == "hiding" && m_owner_.lock()->getComponent<lc::Animation>("Animation")->get_actual_animation_key().lock()->get_actual_frame() != 0)
			return true;
	
	return false;
}

void bt::ActionNode::IsHiding::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	m_owner_ = owner;
}

std::shared_ptr<bt::Node> bt::ActionNode::IsHiding::clone()
{
	return  std::make_shared<IsHiding>(*this);
}

bt::ActionNode::StayAway::StayAway(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_away_(false)
{
}

void bt::ActionNode::StayAway::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rb = agent->getComponent<lc::RigidBody>("RigidBody");

		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
			{
				auto node_cast = std::dynamic_pointer_cast<StayAway>(node);
				if (node_cast->m_bool_away_)
				{
					if (!node_cast->m_target_.expired())
					{
						if (Tools::Vector::getDistance(node_cast->m_agent_.lock()->getTransform().getPosition(), node_cast->m_target_.lock()->getTransform().getPosition()) < 1000)
						{
							rigid_body->getVelocity().x = -Tools::Vector::normalize((node_cast->m_target_.lock()->getTransform().getPosition() + node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f)
								- (node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize() / 2.f)).x * 100.f;
						}
					}
					node_cast->m_bool_away_ = false;
				}
			});
	}
}

bool bt::ActionNode::StayAway::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_away_ = true;
	}
	return m_bool_away_;
}

bt::ActionNode::ResetDirection::ResetDirection(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_direction_(false)
{
}

void bt::ActionNode::ResetDirection::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rb = agent->getComponent<lc::RigidBody>("RigidBody");

		rb->AddInputFunction([node](lc::RigidBody* rigid_body)
			{
				auto node_cast = std::dynamic_pointer_cast<ResetDirection>(node);
				if (node_cast->m_bool_direction_)
				{
					if (!node_cast->m_target_.expired())
					{
						if (node_cast->m_target_.lock()->getTransform().getPosition().x > node_cast->m_agent_.lock()->getTransform().getPosition().x)
						{
							node_cast->m_agent_.lock()->getTransform().getScale().x = -1.f;
							node_cast->m_agent_.lock()->getTransform().getOrigin().x = node_cast->m_agent_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize().x;
						}
						else
						{
							node_cast->m_agent_.lock()->getTransform().getScale().x = 1.f;
							node_cast->m_agent_.lock()->getTransform().getOrigin() = sf::Vector2f();
						}
							
					}
					node_cast->m_bool_direction_ = false;
				}
			});
	}
}

bool bt::ActionNode::ResetDirection::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_direction_ = true;
	}
	return m_bool_direction_;
}

bt::ActionNode::FarEnough::FarEnough(std::weak_ptr<lc::GameObject> owner)
{
}

bool bt::ActionNode::FarEnough::tick()
{
	if (!m_owner_.expired() && !m_player_.expired())
		if (Tools::Vector::getDistance(m_owner_.lock()->getTransform().getPosition(), m_player_.lock()->getTransform().getPosition()) > 1000 )
			return true;

	return false;
}

void bt::ActionNode::FarEnough::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	m_owner_ = owner;
	m_player_ = lc::GameObject::GetRoot(owner)->getObject("PLAYER");
}

std::shared_ptr<bt::Node> bt::ActionNode::FarEnough::clone()
{
	return std::make_shared<FarEnough>(*this);
}

bt::ActionNode::Shoot::Shoot(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_shoot_(false)
{
}

void bt::ActionNode::Shoot::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");
		//auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Shoot>(node);
		
		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{

				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Shoot>(node);

				if (node_cast->m_bool_shoot_)
				{
					if (!node_cast->m_target_.expired())
					{
						auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
						if (node_cast->m_bool_shoot_)
						{
							
							std::shared_ptr<lc::GameObject> tmpBalle = std::make_shared<lc::GameObject>();
							tmpBalle->getTransform().getPosition() = node_cast->m_agent_.lock()->getTransform().getPosition();
							tmpBalle->getTransform().getOrigin() = sf::Vector2f(100, 100);
							tmpBalle->addComponent<lc::Texture>(lc::Texture("test", "../ASSETS/200x200D.png", sf::IntRect(0,0,200,200)));
							std::shared_ptr<lc::RigidBody> tmpRB = std::make_shared<lc::RigidBody>(lc::RigidBody(sf::FloatRect(0, 0, 200, 200), sf::Vector2f(), sf::Vector2f(), true));
							tmpRB->getIsFlying() = true;
							tmpRB->getCanCollide() = false;
							sf::Vector2f tmpPos = node_cast->m_target_.lock()->getTransform().getPosition();
							sf::Vector2f tmpCenter = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f;
							sf::Vector2f tmpVelocity = Tools::Vector::normalize((node_cast->m_target_.lock()->getTransform().getPosition() + node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f)
								- (node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize() / 2.f)) * 500.f;
							float aliveTimer = 0.f;

							tmpRB->AddInputFunction([tmpVelocity, &node_cast](lc::RigidBody* rigid_body) {
								//TODO : Degat Balle
								rigid_body->getVelocity() = tmpVelocity; 
								if (!node_cast->m_target_.expired())
								{
									auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");

									if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(),
										rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(),
										rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(),
										rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()),
										rbt->getCollider()))
									{
										std::cout << "col damage" << std::endl;
										node_cast->m_target_.lock()->needToBeRemoved(true);
									}
								}
								
								});

							tmpBalle->addComponent<lc::RigidBody>(tmpRB);
							
							
							lc::GameObject::GetRoot(node_cast->m_agent_.lock())->addObject(tmpBalle);

							node_cast->m_bool_shoot_ = false;
						}
					}
				}

				


			});
	}
}

bool bt::ActionNode::Shoot::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_shoot_ = true;
	}
	return m_bool_shoot_;
}

bt::ActionNode::Is_Player_In_Sight::Is_Player_In_Sight(std::weak_ptr<lc::GameObject> owner) : m_owner_(owner)
{
}

bool bt::ActionNode::Is_Player_In_Sight::tick()
{
	if (!m_player_.expired())
	{
		if (m_owner_.lock()->getTransform().getScale().x == 1 && m_player_.lock()->getTransform().getPosition().x < m_owner_.lock()->getTransform().getPosition().x)
			return true;
		else if (m_owner_.lock()->getTransform().getScale().x == -1 && m_player_.lock()->getTransform().getPosition().x > m_owner_.lock()->getTransform().getPosition().x)
			return true;
	}

	return false;
}

void bt::ActionNode::Is_Player_In_Sight::load(std::ifstream& file, std::shared_ptr<lc::GameObject> owner)
{
	m_owner_ = owner;
	m_player_ = lc::GameObject::GetRoot(owner)->getObject("PLAYER");
}

bt::ActionNode::SentryShoot::SentryShoot(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_shoot_(false)
{

}

void bt::ActionNode::SentryShoot::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");
		auto rbt = agent->getComponent<lc::RigidBody>("RigidBody");
		//auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Shoot>(node);

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{

				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::SentryShoot>(node);

				if (node_cast->m_bool_shoot_)
				{
					if (!node_cast->m_target_.expired())
					{
						auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
						if (node_cast->m_bool_shoot_)
						{

							std::shared_ptr<lc::GameObject> tmpBalle = std::make_shared<lc::GameObject>();
							tmpBalle->getTransform().getPosition() = node_cast->m_agent_.lock()->getTransform().getPosition();
							tmpBalle->getTransform().getOrigin() = sf::Vector2f(100, 100);
							tmpBalle->addComponent<lc::Texture>(lc::Texture("test", "../ASSETS/200x200D.png", sf::IntRect(0, 0, 200, 200)));
							std::shared_ptr<lc::RigidBody> tmpRB = std::make_shared<lc::RigidBody>(lc::RigidBody(sf::FloatRect(0, 0, 200, 200), sf::Vector2f(), sf::Vector2f(), true));
							tmpRB->getIsFlying() = true;
							tmpRB->getCanCollide() = false;
							sf::Vector2f tmpPos = node_cast->m_target_.lock()->getTransform().getPosition();
							sf::Vector2f tmpCenter = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f;
							sf::Vector2f tmpVelocity = Tools::Vector::normalize((node_cast->m_target_.lock()->getTransform().getPosition() + node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f)
								- (node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize() / 2.f)) * 500.f;


							tmpRB->AddInputFunction([tmpVelocity, &node_cast](lc::RigidBody* rigid_body) {
								//TODO : Degat Balle
								rigid_body->getVelocity().x = tmpVelocity.x;
								if (!node_cast->m_target_.expired())
								{
									auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");
									if (Tools::Collisions::rect_rect(sf::FloatRect(rigid_body->getCollider().getPosition().x - rigid_body->getVelocity().x * Tools::getDeltaTime(),
										rigid_body->getCollider().getPosition().y - rigid_body->getVelocity().y * Tools::getDeltaTime(),
										rigid_body->getCollider().getSize().x + rigid_body->getVelocity().x * Tools::getDeltaTime(),
										rigid_body->getCollider().getSize().y + rigid_body->getVelocity().y * Tools::getDeltaTime()),
										sf::FloatRect(rbt->getCollider().getPosition().x - rbt->getVelocity().x * Tools::getDeltaTime(),
											rbt->getCollider().getPosition().y - rbt->getVelocity().y * Tools::getDeltaTime(),
											rbt->getCollider().getSize().x + rbt->getVelocity().x * Tools::getDeltaTime(),
											rbt->getCollider().getSize().y + rbt->getVelocity().y * Tools::getDeltaTime())

									))
									{

										std::cout << "col damage" << std::endl;
										node_cast->m_target_.lock()->needToBeRemoved(true);
									}
								}
								});

							
								

							tmpBalle->addComponent<lc::RigidBody>(tmpRB);


							lc::GameObject::GetRoot(node_cast->m_agent_.lock())->addObject(tmpBalle);

							node_cast->m_bool_shoot_ = false;
						}
					}
				}




			});
	}
}

bool bt::ActionNode::SentryShoot::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_shoot_ = true;
	}
	return m_bool_shoot_;
}

bt::ActionNode::Dash::Dash(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_dash_(false), m_bool_dash_end(false), m_one_pass(false)
{
}

void bt::ActionNode::Dash::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Dash>(node);

				if (node_cast->m_bool_dash_)
				{
					
					rigid_body->getVelocity() = node_cast->m_velocity_;


					if (Tools::Vector::getDistance(node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize() / 2.f, node_cast->m_t_previous_pos_) < 200)
					{
							node_cast->m_bool_dash_end = true;
							
					}


					

					node_cast->m_bool_dash_ = false;
				}

				


			});
	}

}

bool bt::ActionNode::Dash::tick()
{

	if (!m_agent_.expired())
	{
		if (!m_one_pass)
		{
			if (!m_target_.expired())
			{
				m_velocity_ = Tools::Vector::normalize((m_target_.lock()->getTransform().getPosition() + m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f)
					- (m_agent_.lock()->getTransform().getPosition() + m_agent_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f)) * 700.f;
			}
			

			m_t_previous_pos_ = m_target_.lock()->getTransform().getPosition() + m_target_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f;

			m_bool_dash_end = false;

			m_one_pass = true;
		}
		

		m_bool_dash_ = true;
	}
		
	if (m_bool_dash_end)
		m_one_pass = false;

	return m_bool_dash_end;
}

bt::ActionNode::Retract::Retract(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_retract_(false), m_bool_retract_end(false), m_one_pass(false)
{
}

void bt::ActionNode::Retract::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");


		
		//auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Shoot>(node);

	

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{
				

				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Retract>(node);


				if (node_cast->m_bool_retract_)
				{
					
					rigid_body->getVelocity().y = -150;

					auto rbt = node_cast->m_target_.lock()->getComponent<lc::RigidBody>("RigidBody");


					if (Tools::Vector::getDistance(node_cast->m_agent_.lock()->getTransform().getPosition() + rigid_body->getCollider().getSize() / 2.f, node_cast->m_a_last_pos_) > 500)
					{
						node_cast->m_bool_retract_end = true;
					}


					

					node_cast->m_bool_retract_ = false;

				}

				



			});
	}
}

bool bt::ActionNode::Retract::tick()
{
	if (!m_agent_.expired())
	{
		if (!m_one_pass)
		{
			m_bool_retract_end = false;
			m_a_last_pos_ = m_agent_.lock()->getTransform().getPosition() + m_agent_.lock()->getComponent<lc::RigidBody>("RigidBody")->getCollider().getSize() / 2.f;
			m_one_pass = true;
		}
		
		m_bool_retract_ = true;
	}
		
	if (m_bool_retract_end)
		m_one_pass = false;

	return m_bool_retract_end;
}

bt::ActionNode::Exploding::Exploding(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_), m_bool_explode_(false)
{
}

void bt::ActionNode::Exploding::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");



		//auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Shoot>(node);



		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::Exploding>(node);


				if (node_cast->m_bool_explode_)
				{

					
					

					node_cast->m_bool_explode_ = false;

				}





			});
	}
}

bool bt::ActionNode::Exploding::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_explode_ = true;
	}

	return m_bool_explode_;
}

bt::ActionNode::LavaShooting::LavaShooting(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_) : m_agent_(agent_), m_target_(target_),m_bool_shoot_(false)
{
}

void bt::ActionNode::LavaShooting::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody") && m_target_.lock()->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::LavaShooting>(node);


				if (node_cast->m_bool_shoot_)
				{



					 
					node_cast->m_bool_shoot_ = false;
				}





			});
	}
}

bool bt::ActionNode::LavaShooting::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_shoot_ = true;
	}

	return m_bool_shoot_;
}

bt::ActionNode::ResetRigidBody::ResetRigidBody(const std::shared_ptr<lc::GameObject>& agent_, const std::shared_ptr<lc::GameObject>& target_): m_agent_(agent_), m_bool_reset_(false)
{
}

void bt::ActionNode::ResetRigidBody::setup(NodePtr node)
{
	auto agent = m_agent_.lock();
	if (agent->hasComponent("RigidBody"))
	{
		auto rba = agent->getComponent<lc::RigidBody>("RigidBody");

		rba->AddInputFunction([node](lc::RigidBody* rigid_body)
			{


				auto node_cast = std::dynamic_pointer_cast<bt::ActionNode::ResetRigidBody>(node);


				if (node_cast->m_bool_reset_)
				{

					node_cast->m_agent_.lock()->getTransform().getOrigin() = sf::Vector2f();
					rigid_body->getCollider().width = node_cast->m_agent_.lock()->getTransform().getSize().x;
					rigid_body->getCollider().height = node_cast->m_agent_.lock()->getTransform().getSize().y;

					node_cast->m_bool_reset_ = false;
				}





			});
	}
}

bool bt::ActionNode::ResetRigidBody::tick()
{
	if (!m_agent_.expired())
	{
		m_bool_reset_ = true;
	}

	return m_bool_reset_;
}
