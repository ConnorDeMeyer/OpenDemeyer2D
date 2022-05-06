#pragma once
#include <memory>

#include "imgui.h"
#include "EngineFiles/Component.h"
#include "EngineFiles/Scene.h"

#include "Components/Transform.h"

namespace ImGui 
{
	template <typename T>
	void ComponentSelect(const char* label, ComponentBase* thisComponent, std::weak_ptr<T>& component)
	{
		if (ImGui::BeginCombo(label, component.expired() ? typeid(T).name() : component.lock()->GetParent()->GetDisplayName().c_str()))
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
	}

}