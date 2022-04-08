#include "SpriteComponent.h"
#include "RenderComponent.h"
#include "../ResourceManager.h"
#include "../RenderManager.h"
#include "imgui.h"


void SpriteComponent::BeginPlay()
{
	m_pRenderComponent = GetParent()->GetRenderComponent();

	if (m_pRenderComponent && m_Texture) {
		m_pRenderComponent->SetTexture(m_Texture);
		m_pRenderComponent->SetSourceRect(SDL_FRect{
				m_CurrentFrameX * m_FrameDimension.x,
				m_CurrentFrameY * m_FrameDimension.y,
				m_FrameDimension.x,
				m_FrameDimension.y
			});
	}
}

void SpriteComponent::Update(float deltaTime)
{
	if (m_bPauseTime) return;

	m_AccumulatedTime += deltaTime;
	if (m_AccumulatedTime > m_TimePerFrame)
	{
		m_AccumulatedTime -= m_TimePerFrame;
		if (++m_CurrentFrame >= m_TotalFrames + m_FrameOffset)
		{
			if (m_bLoop)
				m_CurrentFrame = m_FrameOffset;
			else
				m_bPauseTime = true;

			OnAnimationEnd.BroadCast();
		}

		const int horizontalFrames{ int(m_Texture->GetWidth() / m_FrameDimension.x) };
		m_CurrentFrameY = m_CurrentFrame / horizontalFrames;
		m_CurrentFrameX = m_CurrentFrame % horizontalFrames;

	}
	if (m_pRenderComponent)
		m_pRenderComponent->SetSourceRect(SDL_FRect{
			m_CurrentFrameX * m_FrameDimension.x,
			m_CurrentFrameY * m_FrameDimension.y,
			m_FrameDimension.x,
			m_FrameDimension.y
			});
	
}

void SpriteComponent::SetTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_Texture = texture;
	if (m_pRenderComponent) m_pRenderComponent->SetTexture(m_Texture);
}

void SpriteComponent::SetTexture(const std::string& filePath)
{
	m_Texture = RESOURCES.LoadTexture(filePath);
	if (m_pRenderComponent && m_Texture) m_pRenderComponent->SetTexture(m_Texture);
}

void SpriteComponent::RenderImGui()
{
	// image
#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_Texture->GetId()), { 100,100 });
#pragma warning(default : 4312)

	// Time per frame
	float tpf{ m_TimePerFrame };
	ImGui::InputFloat("Seconds Per Frame", &tpf);

	if (tpf != m_TimePerFrame)
	{
		m_TimePerFrame = tpf;
	}

	// frame dimensions
	float dims[2]{ m_FrameDimension.x, m_FrameDimension.y };
	ImGui::InputFloat2("Frame Dimensions", dims);

	m_FrameDimension.x = dims[0];
	m_FrameDimension.y = dims[1];

	// frame offset
	ImGui::InputInt("Frame Offset", &m_FrameOffset);
	ImGui::InputInt("Current Frame", &m_CurrentFrame);
	ImGui::Checkbox("Pause", &m_bPauseTime);
	ImGui::Checkbox("Loop", &m_bLoop);

	// Reset button
	if (ImGui::Button("Reset"))
	{
		Reset();
	}
}

void SpriteComponent::SetFrameOffset(int offset)
{
	m_FrameOffset = offset;
	SetCurrentFrame(m_CurrentFrame - m_FrameOffset);
}

void SpriteComponent::SetCurrentFrame(int frame)
{
	m_CurrentFrame = frame + m_FrameOffset;
	int horizontalFrames{ int(m_Texture->GetWidth() / m_FrameDimension.x) };
	m_CurrentFrameY = m_CurrentFrame / horizontalFrames;
	m_CurrentFrameX = m_CurrentFrame % horizontalFrames;
}

void SpriteComponent::Reset()
{
	m_CurrentFrame = m_FrameOffset;
	m_AccumulatedTime = 0;
	m_bPauseTime = false;
}