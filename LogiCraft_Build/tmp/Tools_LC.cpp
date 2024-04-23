#include "Tools_LC.h"

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
