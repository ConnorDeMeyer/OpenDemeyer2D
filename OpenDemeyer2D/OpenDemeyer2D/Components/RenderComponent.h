#pragma once
#include "EngineFiles/ComponentBase.h"
#include "ResourceWrappers/Texture2D.h"
#include "Singletons/RenderManager.h"

class Transform;

class RenderComponent final : public ComponentBase
{
	COMPONENT_BODY(RenderComponent)

public:

	RenderComponent() = default;
	virtual ~RenderComponent() = default;

public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Render() const;

	//void Initialize() override;

	void SetTexture(std::shared_ptr<Texture2D> texture);

	const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }

	void SetRenderAlignMode(eRenderAlignMode mode);

	void SetSourceRect(const SDL_FRect& srcRect);

	void RenderImGui() override;

	void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }

	void SetRenderLayer(int renderLayer) { m_RenderLayer = renderLayer; }

	int GetRenderLayer() const { return m_RenderLayer; }

	// array to 4 vec2 that will be filled with the vertex positions
	glm::vec2* GetWorldRect(glm::vec2* vertices4) const;

private:

	std::shared_ptr<Texture2D> m_Texture;

	SDL_FRect m_SourceRect;

	glm::vec2 m_Pivot{ 0.5,0.5 };

	int m_RenderLayer{};

};