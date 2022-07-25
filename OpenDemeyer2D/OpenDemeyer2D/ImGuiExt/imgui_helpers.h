#pragma once
#include <memory>
#include <SDL.h>
#include <glm/glm.hpp>

#include "imgui.h"
#include "EngineFiles/ComponentBase.h"

#include "EngineIO/Reflection.h"

class Texture2D;
class Surface2D;
class Sound;
class Music;
class RenderTarget;
class Font;
class Prefab;

namespace ImGui 
{
	// Fundamental C++ types

	bool CustomInput(const char* label, short& input);
	bool CustomInput(const char* label, unsigned short& input);
	bool CustomInput(const char* label, int& input);
	bool CustomInput(const char* label, unsigned int& input);
	bool CustomInput(const char* label, long long& input);
	bool CustomInput(const char* label, unsigned long long& input);
	bool CustomInput(const char* label, bool& input);
	bool CustomInput(const char* label, char& input);
	bool CustomInput(const char* label, unsigned char& input);
	bool CustomInput(const char* label, float& input);
	bool CustomInput(const char* label, double& input);


	// GLM types

	bool CustomInput(const char* label, glm::vec2& input);
	bool CustomInput(const char* label, glm::vec3& input);
	bool CustomInput(const char* label, glm::vec4& input);
	bool CustomInput(const char* label, glm::ivec2& input);
	bool CustomInput(const char* label, glm::ivec3& input);
	bool CustomInput(const char* label, glm::ivec4& input);
	bool CustomInput(const char* label, glm::mat2x2& input);
	bool CustomInput(const char* label, glm::mat2x3& input);
	bool CustomInput(const char* label, glm::mat2x4& input);
	bool CustomInput(const char* label, glm::mat3x2& input);
	bool CustomInput(const char* label, glm::mat3x3& input);
	bool CustomInput(const char* label, glm::mat3x4& input);
	bool CustomInput(const char* label, glm::mat4x2& input);
	bool CustomInput(const char* label, glm::mat4x3& input);
	bool CustomInput(const char* label, glm::mat4x4& input);

	// SDL types

	bool CustomInput(const char* label, SDL_Rect& input);
	bool CustomInput(const char* label, SDL_FRect& input);
	bool CustomInput(const char* label, SDL_Color& input);



	template <typename T>
	void ComponentSelect(const char* label, ComponentBase* thisComponent, std::weak_ptr<T>& component)
	{
		if (ImGui::BeginCombo(label, component.expired() ? std::string{ type_name<T>() }.c_str() : component.lock()->GetGameObject()->GetDisplayName().c_str()))
		{
			auto& objects = thisComponent->GetGameObject()->GetScene()->GetAllObjects();
			for (auto& object : objects)
			{
				auto pComponent = object.second->GetComponent<T>();
				if (pComponent)
				{
					if (ImGui::Selectable(object.second->GetDisplayName().c_str()))
					{
						component = pComponent->GetWeakReferenceType();
					}
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
			{
				auto goPayload = *static_cast<GameObject**>(payload->Data);
				if (auto comp = goPayload->GetComponent<T>())
				{
					component = comp->GetWeakReferenceType();
				}
			}

			ImGui::EndDragDropTarget();
		}
	}

	void ResourceSelect(const char* label, std::shared_ptr<Texture2D>& Resource);
	void ResourceSelect(const char* label, std::shared_ptr<Surface2D>& Resource);
	void ResourceSelect(const char* label, std::shared_ptr<Sound>& Resource);
	void ResourceSelect(const char* label, std::shared_ptr<Music>& Resource);
	void ResourceSelect(const char* label, std::shared_ptr<Prefab>& Resource);
	//void ResourceSelect(const char* label, std::shared_ptr<Scene>& Resource);

}