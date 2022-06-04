#pragma once

#include <functional>
#include <vector>

class State;

class Transition final
{
public:
	Transition(const std::function<bool()>& transitionCheck, State* pNextState)
		: m_TransitionCheck{ transitionCheck }
		, m_pToState{ pNextState }
	{}

	Transition(std::function<bool()>&& transitionCheck, State* pNextState)
		: m_TransitionCheck{ transitionCheck }
		, m_pToState{ pNextState }
	{}

	~Transition()								= default;
	Transition(const Transition&)				= default;
	Transition(Transition&&)					= default;
	Transition& operator=(const Transition&)	= default;
	Transition& operator=(Transition&&)			= default;

public:

	inline void SetTransition(const std::function<bool()>& transitionFunction)
	{
		m_TransitionCheck = transitionFunction;
	}

	inline void SetNextState(State* pState)
	{
		m_pToState = pState;
	}

	inline bool CanTransition() const { return m_TransitionCheck(); }
	inline State* GetNextState() const { return m_pToState; }

private:
	std::function<bool()> m_TransitionCheck;
	State* m_pToState;
};

class State final
{
public:

	State() = default;

	State(const std::initializer_list<Transition>& list)
		: m_Transitions{list}
	{}

	State(const Transition& transition)
		: m_Transitions{ transition }
	{}

	State(const std::function<bool()>& transitionFunction, State* pNextState)
		: m_Transitions{ Transition{transitionFunction,pNextState} }
	{}

	~State()						= default;
	State(const State&)				= default;
	State(State&&)					= default;
	State& operator=(const State&)	= default;
	State& operator=(State&&)		= default;

public:

	inline const std::vector<Transition>& GetTransitions() const { return m_Transitions; }

	/** Returns itself if it wont transition*/
	inline State* GetNextState()
	{
		for (auto& transition : m_Transitions)
		{
			if (transition.CanTransition())
				return transition.GetNextState();
		}
		return this;
	}

	/**This will be called every time the it checks for transitions*/
	inline void SetUpdateFunction(const std::function<void(float)>& UpdateFunction)
	{
		m_UpdateFunction = UpdateFunction;
	}

	inline void Update(float deltaTime) const
	{
		m_UpdateFunction(deltaTime);
	}

	inline void AddTransition(const Transition& transition)
	{
		m_Transitions.emplace_back(transition);
	}

	inline void AddTransition(Transition&& transition)
	{
		m_Transitions.emplace_back(transition);
	}

private:
	std::vector<Transition> m_Transitions;
	std::function<void(float)> m_UpdateFunction;
};

class StateMachine final
{
public:
	StateMachine() = default;

	~StateMachine()
	{
		for (auto state : m_States)
			delete state;
	}

	StateMachine(const StateMachine&)				= delete;
	StateMachine(StateMachine&&)					= default;
	StateMachine& operator=(const StateMachine&)	= delete;
	StateMachine& operator=(StateMachine&&)			= default;

public:

	inline void AddState(State* pState)
	{
		m_States.emplace_back(pState);
	}

	inline void RemoveState(State* pState)
	{
		m_States.erase(std::remove(m_States.begin(), m_States.end(), pState), m_States.end());
	}

	inline void SetCurrentState(State* pState)
	{
		auto it = std::find(m_States.begin(), m_States.end(), pState);
		if (it == m_States.end())
		{
			m_States.emplace_back(pState);
		}
		m_pCurrentState = pState;
	}

	inline void Update(float deltaTime)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Update(deltaTime);
			if (((m_AccumulatedTime -= deltaTime) <= 0))
			{
				auto nextState = m_pCurrentState->GetNextState();
				if (nextState != m_pCurrentState)
				{
					m_AccumulatedTime = m_TransitionDelay;
					m_pCurrentState = nextState;
				}
			}
		}
	}

	/** Sets a delay after every transition before checking transitions again*/
	inline void SetTransitionDelay(float delayTime) { m_TransitionDelay = delayTime; }

private:

	std::vector<State*> m_States{};
	State* m_pCurrentState{};

	float m_TransitionDelay{};
	float m_AccumulatedTime{};
};