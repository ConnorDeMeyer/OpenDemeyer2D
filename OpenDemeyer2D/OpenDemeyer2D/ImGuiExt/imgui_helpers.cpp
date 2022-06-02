#include "imgui_helpers.h"

#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Prefab.h"

#include "Singletons/ResourceManager.h"

void ImGui::ResourceSelect(const char* label, std::shared_ptr<Texture2D>& Resource)
{
	if (ImGui::BeginCombo("TextureFile", (Resource) ? Resource->GetFilePath().string().c_str() : label))
	{
		for (auto& resources : RESOURCES.GetTexture2DFiles())
		{
			if (!resources.second.expired() && ImGui::Selectable(resources.first.string().c_str()))
			{
				Resource = resources.second.lock();
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget())
	{
		constexpr std::string_view texture2d = type_name<Texture2D>();
		std::string texture2dString{ texture2d };

		if (auto payload = ImGui::AcceptDragDropPayload(texture2dString.c_str()))
		{
			Resource = *static_cast<const std::shared_ptr<Texture2D>*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}

void ImGui::ResourceSelect(const char* label, std::shared_ptr<Surface2D>& Resource)
{
	if (ImGui::BeginCombo("SurfaceFile", (Resource) ? Resource->GetFilePath().string().c_str() : label))
	{
		for (auto& resources : RESOURCES.GetTexture2DFiles())
		{
			if (!resources.second.expired() && ImGui::Selectable(resources.first.string().c_str()))
			{
				auto filePath = resources.second.lock()->GetFilePath();
				Resource = RESOURCES.LoadSurface(filePath);
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget())
	{
		constexpr std::string_view surface2D = type_name<Surface2D>();
		std::string surface2DString{ surface2D };

		if (auto payload = ImGui::AcceptDragDropPayload(surface2DString.c_str()))
		{
			Resource = *static_cast<const std::shared_ptr<Surface2D>*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}

void ImGui::ResourceSelect(const char* label, std::shared_ptr<Sound>& Resource)
{
	if (ImGui::BeginCombo("SoundFile", (Resource) ? Resource->GetFilePath().string().c_str() : label))
	{
		for (auto& resources : RESOURCES.GetSoundFiles())
		{
			if (!resources.second.expired() && ImGui::Selectable(resources.first.string().c_str()))
			{
				Resource = resources.second.lock();
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget())
	{
		constexpr std::string_view sound = type_name<Sound>();
		std::string soundString{ sound };

		if (auto payload = ImGui::AcceptDragDropPayload(soundString.c_str()))
		{
			Resource = *static_cast<const std::shared_ptr<Sound>*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}

void ImGui::ResourceSelect(const char* label, std::shared_ptr<Music>& Resource)
{
	if (ImGui::BeginCombo("MusicFile", (Resource) ? Resource->GetFilePath().string().c_str() : label))
	{
		for (auto& resources : RESOURCES.GetMusicFiles())
		{
			if (!resources.second.expired() && ImGui::Selectable(resources.first.string().c_str()))
			{
				Resource = resources.second.lock();
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget())
	{
		constexpr std::string_view music = type_name<Music>();
		std::string musicString{ music };

		if (auto payload = ImGui::AcceptDragDropPayload(musicString.c_str()))
		{
			Resource = *static_cast<const std::shared_ptr<Music>*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}

void ImGui::ResourceSelect(const char* label, std::shared_ptr<Prefab>& Resource)
{
	if (ImGui::BeginCombo("Prefab File", (Resource) ? Resource->GetGameObject()->GetDisplayName().c_str() : label))
	{
		for (auto& resources : RESOURCES.GetPrefabs())
		{
			if (!resources.second.expired() && ImGui::Selectable(resources.first.string().c_str()))
			{
				Resource = resources.second.lock();
			}
		}
		
		ImGui::EndCombo();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (auto payload = ImGui::AcceptDragDropPayload(type_name<Prefab>().data()))
		{
			Resource = *static_cast<const std::shared_ptr<Prefab>*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}
