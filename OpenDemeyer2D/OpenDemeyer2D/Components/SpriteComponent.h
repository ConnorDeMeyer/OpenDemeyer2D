#pragma once
#include "EngineFiles/Component.h"
#include "ResourceWrappers/Texture2D.h"
#include <glm/glm.hpp>
#include "UtilityFiles/Delegate.h"

class RenderComponent;

class SpriteComponent : public Component<SpriteComponent>
{
public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Initialize() override;

	void Update(float) override;

	void RenderImGui() override;

	void SetFrameDimension(const glm::vec2& dimension);

	/**
	* Sets the position of the first frame in the bitmap
	* useful for when you want to skip frames
	*/
	void SetFrameOffset(int offset);

	void SetTotalFrames(int amount);

	void SetTimePerFrame(float value);

	void SetCurrentFrame(int frame);

	void Reset();

	void SetPaused(bool isPaused) { m_bPauseTime = isPaused; }

	void SetLooping(bool isLooping) { m_bLoop = isLooping; }

	Delegate<> OnAnimationEnd;

private:

	void UpdateSourceRect();

private:

	glm::vec2 m_FrameDimension{ 32.f, 32.f };
	int m_FrameOffset{};

	float m_TimePerFrame{0.25f};
	float m_AccumulatedTime{};

	int m_CurrentFrame{};
	int m_TotalFrames{ 1 };

	int m_CurrentFrameX{};
	int m_CurrentFrameY{};

	bool m_bPauseTime{};
	bool m_bLoop{ true };

	bool m_NeedsUpdate{};

};
