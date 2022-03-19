#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "GameObject.h"
#include "ComponentBase.h"

template<typename... inputParameterTypes>
class MultiDelegate final
{
private:

	struct BoundFunction
	{
		std::function<void(inputParameterTypes...)> function;
		std::function<bool()> validPredicate;
	};

public:

	MultiDelegate() = default;
	virtual ~MultiDelegate()
	{
		for (auto BoundEvent : m_BoundEvents)
			delete BoundEvent;
	}

	MultiDelegate(const MultiDelegate<inputParameterTypes...>& other)				= default;
	MultiDelegate(MultiDelegate<inputParameterTypes...>&& other)					= default;
	MultiDelegate& operator=(const MultiDelegate<inputParameterTypes...>& other)	= default;
	MultiDelegate& operator=(MultiDelegate<inputParameterTypes...>&& other)			= default;

	/** bind a function that will be invalidated when the GameObject instance goes out of scope */
	void BindFunction(const GameObject* boundObject, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundObject)
		{
			auto weakPtr = boundObject->GetWeakReference();
			m_BoundEvents.push_back(new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				});
		}
	}

	/** bind a function that will be invalidated when the Component instance goes out of scope */
	void BindFunction(const ComponentBase* boundComponent, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundComponent)
		{
			auto weakPtr = boundComponent->GetWeakReference();
			m_BoundEvents.push_back(new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				});
		}
	}

	/** bind a function and a predicate that should return false if the function is invalidated */
	void BindFunction(const std::function<bool()>& ValidPredicate, const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.push_back(new BoundFunction{
				function,
				ValidPredicate });
	}

	/** bind a global function that should never get invalidated */
	void BindFunction(const std::function<void(inputParameterTypes...)>& function)
	{
		m_BoundEvents.push_back(new BoundFunction{
				function,
				[]() {return true; } });
	}

	/** Activate all the bound functions*/
	void BroadCast(const inputParameterTypes&... input...)
	{
		m_BoundEvents.remove_if([](BoundFunction* function) {return !function->validPredicate(); });
		for (BoundFunction* Boundfunction : m_BoundEvents)
		{
			Boundfunction->function(input...);
		}

		/*auto it = m_BoundEvents.rbegin();
		while (it != m_BoundEvents.rend())
		{
			if ((*it)->validPredicate())
			{
				(*it)->function(input...);
			}
			else
			{
				m_BoundEvents.remove(*it);
			}
			if (it != m_BoundEvents.rend())++it;
			else break;
		}*/
	}

	void BroadCast(const std::tuple<inputParameterTypes...>& parameters)
	{
		m_BoundEvents.remove_if([](BoundFunction* function) {return !function->validPredicate(); });
		for (BoundFunction* Boundfunction : m_BoundEvents)
		{
			std::apply(Boundfunction->function, parameters);
		}

		/*auto it = m_BoundEvents.rbegin();
		while (it != m_BoundEvents.rend())
		{
			if ((*it)->validPredicate())
			{
				std::apply((*it)->function, parameters);
			}
			else
			{
				m_BoundEvents.erase(*it);
			}
			if (it != m_BoundEvents.rend())++it;
			else break;
		}*/
	}

	void RemoveAllFunctions()
	{
		m_BoundEvents.clear();
	}

private:

	std::list<BoundFunction*> m_BoundEvents;

};

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
	virtual ~Delegate() = default;

	Delegate(const Delegate<inputParameterTypes...>& other) = default;
	Delegate(Delegate<inputParameterTypes...>&& other) = default;
	Delegate& operator=(const Delegate<inputParameterTypes...>& other) = default;
	Delegate& operator=(Delegate<inputParameterTypes...>&& other) = default;

	/** bind a function that will be invalidated when the IBaseClass instance goes out of scope */
	void BindFunction(const GameObject* boundObject, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundObject)
		{
			auto weakPtr = boundObject->GetWeakReference();
			m_pBoundEvent = new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
				};
		}
	}

	/** bind a function that will be invalidated when the Component instance goes out of scope */
	void BindFunction(const ComponentBase* boundComponent, const std::function<void(inputParameterTypes...)>& function)
	{
		if (boundComponent)
		{
			auto weakPtr = boundComponent->GetWeakReference();
			m_pBoundEvent = new BoundFunction{
				function,
				[weakPtr]() {return !weakPtr.expired(); }
			};
		}
	}

	/** bind a function and a predicate that should return false if the function is invalidated */
	void BindFunction(const std::function<bool()>& ValidPredicate, const std::function<void(inputParameterTypes...)>& function)
	{
		m_pBoundEvent = new BoundFunction{
				function,
				ValidPredicate };
	}

	/** bind a global function that should never get invalidated */
	void BindFunction(const std::function<void(inputParameterTypes...)>& function)
	{
		m_pBoundEvent = new BoundFunction(
			function,
			[]() {return true; });
	}

	/** Activate all the bound function*/
	void Execute(const inputParameterTypes&... input...)
	{
		if (m_pBoundEvent && m_pBoundEvent->validPredicate()) *m_pBoundEvent->function(input...);
	}

	void Execute(const std::tuple<inputParameterTypes...>& input)
	{
		if (m_pBoundEvent && m_pBoundEvent->validPredicate()) std::apply(*m_pBoundEvent->function, input);
	}

	void RemoveFunction()
	{
		m_pBoundEvent = nullptr;
	}

private:

	std::unique_ptr<BoundFunction> m_pBoundEvent{};

};