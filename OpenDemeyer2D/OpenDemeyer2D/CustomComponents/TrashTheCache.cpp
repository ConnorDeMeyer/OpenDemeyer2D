#include "TrashTheCache.h"
#include "imgui.h"

void TrashTheCache::RenderImGui()
{
	ImGui::InputInt("# Samples", &m_Samples, 1, 2);

	if (ImGui::Button("Trash the cache"))
	{
		Trash<int>(m_TimeValuesInts, m_Samples);
	}

	if (!m_TimeValuesInts.empty())
	{
		ImGui::PlotLines("Integer",
			m_TimeValuesInts.data(),
			m_TimeValuesInts.size(),
			0,
			nullptr,
			0,
			m_TimeValuesInts[0],
			ImVec2(0, 80)
		);
	}

	if (ImGui::Button("Trash the cache with GameObject3D"))
	{
		Trash<GameObject3D>(m_TimeValuesGameObject3D, m_Samples);
	}

	if (!m_TimeValuesGameObject3D.empty())
	{
		ImGui::PlotLines("GameObject3D",
			m_TimeValuesGameObject3D.data(),
			m_TimeValuesGameObject3D.size(),
			0,
			nullptr,
			0,
			m_TimeValuesGameObject3D[0],
			ImVec2(0, 80)
		);
	}

	if (ImGui::Button("Trash the cache with GameObject3DAlt"))
	{
		Trash<GameObject3DAlt>(m_TimeValuesGameObject3DAlt, m_Samples);
	}

	if (!m_TimeValuesGameObject3DAlt.empty())
	{
		ImGui::PlotLines("GameObject3DAlt",
			m_TimeValuesGameObject3DAlt.data(),
			m_TimeValuesGameObject3DAlt.size(),
			0,
			nullptr,
			0,
			m_TimeValuesGameObject3DAlt[0],
			ImVec2(0, 80)
		);
	}

	ImGui::Text("Combined: (GameObject3DAlt with the scale of GameObject3D)");

	if (!m_TimeValuesGameObject3DAlt.empty() && !m_TimeValuesGameObject3D.empty())
	{

		ImGui::PlotLines("",
			m_TimeValuesGameObject3DAlt.data(),
			m_TimeValuesGameObject3DAlt.size(),
			0,
			nullptr,
			0,
			m_TimeValuesGameObject3D[0],
			ImVec2(0, 80));
	}

}
