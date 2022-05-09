#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

class State;

class Transition
{
public:
	virtual bool TransitionCheck() const = 0;
	State* toState;
};

class State
{
public:


private:
	std::vector<Transition> m_Transitions;
};

class StateMachine
{

};