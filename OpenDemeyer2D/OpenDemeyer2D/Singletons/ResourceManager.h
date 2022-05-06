#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <thread>
#include <future>
#include <mutex>
#include <functional>
#include <deque>
#include <SDL.h>
#include <cassert>
#include <iostream>

#include "UtilityFiles/Singleton.h"
#define RESOURCES ResourceManager::GetInstance()

class Texture2D;
class Font;
class RenderTarget;
class Surface2D;
class Sound;
class Music;

class ResourceManager final : public Singleton<ResourceManager>
{

	friend class Singleton<ResourceManager>;

protected:

	ResourceManager() = default;
	virtual ~ResourceManager()
	{
		m_TerminateLoaderThreads = true;
	};

public:

	void Init(const std::string& dataPath);

	std::shared_ptr<Texture2D> LoadTexture(const std::string& file);
	std::shared_ptr<Texture2D> LoadTexture(SDL_Surface* pSurface);
	std::shared_ptr<Texture2D> LoadTexture(int width, int height);

	std::shared_ptr<Font> LoadFont(const std::string& file, unsigned int size);

	std::shared_ptr<RenderTarget> CreateRenderTexture(int width, int height);

	std::shared_ptr<Surface2D> LoadSurface(const std::string& file);
	std::shared_ptr<Surface2D> LoadSurface(int width, int height);

	std::shared_ptr<Sound> LoadSound(const std::string& file);
	std::shared_ptr<Music> LoadMusic(const std::string& file);

	template <typename Resource>
	std::shared_ptr<void> LoadResource(const std::string& resourcePath)
	{

		// find loader
		auto loader = m_ResourceLoaders.find(typeid(Resource));
		if (loader == m_ResourceLoaders.end())
		{
			// return if no loader is found
			assert(true);
			return nullptr;
		}

		// Don't load the same Resource if it is already loaded
		auto& resources = m_LoadedResources[typeid(Resource)];
		auto resourceIt = resources.find(resourcePath);
		if (resourceIt != resources.end())
		{
			if (!resourceIt->second.expired())
			{
				return resourceIt->second.lock();
			}
		}

		{
#ifdef _DEBUG
			std::cout << "Started loading " << typeid(Resource).name() << resourcePath << '\n';
#endif
			// Load the resource and lock the resource loading mechanism
			auto locktest = m_ResourceLoaderLocks[typeid(Resource)];
			std::scoped_lock<std::mutex> lock(*m_ResourceLoaderLocks[typeid(Resource)]);


			// Load the resource if it isn't loaded already
			const auto fullPath = m_DataPath + resourcePath;
			auto sharedResource = std::static_pointer_cast<Resource>((loader->second)(fullPath));
			resources.insert({ resourcePath, sharedResource });
		
#ifdef _DEBUG
			std::cout << "Finished loading " << typeid(Resource).name() << resourcePath << '\n';
#endif

			return sharedResource;
		}
	}

	template <typename Resource>
	std::future<std::shared_ptr<void>> LoadResourceAsync(const std::string& resourcePath)
	{
		// find loader
		auto loader = m_ResourceLoaders.find(typeid(Resource));
		if (loader == m_ResourceLoaders.end())
		{
			// return if no loader is found
			assert(true);

			auto promise = std::promise<std::shared_ptr<void>>();
			promise.set_value(nullptr);
			return promise.get_future();
		}

#ifdef _DEBUG
		std::cout << "Started async loading " << typeid(Resource).name() << resourcePath << '\n';
#endif

		// start thread if it hasn't already started
		auto threadIt = m_ResourceLoaderThreads.find(typeid(Resource));
		if (threadIt == m_ResourceLoaderThreads.end())
			m_ResourceLoaderThreads.insert({ typeid(Resource),std::jthread(&ResourceManager::LoaderThread<Resource>, this) });

		{
			std::scoped_lock<std::mutex> lock(*m_ResourceFunctionQueueLocks[typeid(Resource)]);

			auto functionStack = m_ResourceFunctionStacks[typeid(Resource)];
			std::pair<ResourcePromise, ResourceLoaderFunction> queueEntry;
			queueEntry.first = ResourcePromise(new std::promise<std::shared_ptr<void>>);
			queueEntry.second = std::bind(&ResourceManager::LoadResource<Resource>, this, resourcePath);
			functionStack->emplace_back(queueEntry);

#ifdef _DEBUG
			std::cout << "Finished async loading " << typeid(Resource).name() << resourcePath << '\n';
#endif

			return functionStack->back().first->get_future();
		}
	}

public:

