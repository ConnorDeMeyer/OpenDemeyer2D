#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "EngineFiles/GameObject.h"
#include "EngineFiles/ComponentBase.h"

/**
 * A Delegate is an object where multiple functions can be bound to.
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
	~Delegate()
	{
		m_BoundEvents.clear();
	}

	Delegate(const Delegate<inputParameterTypes...>& other)				= delete;
	Delegate(Delegate<inputParameterTypes...>&& other)					= delete;
	Delegate& operator=(const Delegate<inputParameterTypes...>& other)	= delete;
	Delegate& operator=(Delegate<inputParameterTypes...>&& other)		= delete;

	/** Bind a function that will be invalidated when the Component instance goes out of scope */
	inline void BindFunction(const ComponentBase * boundComponent, const std::function<void(inputParameterTypes...)>& function)
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
	inline void BindFunction(const GameObject* boundObject, const std::function<void(inputParameterTypes...)>& function)
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
	inline void BindFunction(const std::function<bool()>& ValidPredicate, const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.emplace_back(new BoundFunction{
				function,
				ValidPredicate });
	}

	/** Bind a global function that should never get invalidated */
	inline void BindFunction(const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.emplace_back(new BoundFunction{
				function,
				[]() {return true; } });
	}

	/** Call all the bound functions*/
	inline void BroadCast(const inputParameterTypes&... input...)
	{
		RemoveInvalidFunctions();		
		for (auto& Boundfunction : m_BoundEvents)
		{
			Boundfunction->function(input...);
		}
	}

	/** Call all bound functions using a tuple as parameters*/
	inline void BroadCast(const std::tuple<inputParameterTypes...>& parameters)
	{
		RemoveInvalidFunctions();
		for (auto& Boundfunction : m_BoundEvents)
		{
			std::apply(Boundfunction->function, parameters);
		}
	}

	/** 
	* Removes the functions that are invalid by checking their valid predicate.
	* This gets called every broadcast.
	*/
	inline void RemoveInvalidFunctions()
	{
		m_BoundEvents.erase(
			std::remove_if(m_BoundEvents.begin(), m_BoundEvents.end(), [](const std::unique_ptr<BoundFunction>& function) 
			{
				return !function->validPredicate(); 
			}),m_BoundEvents.end());
	}

	/** Removes all functions from the call list*/
	inline void RemoveAllFunctions()
	{
		m_BoundEvents.clear();
	}

private:

	std::vector<std::unique_ptr<BoundFunction>> m_BoundEvents;

};