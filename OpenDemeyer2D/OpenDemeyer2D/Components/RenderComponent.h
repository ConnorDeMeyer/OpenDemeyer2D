#pragma once
#include "../ComponentBase.h"
#include "../Texture2D.h"

enum class eRenderAlignMode : Uint8;
class Transform;

class RenderComponent final : public ComponentBase
{
public:

	RenderComponent() = default;
	virtual ~RenderComponent() = default;

public:

	void Render() const override;

	void BeginPlay() override;

	void SetTexture(std::shared_ptr<Texture2D> texture);

	void SetRenderAlignMode(eRenderAlignMode mode) { m_RenderAlignMode = mode; }

	void SetSourceRect(const SDL_Rect& srcRect);

	const std::string GetComponentName() override { return "RenderComponent"; }

	void InitializeComponent(const Dictionary& dictionary) override;

	Dictionary& GetClassDefault() override;

private:

	std::shared_ptr<Texture2D> m_Texture;

	Transform* m_pTransform;

	eRenderAlignMode m_RenderAlignMode;

	SDL_Rect m_SourceRect;

};



