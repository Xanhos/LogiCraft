
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
        node->getParent() = parent;
        node->load(load,getParent());
        node->setup(node);        
        /*if (type_cast == bt::node_type::ATTACK)
        {
            


        }*/
        if(child_size)
        {
            if(auto decorator_node = std::dynamic_pointer_cast<bt::Decorator::Decorator>(node))
            {
                decorator_node->setTask(bt::Node::New(bt::Composite::Sequence()));
                load_node_and_child(decorator_node->getTask(), parent);
            }
            else if (auto composite_node = std::dynamic_pointer_cast<bt::Composite::CompositeNode>(node))
            {
                for (int i = 0; i < child_size; i++)
                {
                    composite_node->addChild(bt::Node::New(bt::Composite::Sequence()));
                }
                for (auto& i : composite_node->getChilds())
                {
                    load_node_and_child(i, parent);
                }
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
