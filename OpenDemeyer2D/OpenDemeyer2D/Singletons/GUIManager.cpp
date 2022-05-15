#include "Singletons/GUIManager.h"
#include <stack>

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


	// ImGui settings window setup
	auto it = settings.begin();
	while (it != settings.end())
	{
		std::string key = it->first;

		auto sstream = std::stringstream();

		it->second->OStreamValue(sstream);

		std::string value;
		sstream >> value;

		// check if the setting isn't already in there
		assert(m_SettingsInputBuffer.find(key) == m_SettingsInputBuffer.end());

		char* buffer = new char[INPUT_TEXT_BUFFER_SIZE] {};
		_memccpy(buffer, value.c_str(), '\0', value.size());
		m_SettingsInputBuffer.insert(std::make_pair(key, buffer));

		++it;
	}
}

void GUIManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImGui::DestroyContext();
}

GUIManager::~GUIManager()
{
	// cleanup ImGui variables
	for (auto& buffer : m_SettingsInputBuffer)
		delete[] buffer.second;
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
	RenderImGuiSceneGraph();
	RenderImGuiEngineStats();
	RenderImGuiEngineSettings();
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
		try
		{
			auto scene = SCENES.GetActiveScene();
			for (auto go : scene->GetAllObjects())
			{
				if (auto comp = go.second->GetComponent<PhysicsComponent>())
				{
					Transform* transform = go.second->GetTransform();
					auto& pos = transform->GetWorldPosition();
					for (auto& shape : comp->GetShapes())
					{
						switch (shape->GetType())
						{
						case b2Shape::Type::e_polygon:
						{
							auto polygon = reinterpret_cast<b2PolygonShape*>(shape.get());
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
							auto circle = reinterpret_cast<b2CircleShape*>(shape.get());
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
		catch (std::exception& e)
		{
			std::cout << e.what();
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

// Returns true if the folder structure has been changed
bool ImGuiDirPopup(Directory* currentDir)
{
	bool returnValue{};

	bool addFolder{};
	bool DeleteFolder{};

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Add Folder"))
		{
			addFolder = true;
		}

		if (ImGui::MenuItem("Delete folder"))
		{
			DeleteFolder = true;
		}

		ImGui::EndPopup();
	}

	if (addFolder)
		ImGui::OpenPopup("Add Folder");

	if (DeleteFolder)
		ImGui::OpenPopup("Delete folder");

	if (ImGui::BeginPopupModal("Add Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char buffer[32]{};
		ImGui::InputText("Name", buffer, 32);
		
		if (ImGui::Button("Create"))
		{
			RESOURCES.AddDirectory(currentDir, std::string(buffer));
			ImGui::CloseCurrentPopup();
			returnValue = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Delete folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted("Are you sure you want to delete this folder. This cant be undone");

		if (ImGui::Button("Create"))
		{
			RESOURCES.DeleteDirectory(currentDir);
			ImGui::CloseCurrentPopup();
			returnValue = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	return returnValue;
}

void GUIManager::RenderImGuiDirView()
{
	m_bDirViewQuickExit = false;

	ImGui::Begin("Directory View");

	auto root = RESOURCES.GetRootDirectory();
	ImGui::PushID(root);
	bool treeNode = (ImGui::TreeNodeEx(root->dirPath.string().c_str(), root->Directories.empty() * ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow));

	m_bDirViewQuickExit = ImGuiDirPopup(root);

	if (treeNode)
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
	if (m_bDirViewQuickExit)
		return;

	ImGui::PushID(dir);
	bool treeNode = (ImGui::TreeNodeEx(dir->dirPath.filename().string().c_str(), dir->Directories.empty() * ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow));

	m_bDirViewQuickExit = ImGuiDirPopup(dir);
	
	if (treeNode)
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

bool GUIManager::RenderImGuiDirSelector(std::filesystem::path& pathOut)
{
	auto root = RESOURCES.GetRootDirectory();

	bool returnValue{};

	ImGui::PushID(root);

	bool treeNode = ImGui::TreeNodeEx(root->dirPath.string().c_str(), root->Directories.empty() * ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		pathOut = root->dirPath;
		returnValue = true;
	}

	if (treeNode)
	{
		for (auto pDir : root->Directories)
		{
			returnValue = returnValue || RenderImGuiDirSelectorRecursive(pDir, pathOut);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

	return returnValue;
}

bool GUIManager::RenderImGuiDirSelectorRecursive(Directory* dir, std::filesystem::path& pathOut)
{
	ImGui::PushID(dir);

	bool returnValue{};

	bool treeNode = ImGui::TreeNodeEx(dir->dirPath.filename().string().c_str(), dir->Directories.empty() * ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		pathOut = dir->dirPath;
		returnValue = true;
	}

	if (treeNode)
	{
		for (auto pDir : dir->Directories)
		{
			returnValue = returnValue || RenderImGuiDirSelectorRecursive(pDir, pathOut);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

	return returnValue;
}

void GUIManager::RenderImGuiFileView()
{
	ImGui::Begin("File View");

	static std::string errorMessage{};

	if (m_pCurrentDirectory)
	{
		for (auto& file : m_pCurrentDirectory->Files)
		{
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

void GUIManager::RenderImGuiEngineStats()
{
	ImGui::Begin("Statistics");

	int w{}, h{};
	ENGINE.GetResolution(w, h);

	ImGui::Text("Application average %.3f ms/frame", 1000.f / ImGui::GetIO().Framerate);
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Resolution: %.3i x %.1i", w, h);

	ImGui::End();
}

void GUIManager::RenderImGuiEngineSettings()
{
	ImGui::Begin("Engine Settings");

	auto& settings = ENGINE.GetSettings();

	auto it = settings.begin();
	while (it != settings.end())
	{
		auto itBuffer = m_SettingsInputBuffer.find(it->first);
		if (itBuffer != m_SettingsInputBuffer.end())
		{
			ImGui::InputText(it->first.c_str(), itBuffer->second, INPUT_TEXT_BUFFER_SIZE);
		}
		++it;
	}

	if (ImGui::Button("Save settings"))
	{
		ENGINE.SaveSettings();
	}

	ImGui::End();
}

void GUIManager::RenderImGuiGameObjectRecursive(GameObject* go)
{
	if (m_bSceneGraphQuickExit)
		return;

	ImGui::PushID(go);

	// Object name in tree
	bool treeNode = (ImGui::TreeNodeEx(go->GetDisplayName().c_str(),
		ImGuiTreeNodeFlags_Leaf * go->GetChildren().empty() | ImGuiTreeNodeFlags_OpenOnArrow));

	// POPUP
	{
		bool changeName{};
		bool deleteObj{};
		bool saveToFile{};
		static char buffer[32]{};

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Object"))
			{
				deleteObj = true;
			}

			if (ImGui::MenuItem("Change Name"))
			{
				changeName = true;
				std::memcpy(buffer, go->GetDisplayName().c_str(), go->GetDisplayName().size());
			}

			if (ImGui::MenuItem("Duplicate"))
			{
				auto otherGo = new GameObject();
				otherGo->Copy(go);
				if (go->GetParent())
					otherGo->SetParent(go->GetParent());
				else
					go->GetScene()->Add(otherGo);
				m_bSceneGraphQuickExit = true;
			}

			if (ImGui::MenuItem("Add Empty Child"))
			{
				auto newGo = new GameObject();
				newGo->SetParent(go);
			}

			if (ImGui::MenuItem("Save to File"))
			{
				saveToFile = true;
			}

			ImGui::EndPopup();
		}

		if (changeName)
			ImGui::OpenPopup("Change Obj Name");

		if (deleteObj)
			ImGui::OpenPopup("Delete Object");

		if (saveToFile)
			ImGui::OpenPopup("Save To File");

		if (ImGui::BeginPopupModal("Change Obj Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("new name", buffer, 32);
			if (ImGui::Button("Change Name"))
			{
				go->SetName(std::string(buffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Delete Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to delete the object?");
			if (ImGui::Button("Delete"))
			{
				go->Destroy();
				m_bSceneGraphQuickExit = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Save To File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Select directory");
			
			static std::filesystem::path directory;

			if (ImGui::BeginChild("directory", ImVec2{ 200,200 }, true))
			{
				RenderImGuiDirSelector(directory);
				
				ImGui::EndChild();
			}

			ImGui::Text(directory.string().c_str());

			static char fileBuffer[64]{};
			ImGui::InputText("File Name", fileBuffer, 64);

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				std::filesystem::path outputPath = directory;
				outputPath /= std::string(fileBuffer);
				RESOURCES.SaveGameObject(go, outputPath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			SetSelectedObject(go);

	// PAYLOAD
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("GameObject", &go, sizeof(GameObject*));

		ImGui::Text(go->GetDisplayName().c_str());

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			if (payload->Data)
			{
				auto goPayload = *static_cast<GameObject**>(payload->Data);
				goPayload->SetParent(go);
				m_bSceneGraphQuickExit = true;
			}
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObjectFile"))
		{
			if (payload->Data)
			{
				auto& goPayload = *static_cast<std::unique_ptr<GameObject>*>(payload->Data);
				auto newGo = new GameObject();
				newGo->Copy(goPayload.get());
				newGo->SetParent(go);
				m_bSceneGraphQuickExit = true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (treeNode)
	{
		for (auto& child : go->GetChildren())
		{
			RenderImGuiGameObjectRecursive(child);
			if (m_bSceneGraphQuickExit)
				break;
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GUIManager::RenderImGuiSceneGraph()
{
	ImGui::Begin("SceneGraph");

	static char buffer[32]{};

	if (ImGui::Button("Create Scene"))
	{
		ImGui::OpenPopup("Create Scene");
	}

	if (ImGui::BeginTabBar("Scenes"))
	{
		auto& game = SCENES.GetGameScene();
		if (game)
		{
			ImGui::PushID(game.get());
			RenderImGuiScene(game.get());
			ImGui::PopID();
		}

		for (Scene* pScene : SCENES.GetScenes())
		{
			ImGui::PushID(pScene);
			RenderImGuiScene(pScene);
			ImGui::PopID();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::BeginPopupModal("Create Scene", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("name", buffer, 32);
		if (ImGui::Button("Create"))
		{
			SCENES.CreateScene(std::string(buffer));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

void GUIManager::RenderImGuiScene(Scene* pScene)
{
	ImGui::PushID(pScene);
	
	bool beginTab = ImGui::BeginTabItem(pScene->GetName().c_str());

	// POPUP
	{
		bool changeName{};
		bool saveScene{};
		static char buffer[32]{};

		if (ImGui::BeginPopupContextItem("Scene settings"))
		{
			if (ImGui::MenuItem("Delete Scene"))
			{
				SCENES.RemoveScene(pScene);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Change Name"))
			{
				changeName = true;
				std::memcpy(buffer, pScene->GetName().c_str(), pScene->GetName().size());
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Set as active"))
			{
				SCENES.SetActiveScene(pScene);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Add GameObject"))
			{
				pScene->Add(new GameObject());
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				saveScene = true;
			}

			ImGui::EndPopup();
		}

		if (changeName)
			ImGui::OpenPopup("Change Scene Name");

		if (saveScene)
			ImGui::OpenPopup("Save Scene");

		if (ImGui::BeginPopupModal("Change Scene Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("new name", buffer, 32);
			if (ImGui::Button("Change Name"))
			{
				pScene->ChangeName(std::string(buffer));
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Save Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Select directory");

			static std::filesystem::path directory;

			if (ImGui::BeginChild("directory", ImVec2{ 200,200 }, true))
			{
				RenderImGuiDirSelector(directory);

				ImGui::EndChild();
			}

			ImGui::Text(directory.string().c_str());

			static char fileBuffer[64]{};
			ImGui::InputText("File Name", fileBuffer, 64);

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				std::filesystem::path outputPath = directory;
				outputPath /= std::string(fileBuffer);
				RESOURCES.SaveScene(pScene, outputPath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	// TAB
	if (beginTab) {


		// SCENE GRAPH
		auto& gameObjects = pScene->GetSceneTree();
		m_bSceneGraphQuickExit = false;

		for (size_t i{}; i < gameObjects.size(); ++i)
		{
			auto pObject{ gameObjects[i] };
			RenderImGuiGameObjectRecursive(pObject);
		}

		ImVec2 emptySize = ImGui::GetWindowSize();
		emptySize.y -= 20.f;
		emptySize.x -= 20.f;
		emptySize.y -= ImGui::GetCursorPosY();

		emptySize.y = std::max(emptySize.y, 40.f);

		ImGui::BeginChild("SceneGraph", emptySize);

		ImGui::EndChild();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
			{
				if (payload->Data)
				{
					auto goPayload = *static_cast<GameObject**>(payload->Data);
					goPayload->SetParent(pScene);
				}
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObjectFile"))
			{
				if (payload->Data && payload->Delivery)
				{
					auto& goPayload = *static_cast<std::unique_ptr<GameObject>*>(payload->Data);
					auto newGo = new GameObject();
					newGo->Copy(goPayload.get());
					newGo->SetParent(pScene);
				}
			}
		
			ImGui::EndDragDropTarget();
		}

		ImGui::EndTabItem();
	}


	ImGui::PopID();
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
		SCENES.PlayScene();
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
	if (ImGui::Button("Stop"))
	{
		SCENES.StopPlayingScene();
	}

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
		mousePos.x -= offsetX;
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