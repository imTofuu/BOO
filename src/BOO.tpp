
template<typename T>
std::shared_ptr<IComponentPool> ComponentPool<T>::cloneType() {
    return std::make_shared<ComponentPool>();
}

template<typename T>
size_t ComponentPool<T>::addMember() {
    m_componentData.emplace_back(T());
    return m_componentData.size() - 1;
}

template<typename T>
size_t ComponentPool<T>::addMember(const void* component) {
    m_componentData.emplace_back(*static_cast<const T*>(component));
    return m_componentData.size() - 1;
}

template<typename T>
T* ComponentPool<T>::getMember(size_t index) {
    return index < m_componentData.size() ? &m_componentData.at(index) : nullptr;
}

template<typename T>
void ComponentPool<T>::removeMember(size_t index) {
    if(index >= m_componentData.size()) return;
    m_componentData[index] = m_componentData.back();
    m_componentData.pop_back();
}
