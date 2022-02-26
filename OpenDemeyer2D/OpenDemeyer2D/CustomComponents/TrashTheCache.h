#pragma once
#include "../ComponentBase.h"
#include <vector>
#include <chrono>
#include "glm/glm.hpp"
#include <string>

class TrashTheCache : public ComponentBase
{
public:

	TrashTheCache() = default;
	virtual ~TrashTheCache() = default;

	void RenderImGui() override;

	const std::string GetComponentName() override { return "TrashTheCache"; }

private:

	struct Transform
	{
		float matrix[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
	};

	class GameObject3D
	{
	public:
		Transform transform{};
		int ID{ 1 };

		GameObject3D& operator*=(int i)
		{
			ID *= i;
			return *this;
		}
	};

	class GameObject3DAlt
	{
	public:
		Transform* transform{};
		int ID{ 1 };

		GameObject3DAlt& operator*=(int i)
		{
			ID *= i;
			return *this;
		}
	};

	template<typename T>
	void Trash(std::vector<float>& timeValues, int samples)
	{
		const size_t ARRAY_SIZE{ 131'072 };

		timeValues.clear();

		std::vector<T> valueVector(ARRAY_SIZE, T{});

		const size_t m_MaxStep{ static_cast<size_t>(glm::pow(2, samples)) };
		for (size_t i{ 2 }; i <= m_MaxStep; i *= 2)
		{
			auto begin = std::chrono::high_resolution_clock::now();
			for (size_t j{}; j < valueVector.size(); j += i)
				valueVector[j] *= 2;
			auto end = std::chrono::high_resolution_clock::now();

			auto duration = (end - begin);
			timeValues.push_back(static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()));
		}
	}

	int m_Samples{10};

	std::vector<float> m_TimeValuesInts;

	std::vector<float> m_TimeValuesGameObject3D;

	std::vector<float> m_TimeValuesGameObject3DAlt;

};

