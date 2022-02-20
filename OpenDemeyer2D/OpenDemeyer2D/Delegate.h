#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "GameObject.h"

template<typename... inputParameterTypes>
class Delegate
{
private:

	struct BindedFunction
	{
		std::function<void(inputParameterTypes...)> function;
		std::function<bool()> validPredicate;
	};

public:

	Delegate() = default;
	virtual ~Delegate()
	{
		for (auto bindedEvent : m_BindedEvents)
			delete bindedEvent;
	}

	Delegate(const Delegate<inputParameterTypes...>& other)				= default;
	Delegate(Delegate<inputParameterTypes...>&& other)					= default;
	Delegate& operator=(const Delegate<inputParameterTypes...>& other)	= default;
	Delegate& operator=(Delegate<inputParameterTypes...>&& other)		= default;

	/** bind a function that will be invalidated when the IBaseClass instance goes out of scope */
	void BindFunction(GameObject* boundObject, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundObject)
		{
			auto weakPtr = boundObject->GetWeakReference();
			m_BindedEvents.push_back(new BindedFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				});
		}
	}

	/** bind a function and a predicate that should return false if the function is invalidated */
	void BindFunction(const std::function<bool()>& ValidPredicate, const std::function<void(inputParameterTypes...)>& function)
	{
		m_BindedEvents.push_back(new BindedFunction{
				function,
				ValidPredicate });
	}

	/** bind a global function that should never get invalidated */
	void BindFunction(const std::function<void(inputParameterTypes...)>& function)
	{
		m_BindedEvents.push_back(new BindedFunction{
				function,
				[]() {return true; } });
	}

	/** Activate all the bound functions*/
	void BroadCast(const inputParameterTypes&... input...)
	{
		auto it = m_BindedEvents.rbegin();
		while (it != m_BindedEvents.rend())
		{
			if ((*it)->validPredicate())
			{
				(*it)->function(input...);
			}
			else
			{
				m_BindedEvents.remove(*it);
			}
			if (it != m_BindedEvents.rend())++it;
			else break;
		}
	}

	void RemoveAllFunctions()
	{
		m_BindedEvents.clear();
	}

private:

	std::list<BindedFunction*> m_BindedEvents;

};