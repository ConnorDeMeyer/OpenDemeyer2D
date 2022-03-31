#pragma once
#include "../ComponentBase.h"
#include "../Texture2D.h"
#include <glm/glm.hpp>
#include "../Delegate.h"

class RenderComponent;

class SpriteComponent : public ComponentBase
{
public:

	void BeginPlay() override;

	void Update(float) override;

	void SetTexture(const std::shared_ptr<Texture2D>& texture);

	void SetTexture(const std::string& filePath);

	void RenderImGui() override;

	void SetFrameDimension(const glm::vec2& dimension) { m_FrameDimension = dimension; }

	/**
	* Sets the position of the first frame in the bitmap
	* useful for when you want to skip frames
	*/
	void SetFrameOffset(int offset);

	void SetTotalFrames(int amount) { m_TotalFrames = amount; }

	void SetTimePerFrame(float value) { m_TimePerFrame = value; }

	void SetCurrentFrame(int frame);

	void Reset();

	void SetPaused(bool isPaused) { m_bPauseTime = isPaused; }

	void SetLooping(bool isLooping) { m_bLoop = isLooping; }

	Delegate<> OnAnimationEnd;

	const std::string GetComponentName() override { return "SpriteComponent"; }

private:

	std::shared_ptr<Texture2D> m_Texture;

	RenderComponent* m_pRenderComponent{};

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

};
