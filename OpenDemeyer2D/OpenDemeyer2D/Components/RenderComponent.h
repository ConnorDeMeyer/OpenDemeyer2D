#pragma once
#include "Component.h"
#include "../Texture2D.h"
#include "../RenderManager.h"

class Transform;

class RenderComponent final : public Component<RenderComponent>
{
public:

	RenderComponent() = default;
	virtual ~RenderComponent() = default;

public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Render() const;

	void BeginPlay() override;

	void SetTexture(std::shared_ptr<Texture2D> texture);

	void SetRenderAlignMode(eRenderAlignMode mode);

	void SetSourceRect(const SDL_FRect& srcRect);

	//const std::string GetComponentName() override { return "RenderComponent"; }

	void RenderImGui() override;

	void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }

	void SetRenderLayer(int renderLayer) { m_RenderLayer = renderLayer; }

	int GetRenderLayer() const { return m_RenderLayer; }

private:

	std::shared_ptr<Texture2D> m_Texture;

	Transform* m_pTransform;

	SDL_FRect m_SourceRect;

	glm::vec2 m_Pivot{};

	int m_RenderLayer{};

};