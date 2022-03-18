#pragma once
#include <vector>
#include <functional>
#include <tuple>
#include <deque>
#include "Delegate.h"

template <typename... EventParameters>
class EventQueue final
{
public:
	EventQueue() = default;
	virtual ~EventQueue() = default;
	EventQueue(const EventQueue& other) = delete;
	EventQueue(EventQueue&& other) noexcept = delete;
	EventQueue& operator=(const EventQueue& other) = delete;
	EventQueue& operator=(EventQueue&& other) noexcept = delete;

	static MultiDelegate<EventParameters...>& Listeners()
	{
		return m_BoundListener;
	}

	static void AddMessage(const EventParameters&... eventParameters)
	{
		m_InputQueue.push_back(std::tuple<EventParameters...>(eventParameters...));

		if (m_Active) return;
		m_Active = true;

		while (!m_InputQueue.empty())
		{
			for (auto& listener : m_BoundListener)
			{
				listener.BroadCast(m_InputQueue.front());
			}

			m_InputQueue.pop_front();
		}

		m_Active = false;
	}

private:

	static std::vector<MultiDelegate<EventParameters...>> m_BoundListener;

	static std::deque<std::tuple<EventParameters...>> m_InputQueue;

	static bool m_Active;

};

template<typename... EventParameters>
std::vector<MultiDelegate<EventParameters...>> EventQueue<EventParameters...>::m_BoundListener{};

template<typename... EventParameters>
std::deque<std::tuple<EventParameters...>> EventQueue<EventParameters...>::m_InputQueue{};

template<typename... EventParameters>
bool EventQueue<EventParameters...>::m_Active{};
