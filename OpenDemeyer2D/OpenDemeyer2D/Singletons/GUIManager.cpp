#include "Singletons/GUIManager.h"

#include "OpenDemeyer2D.h"

#include "box2d.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl.h"

#include "ImGuiExt/EngineStyle.h"
#include "ImGuiExt/FileDetailView.h"

#include "Singletons/ResourceManager.h"
#include "Singletons/SceneManager.h"
#include "Singletons/RenderManager.h"

#include "ResourceWrappers/RenderTarget.h"

#include "EngineFiles/GameObject.h"
#include "EngineFiles/Component.h"
#include "EngineFiles/Scene.h"

#include "Components/RenderComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/Transform.h"

void GUIManager::Init(SDL_Window* window)
{
	m_Window = window;

	auto& settings = ENGINE.GetSettings();

	settings.GetData(OD_KEEP_ASPECT_RATIO_EDITOR, m_bKeepAspectRatio);
	settings.GetData(OD_GAME_RESOLUTION_WIDTH, m_GameResWidth);
	settings.GetData(OD_GAME_RESOLUTION_HEIGHT, m_GameResHeight);

	/**
	* IMGUI
	*/

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init("#version 130");

	SetImGuiStyle();

	m_ImGuiRenderTarget = RESOURCES.CreateRenderTexture(m_GameResWidth, m_GameResHeight);

	m_pCurrentDirectory = RESOURCES.GetRootDirectory();
}

void GUIManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImGui::DestroyContext();
}

void GUIManager::BeginGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();
}

void GUIManager::EndGUI()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::RenderGUI()
{
	RenderImGuiFileView();
	RenderImGuiDirView();
	RenderImGuiMainMenu();
	RenderImGuiFileDetails();
	SCENES.RenderImGui();
	ENGINE.RenderStats();
	ENGINE.RenderSettings();
	RenderImGuiGameWindow();
	RenderImGuiRenderInfo();
	RenderImGuiObjectInfo();
	ImGui::Render();
}

void GUIManager::RenderGUIOnGame()
{
	if (m_bShowHitboxes) RenderHitboxes();
	RenderImGuiObjectLocation();
}


void GUIManager::RenderImGuiRenderInfo()
{
	ImGui::Begin("Render Information");

	float width = ImGui::GetWindowSize().x;

	//bool renderHitboxes{ m_bShowHitboxes };
	ImGui::Checkbox("Show hitboxes", &m_bShowHitboxes);

	auto& renderLayers = RENDER.GetRenderLayers();

	for (size_t i{}; i < renderLayers.size(); ++i)
	{
		std::string text{ "Render Target " };
		text += std::to_string(i);
		ImGui::Text(text.c_str());

		float aspectRatio = float(renderLayers[i]->GetWidth()) / float(renderLayers[i]->GetHeight());

#pragma warning(disable : 4312)
		ImGui::Image((ImTextureID)renderLayers[i]->GetRenderedTexture(), { width, width / aspectRatio });
#pragma warning(default : 4312)
	}

	ImGui::End();
}

