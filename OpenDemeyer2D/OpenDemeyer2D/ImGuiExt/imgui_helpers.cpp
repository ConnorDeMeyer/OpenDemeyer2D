#include "imgui_helpers.h"

#include <limits.h>

#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Prefab.h"

#include "Singletons/ResourceManager.h"

constexpr float slideSpeed{ 0.5f };

bool ImGui::CustomInput(const char* label, short& input)
{
	static_assert(sizeof(short) == 2);
	return DragScalar(label, ImGuiDataType_S16, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, unsigned short& input)
{
	static_assert(sizeof(unsigned short) == 2);
	return DragScalar(label, ImGuiDataType_U16, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, int& input)
{
	static_assert(sizeof(int) == 4);
	return DragScalar(label, ImGuiDataType_S32, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, unsigned int& input)
{
	static_assert(sizeof(unsigned int) == 4);
	return DragScalar(label, ImGuiDataType_U32, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, long long& input)
{
	static_assert(sizeof(long long) == 8);
	return DragScalar(label, ImGuiDataType_S64, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, unsigned long long& input)
{
	static_assert(sizeof(unsigned long long) == 8);
	return DragScalar(label, ImGuiDataType_U64, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, bool& input)
{
	return Checkbox(label, &input);
}

bool ImGui::CustomInput(const char* label, char& input)
{
	static_assert(sizeof(char) == 1);
	return DragScalar(label, ImGuiDataType_S8, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, unsigned char& input)
{
	static_assert(sizeof(unsigned char) == 1);
	return DragScalar(label, ImGuiDataType_U8, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, float& input)
{
	return DragScalar(label, ImGuiDataType_Float, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, double& input)
{
	return DragScalar(label, ImGuiDataType_Double, &input, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::vec2& input)
{
	return DragFloat2(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::vec3& input)
{
	return DragFloat3(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::vec4& input)
{
	return DragFloat4(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::ivec2& input)
{
	return DragInt2(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::ivec3& input)
{
	return DragInt3(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::ivec4& input)
{
	return DragInt4(label, &input.x, slideSpeed);
}

bool ImGui::CustomInput(const char* label, glm::mat2x2& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat2x3& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat2x4& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat3x2& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat3x3& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat3x4& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat4x2& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	changed |= CustomInput("[3]", input[3]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat4x3& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	changed |= CustomInput("[3]", input[3]);
	return changed;
}

bool ImGui::CustomInput(const char* label, glm::mat4x4& input)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("[0]", input[0]);
	changed |= CustomInput("[1]", input[1]);
	changed |= CustomInput("[2]", input[2]);
	changed |= CustomInput("[3]", input[3]);
	return changed;
}

bool ImGui::CustomInput(const char* label, SDL_Color& input)
{
	static_assert(sizeof(SDL_Color) == sizeof(ImU32));
	static_assert(offsetof(ImVec4, x) == 0);
	static_assert(offsetof(SDL_Color, r) == 0);

	ImVec4 color = ColorConvertU32ToFloat4(reinterpret_cast<ImU32&>(input));
	bool changed = ColorEdit4(label, &color.x);
	auto color32 = ColorConvertFloat4ToU32(color);
	input = reinterpret_cast<SDL_Color&>(color32);

	return changed;
}

bool ImGui::CustomInput(const char* label, SDL_Rect& rect)
{
	Text(label);
	bool changed{};
	changed |= CustomInput("X Coordinate", rect.x);
	changed |= CustomInput("Y Coordinate", rect.y);
	changed |= CustomInput("Width", rect.w);
	changed |= CustomInput("Height", rect.h);
	return changed;
}

bool ImGui::CustomInput(const char* label, SDL_FRect& rect)
{
	ImGui::Text(label);
	bool changed{};
	changed |= CustomInput("X Coordinate", rect.x);
	changed |= CustomInput("Y Coordinate", rect.y);
	changed |= CustomInput("Width", rect.w);
	changed |= CustomInput("Height", rect.h);

	return changed;
}

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
