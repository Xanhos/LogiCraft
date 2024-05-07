
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

#include "AI.h"
#include "GameObject.h"

lc::AI::AI() : m_root_()
{
    m_type = TYPE::IA;
    m_typeName = "IA";
    m_name = "AI";
}

lc::AI::~AI()
{
}


void lc::AI::Load(std::ifstream& load)
{
    int child_size;
    int type;
    load >> type >> child_size;
    std::function<void(bt::NodePtr&,const bt::NodePtr&)> load_node_and_child = [&](bt::NodePtr& node,const bt::NodePtr& parent)
    {
        load >> type >> child_size;
		const auto type_cast = static_cast<bt::node_type>(type);
        node = bt::Factory(type_cast);
        if(type_cast == bt::node_type::MOVE_TO)
        {
            std::string target;
			load >> target;
            auto scene = getParent();
            while(scene->getParent())
			{
				scene = scene->getParent();
			}

            node = bt::Node::New(bt::ActionNode::MoveTo(getParent(),scene->getObject(target),100.f));
            std::dynamic_pointer_cast<bt::ActionNode::MoveTo>(node)->Setup(node);
		}
        else if (type_cast == bt::node_type::ATTACK)
        {
            std::string attack_name;
            load >> attack_name;

            if (attack_name == "damageBox")
            {
                std::cout << "damageBox loaded" << std::endl;
            }
            else if (attack_name == "lanceHit")
            {
                std::cout << "lanceHit loaded" << std::endl;
            }
            else if (attack_name == "CrazyHit")
            {
                std::cout << "CrazyHit loaded" << std::endl;
            }
            else if (attack_name == "pickHit")
            {
                std::cout << "CrazyHit loaded" << std::endl;
            }
            else if (attack_name == "protect")
            {
                std::cout << "protect loaded" << std::endl;
            }
            else
            {
                std::cout << "no attack of this name exist" << std::endl;
            }


        }
        else if (type_cast == bt::node_type::SHOT)
        {
            std::string garbage;
            load >> garbage;
        }
        else if (type_cast == bt::node_type::JUMP)
        {
            float jump_height;
            load >> jump_height;
            node = bt::Node::New(bt::ActionNode::Jump(jump_height,getParent()));
            std::dynamic_pointer_cast<bt::ActionNode::Jump>(node)->Setup(node);            
        }
        else if (type_cast == bt::node_type::PLAY_SOUND)
        {
            std::string sound_name;
            bool new_sound = false;
            float attenuation(0.f), minDistance(0.f);
            load >> sound_name >> new_sound >> attenuation >> minDistance;
            
            node = bt::Node::New(bt::ActionNode::Play_Sound(sound_name,new_sound,getParent(),attenuation,minDistance));
            std::dynamic_pointer_cast<bt::ActionNode::Play_Sound>(node)->Setup(node);
        }
        else if(type_cast == bt::node_type::WANDER)
        {
            node = bt::Node::New(bt::ActionNode::Wander(getParent()));
            std::dynamic_pointer_cast<bt::ActionNode::Wander>(node)->Setup(node);
        }
        else if(type_cast == bt::node_type::CONDITION)
        {
            int condition_type;
            load >> condition_type;
            if(condition_type == 0)
            {
                float detection_range;
                load >> detection_range;
                auto scene = getParent();
                while(scene->getParent())
                    scene = scene->getParent();
                std::weak_ptr<GameObject> player = scene->getObject("slug"), agent = getParent();
                std::dynamic_pointer_cast<bt::Decorator::Condition>(node)->setCondition([player, agent, detection_range]
                {
                    if(!player.expired() and !agent.expired())
                    {
                        return Tools::Vector::getDistance(player.lock()->getTransform().getPosition(),agent.lock()->getTransform().getPosition()) < detection_range;
                    }
                    return false;
                });
                if(child_size)
                {
                    std::dynamic_pointer_cast<bt::Decorator::Condition>(node)->setTask(bt::Node::New(bt::Composite::Sequence()));
                    load_node_and_child(std::dynamic_pointer_cast<bt::Decorator::Condition>(node)->getTask(), parent);
                }
            }
            
        }
        else if(type_cast == bt::node_type::COOLDOWN)
        {
        	float time;
			load >> time;
			node = bt::Node::New(bt::Decorator::Cooldown());
            std::dynamic_pointer_cast<bt::Decorator::Cooldown>(node)->setTimer(time);
            if(child_size)
            {
                std::dynamic_pointer_cast<bt::Decorator::Cooldown>(node)->setTask(bt::Node::New(bt::Composite::Sequence()));
                load_node_and_child(std::dynamic_pointer_cast<bt::Decorator::Cooldown>(node)->getTask(), parent);
			}
        }
        else if (type_cast == bt::node_type::LOOP)
        {
            int loop;
            load >> loop;
            node = bt::Node::New(bt::Decorator::Loop());
            std::dynamic_pointer_cast<bt::Decorator::Loop>(node)->setLoop(loop);
            if (child_size)
            {
                std::dynamic_pointer_cast<bt::Decorator::Loop>(node)->setTask(bt::Node::New(bt::Composite::Sequence()));
                load_node_and_child(std::dynamic_pointer_cast<bt::Decorator::Loop>(node)->getTask(), parent);
            }
        }
        else if (type_cast == bt::node_type::WAIT)
        {
            float timer;
            load >> timer;
            node = bt::Node::New(bt::ActionNode::Wait());
            std::dynamic_pointer_cast<bt::ActionNode::Wait>(node)->setTimer(timer);
            std::dynamic_pointer_cast<bt::ActionNode::Wait>(node)->Setup(std::dynamic_pointer_cast<bt::ActionNode::Wait>(node), parent, m_root_);
        }
        else if (type_cast == bt::node_type::INVERSER or type_cast == bt::node_type::FORCE_SUCCESS)
        {
            if (child_size)
            {
                type_cast == bt::node_type::INVERSER ? std::dynamic_pointer_cast<bt::Decorator::Inverser>(node)->setTask(bt::Node::New(bt::Composite::Sequence())) : std::dynamic_pointer_cast<bt::Decorator::ForceSuccess>(node)->setTask(bt::Node::New(bt::Composite::Sequence()));
                load_node_and_child(type_cast == bt::node_type::INVERSER ? std::dynamic_pointer_cast<bt::Decorator::Inverser>(node)->getTask() : std::dynamic_pointer_cast<bt::Decorator::ForceSuccess>(node)->getTask(), parent);
            }
        }
        else if (type_cast < bt::node_type::INVERSER)
        {
        	for (int i = 0; i < child_size; i++)
			{
                dynamic_cast<bt::Composite::CompositeNode*>(node.get())->addChild(bt::Node::New(bt::Composite::Sequence()));
			}
            for (auto& i : dynamic_cast<bt::Composite::CompositeNode*>(node.get())->getChilds())
            {
                load_node_and_child(i, parent);
            }
        }
	};
    m_root_ = bt::Node::New(bt::Composite::Sequence());
    for (int i = 0; i < child_size; i++)
    {
        m_root_->addChild(bt::Node::New(bt::Composite::Sequence()));
    }
    for (auto& i : m_root_->getChilds())
    {
        load_node_and_child(i, m_root_);
    }
}




void lc::AI::UpdateEvent(sf::Event& _window)
{
}

void lc::AI::Update(WindowManager& _window)
{
    m_root_->tick();
}



void lc::AI::Draw(WindowManager& _window)
{
}

void lc::AI::Draw(sf::RenderTexture& _window)
{
}

std::shared_ptr<lc::GameComponent> lc::AI::Clone()
{
    return std::make_shared<lc::AI>(*this);
}