void GUIManager::RenderHitboxes() const
{
	static_assert(sizeof(b2Vec2) == sizeof(glm::vec2));

	auto& renderer = RENDER;
	renderer.SetRenderTarget(renderer.GetRenderLayers().back());
	renderer.SetColor({ 255,55,255,200 });

	// when the game is running and bodies are initialized
	if (SCENES.GetActiveScene()->HasBegunPlay())
	{
		auto pWorld = SCENES.GetActiveScene()->GetPhysicsWorld();

		for (auto pBody{ pWorld->GetBodyList() }; pBody; pBody = pBody->GetNext())
		{
			auto& position = pBody->GetPosition();
			for (auto fixture{ pBody->GetFixtureList() }; fixture; fixture = fixture->GetNext())
			{
				switch (fixture->GetType())
				{
				case b2Shape::Type::e_polygon:
				{
					auto polygon = reinterpret_cast<b2PolygonShape*>(fixture->GetShape());
					b2Vec2 positions[b2_maxPolygonVertices]{};
					for (int32 i{}; i < polygon->m_count; ++i)
					{
						positions[i] = polygon->m_vertices[i] + position;
					}
					renderer.RenderPolygon(reinterpret_cast<glm::vec2*>(positions), polygon->m_count, true);
				}
				break;
				case b2Shape::Type::e_circle:
				{
					auto circle = reinterpret_cast<b2CircleShape*>(fixture->GetShape());
					glm::vec2 center = { position.x + circle->m_p.x,position.y + circle->m_p.y };
					renderer.RenderEllipse(center, { circle->m_radius, circle->m_radius }, true);
				}
				break;
				}
			}

		}
	}
	else // editor mode where there are no bodies yet
	{
		auto scene = SCENES.GetActiveScene();
		for (auto go : scene->GetAllObjects())
		{
			if (auto comp = go.second->GetComponent<PhysicsComponent>())
			{
				Transform* transform = go.second->GetTransform();
				auto& pos = transform->GetWorldPosition();
				for (auto& fixture : comp->GetFixtureDefs())
				{
					switch (fixture.second->GetType())
					{
					case b2Shape::Type::e_polygon:
					{
						auto polygon = reinterpret_cast<b2PolygonShape*>(fixture.second.get());
						glm::vec2 positions[b2_maxPolygonVertices]{};
						for (int32 i{}; i < polygon->m_count; ++i)
						{
							positions[i] = reinterpret_cast<const glm::vec2&>(polygon->m_vertices[i]) + pos;
						}
						RENDER.RenderPolygon(positions, polygon->m_count, true);
					}
					break;
					case b2Shape::Type::e_circle:
					{
						auto circle = reinterpret_cast<b2CircleShape*>(fixture.second.get());
						glm::vec2 center = { pos.x + circle->m_p.x,pos.y + circle->m_p.y };
						RENDER.RenderEllipse(center, { circle->m_radius, circle->m_radius }, true);
					}
					break;
					case b2Shape::Type::e_edge:

						break;
					}
				}
			}
		}
	}

}

