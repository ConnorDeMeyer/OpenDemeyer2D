#pragma once
#include "../ComponentBase.h"
#include <glm/vec2.hpp>

class Transform final : public ComponentBase
{
public:

	Transform() = default;
	virtual ~Transform() = default;

public:

	const glm::vec2& GetPosition() const { return m_Position; }
	glm::vec2& GetPosition() { return m_Position; }

	/** Sets the position of the transform and change the position of the children of the parent*/
	void SetPosition(const glm::vec2& pos);

	const glm::vec2& GetScale() const { return m_Scale; }
	glm::vec2& GetScale() { return m_Scale; }

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

private:

	glm::vec2 m_Position;
	glm::vec2 m_Scale;

};



