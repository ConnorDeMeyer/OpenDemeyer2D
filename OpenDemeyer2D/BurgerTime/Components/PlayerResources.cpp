#include "PlayerResources.h"
#include "Components/TextureComponent.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "Components/TextPixelComponent.h"
#include "PeterPepper.h"

#include "ResourceWrappers/Texture2D.h"
#include "ImGuiExt/CustomImGuiFields.h"
#include "ImGuiExt/imgui_helpers.h"

void PlayerResources::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::shared_ptr<Texture2D>>("Texture", offsetof(PlayerResources, m_pTexture));
	binder.Add<std::weak_ptr<PeterPepper>>("Player", offsetof(PlayerResources, m_Player));
	binder.Add<std::weak_ptr<TextPixelComponent>>("PepperText", offsetof(PlayerResources, m_pPepperText));
	binder.Add<SDL_FRect>("SourceRect", offsetof(PlayerResources, m_SourceRect));
}

void PlayerResources::RenderImGui()
{
	ImGui::ResourceSelect("Resource Texture", m_pTexture);

	ImGui::ComponentSelect("Player", this, m_Player);

	ImGui::CustomInput("Resource Source Rect", m_SourceRect);

	if (ImGui::Button("Preview Lifes"))
	{
		GenerateLives();
	}

	ImGui::Separator();

	ImGui::ComponentSelect("Pepper Text", this, m_pPepperText);
}

void PlayerResources::BeginPlay()
{
	GenerateLives();

	if (auto player{ m_Player.lock() })
	{
		player->OnLifeLost.BindFunction(this, std::bind(&PlayerResources::LoseLife, this));
		player->OnPepperSpray.BindFunction(this, std::bind(&PlayerResources::LosePepper, this));

		if (auto pepperText{ m_pPepperText.lock()})
		{
			pepperText->SetText(std::to_string(player->GetPepperAmount()));
		}
	}
}

void PlayerResources::GenerateLives()
{
	// remove all existant children
	for (auto child : GetObject()->GetChildren())
	{
		child->Destroy();
	}
	m_Resources.clear();

	if (auto player{ m_Player.lock() })
	{
		int lives = player->GetLives();

		for (int i{}; i < lives; ++i)
		{
			auto go =  new GameObject();
			
			auto render = go->AddComponent<RenderComponent>();
			render->SetSourceRect(m_SourceRect);
			
			auto texture = go->AddComponent<TextureComponent>();
			texture->SetTexture(m_pTexture);

			auto transform = go->GetTransform();
			transform->SetPosition({ 0,i * m_SourceRect.h });

			m_Resources.emplace_back(go);
			
			go->SetParent(GetObject());
		}
	}
}

void PlayerResources::LosePepper()
{
	if (auto text{ m_pPepperText.lock() })
		if (auto player{ m_Player.lock() })
			text->SetText(std::to_string(player->GetPepperAmount()));
}

void PlayerResources::LoseLife()
{
	if (m_Resources.empty()) return;

	m_Resources.back()->Destroy();
	m_Resources.pop_back();
}

void PlayerResources::SetResourceTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_pTexture = texture;
	UpdateResources();
}

void PlayerResources::SetSourceRect(const SDL_FRect& rect)
{
	m_SourceRect = rect;
	UpdateResources();
}

void PlayerResources::UpdateResources()
{
	float TexturePosY{};
	for (auto go : m_Resources)
	{
		auto textureComp = go->GetComponent<TextureComponent>();
		auto renderComp = GetRenderComponent();
		if (renderComp && textureComp) 
		{
			textureComp->SetTexture(m_pTexture);
			renderComp->SetSourceRect(m_SourceRect);
			go->GetTransform()->SetPosition({ 0,TexturePosY });
			TexturePosY += m_SourceRect.h;
		}
	}
}