void GUIManager::RenderImGuiObjectInfo()
{
	ImGui::Begin("Component Details");

	if (!m_pSelectedObject.expired())
	{
		bool openError{};

		if (ImGui::BeginCombo("Add Component","Component"))
		{
			for (auto& typeInfo : TypeInformation::GetInstance().GetTypeInfos())
			{
				bool Selected{};
				std::string name{ typeInfo.second.name };
				if (ImGui::Selectable(name.c_str(), &Selected))
				{
					try
					{
						typeInfo.second.componentGenerator(m_pSelectedObject.lock().get());
					}
					catch (std::exception&)
					{
						openError = true;
					}
				}
			}
			ImGui::EndCombo();
		}

		if (openError)
			ImGui::OpenPopup("Add Component Error");

		if (ImGui::BeginPopupModal("Add Component Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextWrapped("Component is already in the Game Object");
			if (ImGui::Button("continue", { 200,0 }))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		auto pObject = m_pSelectedObject.lock();
		auto& components = pObject->GetComponents();
		for (auto& comp : components)
		{
			ImGui::PushID(comp.second);
			std::string label(comp.second->GetComponentName());
			bool header{ ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None) };
			if (ImGui::BeginPopupContextItem("component options"))
			{
				if (ImGui::MenuItem("delete component"))
				{
					pObject->RemoveComponent(comp.second);
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					ImGui::PopID();
					break;
				}
				ImGui::EndPopup();
			}
			if (header)
			{
				comp.second->RenderImGui();
			}
			ImGui::PopID();
		}
	}



	ImGui::End();
}

void GUIManager::RenderImGuiObjectLocation()
{
	if (!m_pSelectedObject.expired())
	{
		// Draw bounding box
		auto pObject = m_pSelectedObject.lock();
		if (auto pRenderComp = pObject->GetRenderComponent())
		{
			auto& renderer = RENDER;
			renderer.SetColor({ 25,255,25,175 });

			glm::vec2 vertices[4];
			pRenderComp->GetWorldRect(vertices);

			renderer.SetRenderTarget(renderer.GetRenderLayers().back());
			renderer.RenderPolygon(vertices, 4, false);
		}
	}
}

void GUIManager::RenderImGuiMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Game", "CTRL+S"))
			{
				ENGINE.SaveGame();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void GUIManager::RenderImGuiDirView()
{
	ImGui::Begin("Directory View");

	auto root = RESOURCES.GetRootDirectory();
	ImGui::PushID(root);
	if (ImGui::TreeNodeEx(root->dirPath.string().c_str(), root->Directories.empty() * ImGuiTreeNodeFlags_Leaf))
	{
		for (auto subDir : root->Directories)
		{
			RenderImGuiDirViewRecursive(subDir);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

	ImGui::End();
}

void GUIManager::RenderImGuiDirViewRecursive(Directory* dir)
{
	ImGui::PushID(dir);
	if (ImGui::TreeNodeEx(dir->dirPath.filename().string().c_str(), dir->Directories.empty() * ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_pCurrentDirectory = dir;
		for (auto subDir : dir->Directories)
		{
			RenderImGuiDirViewRecursive(subDir);
		}
		ImGui::TreePop();
	}
	else if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		m_pCurrentDirectory = dir;

	ImGui::PopID();
}

void GUIManager::RenderImGuiFileView()
{
	ImGui::Begin("File View");

	static std::string errorMessage{};

	if (m_pCurrentDirectory)
	{
		for (auto& file : m_pCurrentDirectory->Files)
		{
			/*if (ImGui::Selectable(file->GetFileName().c_str()))
			{
				try
				{
					m_FileDetails = file.get();
				}
				catch (std::runtime_error& e)
				{
					errorMessage = e.what();
					ImGui::OpenPopup("Error Viewing File");
				}
			}*/
			if (file->RenderImGuiFileName())
			{
				m_FileDetails = file.get();
			}
		}
	}

	if (ImGui::BeginPopupModal("Error Viewing File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped(errorMessage.c_str());
		if (ImGui::Button("continue", {200,0}))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

void GUIManager::RenderImGuiFileDetails()
{
	ImGui::Begin("File Details");

	if (m_FileDetails)
		m_FileDetails->RenderDetails();
	
	ImGui::End();
}

inline float cross2D(const glm::vec2& vec0, const glm::vec2& vec1)
{
	return vec0.x * vec1.y - vec0.y * vec1.x;
}

inline bool IsPointInRect(const glm::vec2& point, glm::vec2* vertices)
{
	// A--->B
	// ^    |
	// |    v
	// D<---C

	glm::vec2 AB = vertices[1] - vertices[0];
	glm::vec2 BC = vertices[2] - vertices[1];
	glm::vec2 CD = vertices[3] - vertices[2];
	glm::vec2 DA = vertices[0] - vertices[3];

	glm::vec2 AP = point - vertices[0];
	glm::vec2 BP = point - vertices[1];
	glm::vec2 CP = point - vertices[2];
	glm::vec2 DP = point - vertices[3];

	return
		cross2D(AB, AP) < 0 &&
		cross2D(BC, BP) < 0 &&
		cross2D(CD, CP) < 0 &&
		cross2D(DA, DP) < 0;
}

GameObject* SearchObjectForHit(const glm::vec2& point, GameObject* ob)
{
	glm::vec2 vertices[4]{};
	if (ob->GetRenderComponent() && IsPointInRect(point, ob->GetRenderComponent()->GetWorldRect(vertices)))
	{
		return ob;
	}
	for (auto pObject : ob->GetChildren())
	{
		if (SearchObjectForHit(point, pObject))
			return pObject;
	}
	return nullptr;
}

void SearchObjectForHit(const glm::vec2& point, GameObject* ob, std::vector<GameObject*>& hits)
{
	glm::vec2 vertices[4]{};
	if (ob->GetRenderComponent() && IsPointInRect(point, ob->GetRenderComponent()->GetWorldRect(vertices)))
	{
		hits.emplace_back(ob);
	}
	for (auto pObject : ob->GetChildren())
	{
		SearchObjectForHit(point, pObject, hits);
	}
}

void GUIManager::SetSelectedObject(GameObject* pObject)
{
	auto now = std::chrono::high_resolution_clock::now();
	if ((now - m_LastClickTime).count() > 2'000'000)
	{
		m_LastClickTime = now;
		if (pObject)
		{
			if (pObject == m_pSelectedObject.lock().get())
				m_pSelectedObject = std::weak_ptr<GameObject>();
			else
				m_pSelectedObject = pObject->GetWeakReference();
		}
		else
			m_pSelectedObject = std::weak_ptr<GameObject>();
	}
}

void GUIManager::RenderImGuiGameWindow()
{
	ImGuiWindowFlags flags{};
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoScrollbar;

	ImGui::Begin("Game", nullptr, flags);

	// play/pause/stop buttons
	if (ImGui::Button("Play"))
	{
		auto& scenes = SCENES;
		auto activeScene = scenes.GetActiveScene();
		if (activeScene)
		{
			auto& scene = scenes.CreateScene(activeScene->GetName() + std::string("(copy)"));
			scene.Copy(activeScene);
			scenes.SetActiveScene(&scene);

			ENGINE.PlayGame();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		ENGINE.PauseGame(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("Resume"))
	{
		ENGINE.PauseGame(false);
	}
	ImGui::SameLine();
	ImGui::Button("Stop");

	float aspectRatio = float(m_GameResWidth) / float(m_GameResHeight);

	auto pos = glm::vec2(ImGui::GetCursorPos()) + glm::vec2(ImGui::GetWindowPos());
	auto imageSize = ImGui::GetWindowSize();
	imageSize.y -= 30; // add offset

	if (m_bKeepAspectRatio)
	{
		ImVec2 newSize = { imageSize.y * aspectRatio, imageSize.y };
		if (newSize.x > imageSize.x)
			newSize = { imageSize.x, imageSize.x / aspectRatio };
		imageSize = newSize;
	}

	auto& renderer = RENDER;

	renderer.SetRenderTarget(m_ImGuiRenderTarget);
	for (auto& renderTarget : renderer.GetRenderLayers())
	{
		renderer.RenderTexture(renderTarget, glm::vec2{ 0,renderTarget->GetHeight() }, { 1,-1 }, 0, { 1,1 });
	}
	renderer.SetRenderTargetScreen();

	// Draw image in center
	float offsetX = (ImGui::GetWindowWidth() - imageSize.x) * 0.5f;
	ImGui::SetCursorPosX(offsetX);

#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)m_ImGuiRenderTarget->GetRenderedTexture(), imageSize);
#pragma warning(default : 4312)

	// OBJECT SELECTOR
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		glm::vec2 mousePos = ImGui::GetMousePos();
		mousePos -= glm::vec2(pos);
		mousePos.x -= offsetX;
		mousePos.x *= m_GameResWidth / imageSize.x;
		mousePos.y *= m_GameResHeight / imageSize.y;
		mousePos.y = m_GameResHeight - mousePos.y;

		auto& sceneTree = SCENES.GetActiveScene()->GetSceneTree();
		for (GameObject* pObject : sceneTree)
		{
			if (GameObject* ob = SearchObjectForHit(mousePos, pObject))
			{
				SetSelectedObject(ob);
				break;
			}
		}

		SetSelectedObject(nullptr);
	} 
	else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		glm::vec2 mousePos = ImGui::GetMousePos();
		mousePos -= glm::vec2(pos);
		mousePos.x *= m_GameResWidth / imageSize.x;
		mousePos.y *= m_GameResHeight / imageSize.y;
		mousePos.y = m_GameResHeight - mousePos.y;

		auto& sceneTree = SCENES.GetActiveScene()->GetSceneTree();
		m_HitObjects.clear();
		for (GameObject* pObject : sceneTree)
		{
			SearchObjectForHit(mousePos, pObject, m_HitObjects);
		}

		if (!m_HitObjects.empty())
		{
			ImGui::OpenPopup("hit objects");
		}
	}

	if (ImGui::BeginPopup("hit objects"))
	{
		for (auto ob : m_HitObjects)
		{
			ImGui::MenuItem(ob->GetDisplayName().c_str());
			if (ImGui::IsItemClicked())
			{
				SetSelectedObject(ob);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}