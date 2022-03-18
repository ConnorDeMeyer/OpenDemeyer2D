#pragma once
#include <functional>

struct ICommand
{
	ICommand() = default;
	virtual ~ICommand() = default;

	ICommand(const ICommand& other) = delete;
	ICommand(ICommand&& other) noexcept = delete;
	ICommand& operator=(const ICommand& other) = delete;
	ICommand& operator=(ICommand&& other) noexcept = delete;

	virtual void Execute() = 0;
	virtual void Undo() = 0;
};

struct DynamicCommand : public ICommand
{
	DynamicCommand()
		: ExecuteFunction([](){})
		, UndoFunction([](){})
		{}

	virtual ~DynamicCommand() = default;

	std::function<void()> ExecuteFunction;
	std::function<void()> UndoFunction;

	void Execute() override { ExecuteFunction(); }
	void Undo() override { UndoFunction(); }
};