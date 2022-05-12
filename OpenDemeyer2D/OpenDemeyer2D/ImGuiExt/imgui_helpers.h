#pragma once
#include <memory>

#include "imgui.h"
#include "EngineFiles/Component.h"
#include "EngineFiles/Scene.h"

#include "EngineIO/Reflection.h"

#include "Components/Transform.h"

class Texture2D;
class Surface2D;
class Sound;
class Music;
class RenderTarget;
class Font;

namespace ImGui 
{
	template <typename T>
	void ComponentSelect(const char* label, ComponentBase* thisComponent, std::weak_ptr<T>& component)
	{
		if (ImGui::BeginCombo(label, component.expired() ? type_name<T>().data() : component.lock()->GetParent()->GetDisplayName().c_str()))
		{
			auto& objects = thisComponent->GetParent()->GetScene()->GetAllObjects();
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

}