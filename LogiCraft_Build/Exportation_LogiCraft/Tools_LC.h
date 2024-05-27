#pragma once
#include "SFML_ENGINE/Tools.h"

typedef std::map<unsigned char, std::string> Layers;
typedef std::pair<unsigned char, std::string> Layer;

namespace fs = std::filesystem;

const std::string background_holder_name = "BACKGROUND_HOLDER";

class ThreadManager
{
	std::list<std::pair<std::thread,std::shared_ptr<bool>>> m_thread_list_;
	unsigned char m_thread_limit_;
public:
	ThreadManager() : m_thread_limit_(10) {}
	ThreadManager(const unsigned char& thread_limit) : m_thread_limit_(thread_limit){}
	~ThreadManager();
	ThreadManager operator=(const ThreadManager& thread) = delete;
	
	template <typename Func>
	void AddNewThread(const Func& func);
	void Update();	
};

template <typename Func>
void ThreadManager::AddNewThread(const Func& func)
{
	while(m_thread_list_.size() == m_thread_limit_)
		this->Update();
	auto is_thread_finish = std::make_shared<bool>(false);
	m_thread_list_.push_back({std::thread([func, is_thread_finish]
	{
		func();
		*is_thread_finish = true;
	}),is_thread_finish});
}

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
	const Layers s_layers = { {13, "BackGround 9"},{12, "BackGround 8"},{11, "BackGround 7"},{10,"BackGround 6"},{ 9, "BackGround 5"},{8, "BackGround 4"},{7, "BackGround 3"},{ 6, "BackGround 2"},{5, "BackGround 1"},{4, "Player Plan"},{3, "Front Plan 4"}, {2, "Front Plan 3"}, {1, "Front Plan 2"}, {0, "Front Plan 1"} };
	
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


template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& vec)
{
	return os << vec.x << " " << vec.y;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& rect)
{
	return os << rect.left << " " << rect.top << " " << rect.width << " " << rect.height;
}

std::ostream& operator<<(std::ostream& os, const sf::Color& color);

template<typename T>
std::istream& operator>>(std::istream& is, sf::Vector2<T>& vec)
{
	return is >> vec.x >> vec.y;
}

template<typename T>
std::istream& operator>>(std::istream& is, sf::Rect<T>& rect)
{
	return is >> rect.left >> rect.top >> rect.width >> rect.height;
}

std::istream& operator>>(std::istream& is, sf::Color& color);