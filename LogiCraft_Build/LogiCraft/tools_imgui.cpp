
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

#include "tools_imgui.h"
#include "nfd.h"

bool Tools::CameraGrabbed = false;

void Tools::IG::LoadRessourcesFromFile(std::string& path, const char* filter)
{
	{

		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(filter, NULL, &outPath);
		path = "";
		if(outPath)
			path = std::string(outPath);
	}
}

bool Tools::IG::MouseIsOnAboveWindow(ImGuiWindow* _window)
{
	for (auto& window : ImGui::GetCurrentContext()->Windows)
		if (window->ID != _window->ID)
			if (Tools::Collisions::point_rect(ImGui::GetMousePos(), { window->Pos, window->Size }) &&
				ImGui::IsWindowAbove(window, _window) &&
				window->WasActive &&
				std::string(window->Name).find("lc") != std::string::npos)
			{
				return true;
			}
	 
	return false;
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

void Tools::ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar)
{
	for (auto& character : _sentence)
		if (character == _characterToReplace)
			character = _replaceChar;
}

std::string Tools::replaceSpace(std::string string, bool spaceOrUnderscore)
{
	if (!spaceOrUnderscore)
	{
		for (size_t i = 0; i < string.length(); ++i)
		{
			if (string[i] == '_')
			{
				string[i] = ' ';
			}
		}
	}
	else
	{
		for (size_t i = 0; i < string.length(); ++i)
		{
			if (string[i] == ' ')
			{
				string[i] = '_';
			}
		}
	}
	return string;
}
