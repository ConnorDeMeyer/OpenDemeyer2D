#include "CustomImGuiFields.h"

#include "imgui.h"

bool ImGui::CustomInput(const char* label, SDL_Rect& rect)
{
	ImGui::Text(label);
	bool changedx = ImGui::InputInt("X Coordinate", &rect.x);
	bool changedy = ImGui::InputInt("Y Coordinate", &rect.y);
	bool changedw = ImGui::InputInt("Width", &rect.w);
	bool changedh = ImGui::InputInt("Height", &rect.h);

	return changedx || changedy || changedw || changedh;
}

bool ImGui::CustomInput(const char* label, SDL_FRect& rect)
{
	ImGui::Text(label);
	bool changedx = ImGui::InputFloat("X Coordinate", &rect.x);
	bool changedy = ImGui::InputFloat("Y Coordinate", &rect.y);
	bool changedw = ImGui::InputFloat("Width", &rect.w);
	bool changedh = ImGui::InputFloat("Height", &rect.h);

	return changedx || changedy || changedw || changedh;
}
