#pragma once
#include "EngineFiles/ComponentBase.h"
#include "ResourceWrappers/Texture2D.h"
#include <glm/glm.hpp>
#include "UtilityFiles/Delegate.h"

class RenderComponent;

class SpriteComponent : public ComponentBase
{
	COMPONENT_BODY(SpriteComponent)

public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Initialize() override;

	void Update(float) override;

	void RenderImGui() override;

	void SetFrameDimension(const glm::vec2& dimension);
	const glm::vec2& GetFrameDimension() const { return m_FrameDimension; }

	/**
	* Sets the position of the first frame in the bitmap
	* useful for when you want to skip frames
	*/
	void SetFrameOffset(int offset);
	int GetFrameOffset() { return m_FrameOffset; }

	void SetTotalFrames(int amount);
	int GetTotalFrames() const { return m_TotalFrames; }

	void SetTimePerFrame(float value);
	float GetTimePerFrame() const { return m_TimePerFrame; }

	void SetCurrentFrame(int frame);
	int GetCurrentFrame() const { return m_CurrentFrame; }

	void Reset();

	void SetPaused(bool isPaused) { m_bPauseTime = isPaused; }
	bool IsPaused() const { return m_bPauseTime; }

	/** 
	* Set the looping mode.
	* Does not Resume the animation if it had reached the end. use the SetPaused Method
	*/
	void SetLooping(bool isLooping) { m_bLoop = isLooping; }
	bool IsLooping() const { return m_bLoop; }

	/** Gets broadcasted every time the animation stops or loops back*/
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
