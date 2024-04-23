#include "Tools_LC.h"


#include "GameObject.h"

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