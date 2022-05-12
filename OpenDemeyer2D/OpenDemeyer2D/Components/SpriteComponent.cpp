#include "SpriteComponent.h"
#include "RenderComponent.h"
#include "Singletons/ResourceManager.h"
#include "Singletons/RenderManager.h"
#include "imgui.h"


void SpriteComponent::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<glm::vec2>("frameDimension", offsetof(SpriteComponent, m_FrameDimension));
	binder.Add<int>("frameOffset", offsetof(SpriteComponent, m_FrameOffset));
	binder.Add<float>("frameTime", offsetof(SpriteComponent, m_TimePerFrame));
	binder.Add<int>("totalFrames", offsetof(SpriteComponent, m_TotalFrames));
	binder.Add<bool>("paused", offsetof(SpriteComponent, m_bPauseTime));
	binder.Add<bool>("loop", offsetof(SpriteComponent, m_bLoop));
}

void SpriteComponent::Initialize()
{
	UpdateSourceRect();
}

void SpriteComponent::Update(float deltaTime)
{
	if (m_bPauseTime) return;

	m_AccumulatedTime += deltaTime;
	if (m_AccumulatedTime > m_TimePerFrame)
	{
		m_AccumulatedTime -= m_TimePerFrame;
		++m_CurrentFrame;
		m_NeedsUpdate = true;
	}

	if (m_NeedsUpdate)
	{
		UpdateSourceRect();
	}
}

void SpriteComponent::RenderImGui()
{
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

void SpriteComponent::SetFrameDimension(const glm::vec2& dimension)
{
	if (dimension != m_FrameDimension) {
		m_FrameDimension = dimension;
		m_NeedsUpdate = true;
	}
}

void SpriteComponent::SetFrameOffset(int offset)
{
	if (offset != m_FrameOffset) {
		m_FrameOffset = offset;
		m_NeedsUpdate = true;
	}
}

void SpriteComponent::SetTotalFrames(int amount)
{
	if (m_TotalFrames != amount)
	{
		m_TotalFrames = amount;
		m_NeedsUpdate = true;
	}
}

void SpriteComponent::SetTimePerFrame(float value)
{
	if (m_TimePerFrame != value)
	{
		m_TimePerFrame = value;
		m_NeedsUpdate = true;
	}
}

void SpriteComponent::SetCurrentFrame(int frame)
{
	RenderComponent* render = GetRenderComponent();
	if (!render)
		return;

	auto texture = render->GetTexture();
	if (!texture)
		return;

	m_CurrentFrame = frame;
	int actualFrame = m_CurrentFrame + m_FrameOffset;
	int horizontalFrames{ int(texture->GetWidth() / m_FrameDimension.x) };
	m_CurrentFrameY = actualFrame / horizontalFrames;
	m_CurrentFrameX = actualFrame % horizontalFrames;
}

void SpriteComponent::Reset()
{
	m_CurrentFrame = 0;
	m_AccumulatedTime = 0;
	m_bPauseTime = false;
}

void SpriteComponent::UpdateSourceRect()
{
	if (m_CurrentFrame >= m_TotalFrames)
	{
		if (m_bLoop)
			m_CurrentFrame = 0;
		else
			m_bPauseTime = true;

		OnAnimationEnd.BroadCast();
	}

	RenderComponent* render = GetRenderComponent();
	if (!render)
		return;

	auto texture = render->GetTexture();
	if (!texture)
		return;

	int actualFrame = m_CurrentFrame + m_FrameOffset;
	int horizontalFrames{ int(texture->GetWidth() / m_FrameDimension.x) };
	m_CurrentFrameY = actualFrame / horizontalFrames;
	m_CurrentFrameX = actualFrame % horizontalFrames;

	render->SetSourceRect(SDL_FRect{
		m_CurrentFrameX * m_FrameDimension.x,
		m_CurrentFrameY * m_FrameDimension.y,
		m_FrameDimension.x,
		m_FrameDimension.y
		});
}
