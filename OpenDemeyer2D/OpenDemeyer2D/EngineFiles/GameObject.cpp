#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"
#include "EngineFiles/ComponentBase.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "imgui.h"
#include "Singletons/GUIManager.h"

GameObject::GameObject()
	: m_Reference	{ std::shared_ptr<GameObject>(this,[](GameObject*){}) }
{
	m_pTransform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
	for (auto comp : m_Components)
	{
		delete comp.second;
	}

	for (auto obj : m_Children)
	{
		delete obj;
	}

	// signal scene that it has been destroyed
	//if (m_pScene) m_pScene->RemoveObject(this); //TODO fix this so it doesnt invalidate the for loop when deleting the scene
}

void GameObject::Update(float deltaTime)
{
	for (auto comp : m_Components)
	{
		comp.second->Update(deltaTime);
	}

	for (auto obj : m_Children)
	{
		obj->Update(deltaTime);
	}
}

void GameObject::LateUpdate()
{
	for (auto comp : m_Components)
	{
		comp.second->LateUpdate();
	}

	for (auto obj : m_Children)
	{
		obj->LateUpdate();
	}
}

void GameObject::Render() const
{
	if (m_pRenderComponent) m_pRenderComponent->Render();

	for (auto obj : m_Children)
	{
		obj->Render();
	}
}

void GameObject::BeginPlay()
{
	for (auto comp : m_Components)
	{
		comp.second->BeginPlay();
	}
	m_HasBegunPlay = true;

	for (auto obj : m_Children)
	{
		if (!obj->m_HasBegunPlay)obj->BeginPlay();
	}
}

void GameObject::InitializeComponents()
{
	for (auto comp : m_Components)
	{
		comp.second->Initialize();
	}
	m_HasBeenInitialized = true;

	for (auto obj : m_Children)
	{
		if (!obj->m_HasBeenInitialized)obj->InitializeComponents();
	}
}

void GameObject::Destroy()
{
	m_pScene->DestroyObject(this);
}

std::weak_ptr<GameObject> GameObject::GetWeakReference() const
{
	return m_Reference;
}

ComponentBase* GameObject::GetComponentById(uint32_t typeId)
{
	auto it = m_Components.find(typeId);
	if (it != m_Components.end())
	{
		return it->second;
	}
	return nullptr;
}

void GameObject::RemoveComponent(ComponentBase* pComponent)
{
	if (!pComponent) return;

	for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
	{
		if (it->second == pComponent) {
			m_Components.erase(it);
			delete pComponent;
			break;
		}
	}
}

void GameObject::SetParent(GameObject* pObject)
{
	// Check if already has a parent and break link between those
	if (m_Parent)
	{
		m_Parent->m_Children.SwapRemove(this);
	}
	// if we dont have a parent but we do have a scene, that means we are attached to the scenegraph of the scene
	else if (m_pScene && pObject)
	{
		// remove this from the scene's scenegraph
		m_pScene->RemoveObject(this);
	}

	// Set as parent
	m_Parent = pObject;

	if (pObject) 
	{
		// Set as child
		pObject->m_Children.emplace_back(this);

		// Set the scene
		if (m_pScene != pObject->m_pScene)
		SetScene(pObject->m_pScene);

		// initialize game object
		if (pObject->m_HasBeenInitialized && !m_HasBeenInitialized) InitializeComponents();
		if (pObject->m_HasBegunPlay && !m_HasBegunPlay) BeginPlay();
	}
	else if (m_pScene)
	{
		m_pScene->UnregisterObject(this);
	}

	// Set transform relative to parent
	m_pTransform->Move({});
}

void GameObject::SetParent(Scene& scene)
{
	if (m_pScene)
	{
		if (m_pScene == &scene)
		{
			return;
		}

		m_pScene->RemoveObject(this);
		m_pScene->UnregisterObject(this);
	}

	if (m_Parent)
	{
		m_Parent->m_Children.SwapRemove(this);
	}

	m_Parent = nullptr;
	SetScene(&scene);

	scene.AddToSceneTree(this);

	m_pTransform->Move({});
}

