#include "Tools_LC.h"
#include "GameObject.h"

ThreadManager::~ThreadManager()
{
	while(!m_thread_list_.empty())
		this->Update();
}

void ThreadManager::Update()
{
	for(auto it = m_thread_list_.begin();it != m_thread_list_.end();)
	{
		if(*it->second)
		{
			it->first.join();
			it = m_thread_list_.erase(it);
		}
	}
}

FileWriter::FileWriter(fs::path path) : std::ofstream(path), m_path(path)
{
}

fs::path FileWriter::getPath() const
{
	return m_path;
}

FileReader::FileReader(fs::path path) : std::ifstream(path), m_path(path)
{
}

fs::path FileReader::getPath() const
{
	return m_path;
}

std::string Tools::replaceSpace(std::string string, bool spaceOrUnderscore)
{
	for (size_t i = 0; i < string.length(); ++i)
	{
		if (string[i] == (!spaceOrUnderscore ? '_' : ' '))
		{
			string[i] = (!spaceOrUnderscore ? ' ' : '_');
		}
	}
	
	return string;
}

void Tools::ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar)
{
	for (auto& character : _sentence)
		if (character == _characterToReplace)
			character = _replaceChar;
}

bool Tools::Collisions::lineRect(sf::FloatRect line, sf::FloatRect rect)
{
	bool left = lineLine(line, { rect.left, rect.top, rect.left, rect.top + rect.height });
	bool right = lineLine(line, { rect.left + rect.width,  rect.top, rect.left + rect.width, rect.top + rect.height });
	bool top = lineLine(line, { rect.left,  rect.top, rect.left + rect.width,  rect.top });
	bool bottom = lineLine(line, { rect.left, rect.top + rect.height, rect.left + rect.width, rect.top + rect.height });

	bool lineInRect = Tools::Collisions::point_rect(line.getPosition(), rect) or Tools::Collisions::point_rect(line.getSize(), rect);
	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom || lineInRect) {
		return true;
	}
	return false;
}

bool Tools::Collisions::lineLine(sf::FloatRect line_one, sf::FloatRect line_two)
{
	float uA = ((line_two.width - line_two.left) * (line_one.top - line_two.top) - (line_two.height - line_two.top) * (line_one.left - line_two.left)) / ((line_two.height - line_two.top) * (line_one.width - line_one.left) - (line_two.width - line_two.left) * (line_one.height - line_one.top));
	float uB = ((line_one.width - line_one.left) * (line_one.top - line_two.top) - (line_one.height - line_one.top) * (line_one.left - line_two.left)) / ((line_two.height - line_two.top) * (line_one.width - line_one.left) - (line_two.width - line_two.left) * (line_one.height - line_one.top));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

		return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream& os, const sf::Color& color)
{
	return os << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " "
			  << static_cast<int>(color.b) << " " << static_cast<int>(color.a);
}

std::istream& operator>>(std::istream& is, sf::Color& color)
{
	int tmp_tab[4]{ 0, 0, 0, 0 };
	is >> tmp_tab[0] >> tmp_tab[1] >> tmp_tab[2] >> tmp_tab[3];
	color = sf::Color(static_cast<sf::Uint8>(tmp_tab[0]), static_cast<sf::Uint8>(tmp_tab[1]),
		static_cast<sf::Uint8>(tmp_tab[2]), static_cast<sf::Uint8>(tmp_tab[3]));
	return is;
}