	template <typename Resource>
	void LoaderThread()
	{
		using namespace std::chrono_literals;

		ResourcePromiseLoaderQueue ResourceFunctionsStack = m_ResourceFunctionStacks[typeid(Resource)];
		std::shared_ptr<std::mutex> resourceFunctionsStacksLock = m_ResourceFunctionQueueLocks[typeid(Resource)];
		//std::shared_ptr<std::mutex> resourceLoaderLock = m_ResourceLoaderLocks[typeid(Resource)];

		bool* terminate{ &m_TerminateLoaderThreads };
		std::condition_variable threadIdle;
		std::mutex waitLock;
		std::unique_lock<std::mutex> uniqueWaitlock(waitLock);
		while (true)
		{
			threadIdle.wait_for(uniqueWaitlock, 500ms ,[&terminate, &ResourceFunctionsStack] 
				{ 
					return *terminate || !ResourceFunctionsStack->empty(); 
				});

			// terminate the thread if terminateLoaderThreads is true
			if (*terminate)
				return;

			if (ResourceFunctionsStack->empty())
				continue;

			// get the loader function
			std::pair<ResourcePromise, ResourceLoaderFunction> loaderFunction;
			{
				std::scoped_lock<std::mutex> lock(*resourceFunctionsStacksLock);
				loaderFunction = ResourceFunctionsStack->front();
				ResourceFunctionsStack->pop_front();
			}

			// Fill the promise
			loaderFunction.first->set_value(loaderFunction.second());
		}
	}

	template <typename Resource>
	void PopulateResourceLocks()
	{
		m_ResourceFunctionQueueLocks.insert({ typeid(Resource), std::shared_ptr<std::mutex>(new std::mutex()) });
		m_ResourceLoaderLocks.insert({ typeid(Resource), std::shared_ptr<std::mutex>(new std::mutex()) });
		m_ResourceFunctionStacks.insert({ typeid(Resource), std::shared_ptr<std::deque<std::pair<ResourcePromise, ResourceLoaderFunction>>>(new std::deque<std::pair<ResourcePromise, ResourceLoaderFunction>>()) });
	}

private:

	typedef std::function<std::shared_ptr<void>(const std::string&)> ResourceFileLoaderFunction;
	typedef std::function<std::shared_ptr<void>()> ResourceLoaderFunction;
	typedef std::shared_ptr<std::promise<std::shared_ptr<void>>> ResourcePromise;
	typedef std::shared_ptr<std::deque<std::pair<ResourcePromise, ResourceLoaderFunction>>> ResourcePromiseLoaderQueue;

	std::unordered_map<std::type_index, std::unordered_map<std::string, std::weak_ptr<void>>> m_LoadedResources;
	std::unordered_map<std::type_index, ResourceFileLoaderFunction> m_ResourceLoaders;
	
	std::unordered_map<std::type_index, std::shared_ptr<std::mutex>> m_ResourceLoaderLocks;
	std::unordered_map<std::type_index, std::jthread> m_ResourceLoaderThreads;
	std::unordered_map<std::type_index, ResourcePromiseLoaderQueue> m_ResourceFunctionStacks;
	std::unordered_map<std::type_index, std::shared_ptr<std::mutex>> m_ResourceFunctionQueueLocks;
	
	bool m_TerminateLoaderThreads{};

	std::string m_DataPath;

	// TODO change this into a unordered_map. needs a special hash funtion
	std::map<std::pair<std::string, unsigned int>, std::weak_ptr<Font>> m_LoadedFonts;
};
