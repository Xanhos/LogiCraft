#pragma once
#include "SFML_ENGINE/Tools.h"

namespace fs = std::filesystem;


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
