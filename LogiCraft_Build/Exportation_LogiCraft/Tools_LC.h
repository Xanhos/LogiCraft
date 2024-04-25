#pragma once
#include "SFML_ENGINE/Tools.h"

namespace fs = std::filesystem;



namespace lc
{
	class GameObject;
	class GameComponent;
}

typedef std::list<std::weak_ptr<lc::GameObject>> ObjWeakPtrList;
typedef std::list<std::shared_ptr<lc::GameObject>> ObjSharedPtrList;

typedef std::list<std::weak_ptr<lc::GameComponent>> CompWeakPtrList;
typedef std::list<std::shared_ptr<lc::GameComponent>> CompSharedPtrList;

typedef std::pair<bool, std::string> RessourceToSearch;

const sf::Vector2i screenResolution = sf::Vector2i(3840, 2160);

//Need to get the path of the file we open, so we make a class that inherit from std::ofstream and that store the path of the file.

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

	namespace Collisions
	{
		bool lineRect(sf::FloatRect line, sf::FloatRect rect);
		bool lineLine(sf::FloatRect line_one, sf::FloatRect line_two);
	}
	//Replace all the character that are the same as _characterToReplace to _replaceCharacter.
	void ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar);

	//Remplace les _ par des espaces si le bool vaut 0 sinon fait l'inverse
	std::string replaceSpace(std::string string, bool spaceOrUnderscore = false);
}
