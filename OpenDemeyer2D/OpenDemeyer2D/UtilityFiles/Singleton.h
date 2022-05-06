#pragma once

template <typename T>
class Singleton
{
public:
	static T& GetInstance()
	{
		static T instance{};
		return instance;
	}

protected:

	virtual ~Singleton()							= default;
	Singleton(const Singleton& other)				= delete;
	Singleton(Singleton&& other)					= delete;
	Singleton& operator=(const Singleton& other)	= delete;
	Singleton& operator=(Singleton&& other)			= delete;

protected:
	
	Singleton() = default;
};