void GameObject::Serialize(std::ostream& os)
{
	os << m_ObjectId << ' ' << m_Name << "\n";

	for (auto& component : m_Components)
	{
		os << component.second->GetComponentName() << '\n' << "{\n";
		component.second->Serialize(os);
		os << "}\n";
	}

	os << "{\n";

	for (auto child : m_Children)
	{
		child->Serialize(os);
	}

	os << "}\n";
}

void GameObject::Deserialize(Deserializer& is)
{
	unsigned int streamId{};
	*is.GetStream() >> streamId;
	is.RegisterGameObject(streamId, this);

	std::getline(*is.GetStream(), m_Name);
	TrimWhitespace(m_Name);

	auto& types = TypeInformation::GetInstance();

	while (is.PeekChar() == 'c')
	{
		// get the class name from the stream
		std::string componentName;
		std::getline(*is.GetStream(), componentName);

		// get the class id
		auto typeInfo = types.GetTypeInfo(componentName);
		assert(typeInfo);

		// generate and add the component
		auto pComponent = typeInfo->componentGenerator(this);

		// deserialize the component
		pComponent->Deserialize(is);
	}

	if (is.CanContinue())
	{
		while (!is.IsEnd())
		{
			auto go = new GameObject();
			go->SetParent(this);
			go->Deserialize(is);
		}
	}
}

void GameObject::SetName(const std::string& name)
{
	m_Name = name;
}

std::string GameObject::GetDisplayName() const
{
	return m_Name.empty() ?
		(std::string("GameObject") + std::to_string(m_ObjectId)).c_str() :
		m_Name.c_str();
}

void GameObject::SetScene(Scene* pScene)
{
	if (pScene != m_pScene)
	{
		if (m_pScene)
			m_pScene->UnregisterObject(this);
		if (pScene)
			pScene->RegisterObject(this);
	}

	m_pScene = pScene;

	for (auto child : m_Children)
	{
		child->SetScene(pScene);
	}
}

void GameObject::Copy(GameObject* originalObject, CopyLinker* copyLinker)
{
	if (copyLinker)
	{
		copyLinker->RegisterObject(originalObject, this);

		for (auto& comp : originalObject->m_Components)
		{
			comp.second->MakeCopy(this, copyLinker);
		}
		SetName(originalObject->m_Name);
		SetTag(originalObject->m_Tag);

		for (auto child : originalObject->m_Children)
		{
			auto go = new GameObject();
			go->Copy(child, copyLinker);
			go->SetParent(this);
		}
	}
	else
	{
		CopyLinker linker;

		linker.RegisterObject(originalObject, this);

		for (auto& comp : originalObject->m_Components)
		{
			comp.second->MakeCopy(this, &linker);
		}
		SetName(originalObject->m_Name);
		SetTag(originalObject->m_Tag);

		for (auto child : originalObject->m_Children)
		{
			auto go = new GameObject();
			go->Copy(child, &linker);
			go->SetParent(this);
		}

		linker.PerformLinkingActions();
	}
}

void CopyLinker::LinkWithNewObject(GameObject* originalObject, const std::function<void(GameObject*)>& linkingAction)
{
	auto it = m_RegisteredObjects.find(originalObject);
	if (it != m_RegisteredObjects.end())
	{
		linkingAction(it->second);
	}
	else
	{
		m_LinkingActions.emplace_back(originalObject, linkingAction);
	}
}

void CopyLinker::RegisterObject(GameObject* originalObject, GameObject* newObject)
{
	m_RegisteredObjects.emplace(originalObject, newObject);
}

void CopyLinker::PerformLinkingActions()
{
	for (auto& actions : m_LinkingActions)
	{
		// if the original object has been copied we transfer the reference to the copy
		auto it = m_RegisteredObjects.find(actions.originalObject);
		if (it != m_RegisteredObjects.end())
		{
			actions.Action(it->second);
		}
	}
	m_LinkingActions.clear();
}
