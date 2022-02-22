#pragma once
#include "../ComponentBase.h"
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>

class Transform final : public ComponentBase
{
public:

	Transform() = default;
	virtual ~Transform() = default;

public:

	void BeginPlay() override;

	void RenderImGui() override;

	const glm::vec2& GetPosition();

	/** Sets the position of the transform and change the position of the children of the parent*/
	void SetPosition(const glm::vec2& pos);

	const glm::vec2& GetScale();

	/** Sets the scale of the transform and change the scale of the children of the parent*/
	void SetScale(const glm::vec2& scale);

	/** Move the transform by the distance given
	 * Will also move all the children game objects
	 */
	void Move(const glm::vec2& distance);

	/** Scale the transform by the distance given
	 * Will also scale all the children game objects
	 */
	void Scale(const glm::vec2& scalar);
	void Scale(float scale);

	/** Add a value to the scale value*/
	void AddScale(const glm::vec2& scale);
	void AddScale(float scale);

	void SetRotation(float rotation);

	void Rotate(float rotation);

	float GetRotation();

	void ApplyMatrix(const glm::mat3x3& matrix);

	void UpdateValues();

private:

	glm::vec2 m_Position{0,0};
	glm::vec2 m_Scale{1,1};
	float m_Rotation{ 0 };

	glm::mat3x3 m_BaseTransformation{
		1,0,0,
		0,1,0,
		0,0,1
	};

	glm::mat3x3 m_Transformation{
		1,0,0,
		0,1,0,
		0,0,1
	};

	enum class TransformChanges : uint8_t
	{
		none = 0b0,
		position = 0b1,
		scale = 0b10,
		rotation = 0b100,
		all = 0b111
	};

	uint8_t m_Changes{};

};

glm::vec2 GetPosFromMat(const glm::mat3x3& matrix);

glm::vec2 GetScaleFromMat(const glm::mat3x3& matrix);

float GetRotationFromMat(const glm::mat3x3& matrix);

glm::mat3x3 TranslationMatrix(glm::vec2 translation);

glm::mat3x3 ScaleMatrix(glm::vec2 scale);

glm::mat3x3 RotationMatrix(float rotation);