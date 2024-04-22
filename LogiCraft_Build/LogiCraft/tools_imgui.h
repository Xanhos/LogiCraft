#pragma once
#include "SFML_ENGINE/Tools.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui-SFML.h"
#include "imnodes.h"
#include "imnodes_internal.h"

#include <windows.h>
#include <commdlg.h>
#include <mutex>
#include <sstream>

#undef CreateWindow

using namespace ImGui;

namespace Tools
{
	extern bool CameraGrabbed;

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

