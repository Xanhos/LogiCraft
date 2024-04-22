#pragma once
#include "SFML_ENGINE/Tools.h"
#include "Ressource.h"

namespace fs = std::filesystem;


class Folder //reproduce Windows folder
{
	Folder* m_parent;
	std::string m_parentFolder;
	bool m_open;
	fs::path m_path;
	std::list<Folder> m_childList;
	std::list<std::unique_ptr<lc::Ressource>> m_ressources;
	/**
	 * @brief Create a child folder
	 * 
	 * \param name Name of the new folder
	 */
	void createChild(std::string name);
	/**
	 * @brief Display the folder and his childrens in a ImGui TreeNode, this function need to be called inside a ImGui Window
	 * 
	 * 
	 * \param selectedFolder set this string to the path of the pressed folder in the TreeNode
	 */
	void displayFolder(std::string& selectedFolder);

	void Sort();

	char* m_newFolderName = new char[25];
	bool m_newFolder = false;

public:
	Folder()
		: m_open(false), m_parent(nullptr) {}
	Folder(std::string path, Folder* parent);
	~Folder() {}

	Folder(Folder&& other) noexcept; //Move constructor cause of unique_ptr

	//Return the list of all the ressources contains in the folder
	const std::list<std::unique_ptr<lc::Ressource>>& getFolderRessources();

	/**
	 * @brief Add a new Ressource in the folder
	 * 
	 * \param ressource New ressources to push
	 * \return Return a ref of the unique_ptr push into the list of ressources
	 */
	std::unique_ptr<lc::Ressource>& AddRessources(std::unique_ptr<lc::Ressource>&& ressource);

	/**
	 * @brief Make the update and the imgui display of the folder
	 * 
	 * \param selectedFolder set this string to the path of the pressed folder in the TreeNode
	 */
	void update(std::string& selectedFolder);

	/**
	 * @brief Open a new imgui window to create a child folder
	 * 
	 * \return True if the window is still open, else return false
	 */
	bool CreateNewFolder();


	/**
	 * @brief Return a folder if path matches the folder path
	 * 
	 * \param path path of the folder wanted
	 * \return a ref of the wanted folder
	 */
	Folder& getFolder(std::string path);

	/**
	 * @brief return the child list of the folder
	 * 
	 * \return a ref to the list of child folder
	 */
	std::list<Folder>& getFolderList() { return m_childList; }

	bool getNewFolder(bool value) { m_newFolder = value;return m_newFolder; }

	/**
	 * @brief Function to get the path of the folder
	 * 
	 * \return the path of the folder
	 */
	std::filesystem::path getPath();


	/**
	 * @brief Reload all the ressources of the folder
	 *
	 */
	void reload();

	/**
	 * @brief Save all the ressources of the folder into his real windows folder
	 * 
	 */
	void Save();
	/**
	 * @brief Load all the ressources in the windows folder into the memory folder
	 * 
	 */
	void Load();
};

