#include "Texture.h"
#include "GameObject.h"


lc::Texture::Texture()
{
	m_renderer.setTexture(&m_texture);
	m_isUsedByAComponent = false;
}

lc::Texture::Texture(std::string name, sf::IntRect rect, std::string path, std::string newRepo)
{
	fs::path p = path;
	m_textureName = p.filename().string();
	m_texture.loadFromFile(path);
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = name;
	m_typeName = "Texture";
	m_type = TYPE::TEXTURE;
	if (newRepo != "")
		m_texturePath = newRepo + "/" + m_textureName;
	else m_texturePath = path;

	m_isUsedByAComponent = false;
	m_filterColor = sf::Color(0xF90FF0FF);
}

lc::Texture::Texture(const Texture& _texture)
{
	m_texture = _texture.m_texture;
	m_textureName = _texture.m_textureName;
	m_texturePath = _texture.m_texturePath;
	m_textureRect = _texture.m_textureRect;
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = _texture.m_name;
	m_textureName = _texture.m_textureName;
	m_typeName = "Texture";
	m_filterColor = sf::Color(0xF90FF0FF);
	m_type = TYPE::TEXTURE;
	m_isUsedByAComponent = false;
}

lc::Texture::Texture(std::string name, sf::IntRect rect, std::string path, sf::Vector2f _position, sf::Vector2f _scale, sf::Vector2f _relative)
{
	m_typeName = "Texture";
	m_texturePath = path;
	fs::path p = path;
	m_textureName = p.filename().string();
	m_texture.loadFromFile(path);
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = name;
	m_type = TYPE::TEXTURE;
	m_filterColor = sf::Color(0xF90FF0FF);
	m_relativePosition = _relative;
	m_isUsedByAComponent = false;
}

lc::Texture::~Texture()
{
}

void lc::Texture::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
	//if (!getParent()->getIsSaved())
	{
		save << static_cast<int>(m_type)
			<< " " << Tools::replaceSpace(m_typeName, true)
			<< " " << m_textureRect.left
			<< " " << m_textureRect.top
			<< " " << m_textureRect.width
			<< " " << m_textureRect.height
			<< " " << Tools::replaceSpace(m_texturePath, true)
			<< " " << m_relativePosition.x
			<< " " << m_relativePosition.y << std::endl;
	}
}

void lc::Texture::Load(std::ifstream& load)
{
	load >> m_typeName >>
		m_textureRect.left >>
		m_textureRect.top >>
		m_textureRect.width >>
		m_textureRect.height >>
		m_texturePath >>
		m_relativePosition.x >>
		m_relativePosition.y;

	fs::path p = m_texturePath;
	m_textureName = p.filename().string();
	m_name = p.filename().stem().string();
	m_texture.loadFromFile(m_texturePath);
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_type = TYPE::TEXTURE;
}

void lc::Texture::UpdateEvent(sf::Event& _window)
{
}

void lc::Texture::Update(WindowManager& _window)
{
	if (getParent())
		setPosition(getParent()->getTransform().getPosition());
}

void lc::Texture::Draw(WindowManager& _window)
{
	if (!m_isUsedByAComponent)
	{
		if (getParent())
		{
			m_renderer.setPosition(getParent()->getTransform().getPosition());
		}

		m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());
		if (m_displayThumbnail)
			m_renderer.setSize(m_thumbnailSize);

		_window.draw(m_renderer);

		onDraw(_window, sf::Color::Magenta);
	}
}

void lc::Texture::Draw(sf::RenderTexture& _window)
{
	if (!m_isUsedByAComponent)
	{
		if (getParent())
		{
			m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
			m_renderer.setScale(getParent()->getTransform().getScale());
			m_renderer.setRotation(getParent()->getTransform().getRotation());
			m_renderer.setOrigin(getParent()->getTransform().getOrigin());
		}

		m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());

		_window.draw(m_renderer);
	}
}

std::shared_ptr<lc::GameComponent> lc::Texture::Clone()
{
	return std::make_shared<lc::Texture>(*this);
}

void lc::Texture::setHierarchieFunc()
{
	m_hierarchieInformation = [this]()
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::Text("Texture Name : %s", m_textureName.c_str());

			if (!m_isUsedByAComponent)
				ImGui::DragFloat2("Relative Position", m_relativePosition, 1.0f, 0.0f, 0.0f, "%.2f");
	};
}
