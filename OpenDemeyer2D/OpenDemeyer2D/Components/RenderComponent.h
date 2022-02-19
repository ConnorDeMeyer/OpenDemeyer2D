#pragma once
#include "../ComponentBase.h"
#include "../Texture2D.h"

class Transform;

enum class RenderAlignMode : Uint8
{
	topLeft = 0b00,
	topRight = 0b01,
	bottomLeft = 0b10,
	bottomRight = 0b11,
	center
};

class RenderComponent final : public ComponentBase
{
public:

	RenderComponent() = default;
	virtual ~RenderComponent() = default;

public:

	void Render() const override;

	void BeginPlay() override;

	void SetTexture(std::shared_ptr<Texture2D> texture) { m_Texture = texture; }

	void SetRenderAlignMode(RenderAlignMode mode) { m_RenderAlignMode = mode; }

private:

	std::shared_ptr<Texture2D> m_Texture;

	Transform* m_pTransform;

	RenderAlignMode m_RenderAlignMode;

};



