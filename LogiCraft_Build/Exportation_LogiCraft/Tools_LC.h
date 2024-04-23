#pragma once
#include "SFML_ENGINE/Tools.h"

namespace fs = std::filesystem;

//Need to get the path of the file we open, so we make a class that inherit from std::ofstream and that store the path of the file.

const sf::Vector2i screenResolution = sf::Vector2i(3840, 2160);

class FileWriter : public std::ofstream
{
	fs::path m_path;

public:
	FileWriter(fs::path path);

	fs::path getPath() const;
};

class FileReader : public std::ifstream
{
	fs::path m_path;

public:
	FileReader(fs::path path);

	fs::path getPath() const;
};

namespace Tools
{

	//Replace all the character that are the same as _characterToReplace to _replaceCharacter.
	void ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar);

	//Remplace les _ par des espaces si le bool vaut 0 sinon fait l'inverse
	std::string replaceSpace(std::string string, bool spaceOrUnderscore = false);
}
