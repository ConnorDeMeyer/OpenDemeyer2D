#pragma once


#include "Allocators/NewDeleteOverride.h"
#include "Allocators/SmallObjectAllocator.h"
#include "Allocators/ObjectPoolAllocator.h"
#include "Allocators/Mallocator.h"

#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <map>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

#include <tuple>
#include <memory>
#include <chrono>
#include <thread>
#include <cassert>

#include "glm/glm.hpp"

#include <box2d.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <gl/glew.h>

//#include "UtilityFiles/Delegate.h"
#include "UtilityFiles/Dictionary.h"
#include "UtilityFiles/EventQueue.h"
#include "UtilityFiles/ICommand.h"
#include "UtilityFiles/ODArray.h"
#include "UtilityFiles/Singleton.h"

#include "Shaders/ShaderBase.h"

#include "ResourceWrappers/Font.h"
#include "ResourceWrappers/Prefab.h"
#include "ResourceWrappers/RenderTarget.h"
#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Texture2D.h"

#include "ImGui/imgui.h"

//#include "EngineIO/Reflection.h"
//#include "EngineIO/CustomSerializers.h"
//#include "EngineIO/Deserializer.h"
//#include "EngineIO/TypeInformation.h"

