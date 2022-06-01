#pragma once
#include "EngineFiles/ComponentBase.h"
#include "ResourceWrappers/Texture2D.h"
#include <string>

class RenderComponent;

class TextureComponent final : public ComponentBase
{
	COMPONENT_BODY(TextureComponent)

public:

	TextureComponent() = default;
	virtual ~TextureComponent() = default;

public:

	virtual void DefineUserFields(UserFieldBinder&) const;

	void Initialize() override;

	void SetTexture(const std::shared_ptr<Texture2D>& texture);

	void RenderImGui() override;

	const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }

private:

	void UpdateRenderComponent();

	std::shared_ptr<Texture2D> m_Texture;
};



