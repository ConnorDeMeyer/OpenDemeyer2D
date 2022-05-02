//#include "GameObject.h"
//
///**
// * TEMPLATE METHODS
// */
//
//template<typename T>
//T* GameObject::GetComponent() const
//{
//	if constexpr (std::is_same_v<T, Transform>) { return reinterpret_cast<T*>(m_pTransform); }
//	else if constexpr (std::is_same_v<T, Transform>) { return reinterpret_cast<T*>(m_pRenderComponent); }
//	else {
//		auto it = m_Components.find(typeid(T));
//		return (it != m_Components.end()) ? reinterpret_cast<T*>(it->second) : nullptr;
//	}
//}
//
//template <typename T>
//T* GameObject::GetComponentByCast() const
//{
//	for (auto component : m_Components)
//		if (auto returnValue{ dynamic_cast<T*>(component.second) }) return returnValue;
//	return nullptr;
//}
//
//template <typename T>
//std::enable_if_t<std::is_base_of_v<ComponentBase, T>, T*>
//GameObject::AddComponent()
//{
//	if constexpr (std::is_same_v<T, RenderComponent>)
//	{
//		if (m_pRenderComponent) throw std::runtime_error("Component already in gameobject");
//
//		m_pRenderComponent = new RenderComponent();
//		m_Components.insert({ typeid(T), m_pRenderComponent });
//		m_pRenderComponent->m_pParent = this;
//
//		if (m_HasBeenInitialized) m_pRenderComponent->BeginPlay();
//
//		return m_pRenderComponent;
//	}
//	else
//	{
//		assert(!GetComponent<T>()); // Make sure the component is not already added
//		T* comp = new T();
//		m_Components.insert({ typeid(T), comp });
//
//		comp->m_pParent = this;
//
//		if (m_HasBeenInitialized) comp->BeginPlay();
//		return comp;
//	}
//}
//
//template<typename T>
//void GameObject::RemoveComponent()
//{
//	if constexpr (std::is_same_v<T, Transform>) { throw std::runtime_error("Cannot remove transform component"); }
//
//	auto it = m_Components.find(typeid(T));
//	if (it != m_Components.end())
//	{
//		delete it->second;
//		m_Components.erase(it);
//	}
//}