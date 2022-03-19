﻿#pragma once
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

	static void AddMessage(const EventParameters&... eventParameters)
	{
		m_InputQueue.push_back(std::tuple<EventParameters...>(eventParameters...));
	}

	static const std::tuple<EventParameters...>& GetMessage()
	{
		auto message = m_InputQueue.front();
		m_InputQueue.pop_front();
		return message;
	}

private:

	static std::deque<std::tuple<EventParameters...>> m_InputQueue;

};

template<typename... EventParameters>
std::deque<std::tuple<EventParameters...>> EventQueue<EventParameters...>::m_InputQueue{};