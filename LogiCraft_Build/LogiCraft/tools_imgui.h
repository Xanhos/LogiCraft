
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

#pragma once
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/WindowManager.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui-SFML.h"
#include "imnodes.h"
#include "imnodes_internal.h"

#include <windows.h>
#include <commdlg.h>
#include <mutex>
#include <sstream>
#include <fstream>

#undef CreateWindow

using namespace ImGui;
namespace fs = std::filesystem;


class FileWriter : public std::ofstream
{
	fs::path m_path;

public:
	FileWriter(fs::path path);

	fs::path getPath() const;
};



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

namespace Tools
{
	extern bool camera_grabbed;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void UpdateZoom(const sf::Event& _event);

		void Update();

		void Clear();

		void Draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);

		void Draw(const sf::Vertex* vertices, std::size_t vertexCount,
			sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default);

		void Draw(const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states = sf::RenderStates::Default);

		void Draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, 
			std::size_t vertexCount, const sf::RenderStates& states = sf::RenderStates::Default);

		void Display();

	public:
		std::shared_ptr<sf::RenderTexture>& get_render_texture();

		sf::Vector2f& get_added_position();
		sf::Vector2f& get_last_added_position();
		sf::Vector2f& get_size();

		sf::View& get_view();

		bool& his_focus();
		bool& his_grabbed();

		float& get_zoom();
	private:
		void UpdateResize();

		void UpdateMovement();
	private:
		std::shared_ptr<sf::RenderTexture> m_renderer_;
		sf::Vector2f m_renderer_added_position_;
		sf::Vector2f m_renderer_last_added_position_;
		sf::Vector2f m_renderer_size_;
		sf::View m_renderer_view_;

		bool m_renderer_is_focus_;
		bool m_renderer_is_grabbed_;

		float m_renderer_zoom_;
	};

	namespace IG
	{
		template <typename T>
		void SetSpacing(sf::Vector2f size, T func)
		{
			auto lastSize = ImGui::GetStyle().ItemSpacing;
			ImGui::GetStyle().ItemSpacing = size;
			func();
			ImGui::GetStyle().ItemSpacing = lastSize;
		}

		template <typename T>
		static void CreateWindow(std::string name, bool& boolean, ImGuiWindowFlags flags, T func)
		{
			ImGui::Begin(name.c_str(), &boolean, flags);
			func();
			ImGui::End();
		}

		template <typename T>
		static void CreateChild(std::string name, ImVec2 size, ImGuiChildFlags childflag, ImGuiWindowFlags flags, T func)
		{
			ImGui::BeginChild(name.c_str(), size, childflag, flags);
			func();
			ImGui::EndChild();
		}

		void LoadRessourcesFromFile(std::string& path, const char* filter = "All\0*.*\0Text\0*.TXT\0");

		bool MouseIsOnAboveWindow(ImGuiWindow* _window = ImGui::GetCurrentWindow());
	}



	bool sameFile(const std::string& sourceFile, const std::string& destFile);
	void copyFile(const fs::path& sourceFile, const fs::path& destFile);

	static std::list<std::string> s_filePool;

	namespace Collisions
	{
		bool lineRect(sf::FloatRect line, sf::FloatRect rect);
		bool lineLine(sf::FloatRect line_one, sf::FloatRect line_two);
	}
	//Replace all the character that are the same as _characterToReplace to _replaceCharacter.
	void ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar);

	//Remplace les _ par des espaces si le bool vaut 0 sinon fait l'inverse
	std::string replaceSpace(std::string string, bool spaceOrUnderscore = 0);
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