#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "GameObject.h"
#include "ComponentBase.h"

/**
 * A Multi Delegate is an object where multiple functions can be bound to.
 * Bound functions may be called all at once with the BroadCast function.
 * When binding a function you are able to provide a predicate to to check whether the bound function pointer is still valid.
 */
template<typename... inputParameterTypes>
class Delegate final
{
private:

	struct BoundFunction
	{
		std::function<void(inputParameterTypes...)> function;
		std::function<bool()> validPredicate;
	};

public:

	Delegate() = default;
	virtual ~Delegate()
	{
		for (auto BoundEvent : m_BoundEvents)
			delete BoundEvent;
	}

	Delegate(const Delegate<inputParameterTypes...>& other)				= delete;
	Delegate(Delegate<inputParameterTypes...>&& other)					= delete;
	Delegate& operator=(const Delegate<inputParameterTypes...>& other)	= delete;
	Delegate& operator=(Delegate<inputParameterTypes...>&& other)			= delete;

	/** Bind a function that will be invalidated when the Component instance goes out of scope */
	void BindFunction(const ComponentBase * boundComponent, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundComponent)
		{
			auto weakPtr = boundComponent->GetWeakReference();
			m_BoundEvents.emplace_back(new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				});
		}
	}

	/** Bind a function that will be invalidated when the GameObject instance goes out of scope */
	void BindFunction(const GameObject* boundObject, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundObject)
		{
			auto weakPtr = boundObject->GetWeakReference();
			m_BoundEvents.emplace_back(new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				});
		}
	}

	/** Bind a function and a predicate that should return false if the function is invalidated */
	void BindFunction(const std::function<bool()>& ValidPredicate, const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.emplace_back(new BoundFunction{
				function,
				ValidPredicate });
	}

	/** Bind a global function that should never get invalidated */
	void BindFunction(const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.emplace_back(new BoundFunction{
				function,
				[]() {return true; } });
	}

	/** Call all the bound functions*/
	void BroadCast(const inputParameterTypes&... input...)
	{
		m_BoundEvents.remove_if([](BoundFunction* function) {return !function->validPredicate(); });
		for (BoundFunction* Boundfunction : m_BoundEvents)
		{
			Boundfunction->function(input...);
		}
	}

	/** Call all bound functions using a tuple as parameters*/
	void BroadCast(const std::tuple<inputParameterTypes...>& parameters)
	{
		m_BoundEvents.remove_if([](BoundFunction* function) {return !function->validPredicate(); });
		for (BoundFunction* Boundfunction : m_BoundEvents)
		{
			std::apply(Boundfunction->function, parameters);
		}
	}

	/** Removes all functions from the call list*/
	void RemoveAllFunctions()
	{
		m_BoundEvents.clear();
	}

private:

	std::list<BoundFunction*> m_BoundEvents;

};