
namespace BOO {

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
    void* ComponentPool<T>::getMember(size_t index) {
        return &m_componentData.at(index);
    }

    template<typename T>
    void ComponentPool<T>::removeMember(size_t index) {
        if(index >= m_componentData.size()) return;
        m_componentData[index] = m_componentData.back();
        m_componentData.pop_back();
    }


    template<typename T>
    Archetype Archetype::copyWith() {
        Archetype newArchetype;

        newArchetype.f_archetypeID = f_archetypeID;
        for(auto& [ type, pool ] : f_componentStorage) {
            newArchetype.f_componentStorage.emplace(type,  pool->cloneType());
        }
        newArchetype.f_archetypeID.insert(typeid(T));
        newArchetype.f_componentStorage.emplace(typeid(T), std::make_shared<ComponentPool<T>>());

        return newArchetype;
    }

    template<typename T>
    Archetype Archetype::copyWithout() {
        Archetype newArchetype;

        newArchetype.f_archetypeID = f_archetypeID;
        for(auto& [ type, pool ] : f_componentStorage) {
            if(type == typeid(T)) continue;
            newArchetype.f_componentStorage.emplace(type, pool->cloneType());
        }
        newArchetype.f_archetypeID.erase(typeid(T));

        return newArchetype;
    }


    template<typename T>
    T& Registry::addComponentToEntity(EntityID id) {
        Archetype* currentArchetype = m_entityArchetypes.at(id);
        size_t currentIndex = currentArchetype->f_entityComponentIndices.at(id);

        if(currentArchetype->f_archetypeID.find(typeid(T)) != currentArchetype->f_archetypeID.end())
            return getComponentFromEntity<T>(id);

        Archetype& newArchetype = getOrCreateArchetype<T, true>(*currentArchetype);

        // Put current entity in new archetype
        newArchetype.f_entityComponentIndices.emplace(id, newArchetype.f_entityComponentIndices.size());
        for(auto& [ type, _ ] : currentArchetype->f_componentStorage) {
            const void* component = currentArchetype->f_componentStorage.at(type)->getMember(currentIndex);
            newArchetype.f_componentStorage.at(type)->addMember(component);
        }
        newArchetype.f_componentStorage.at(typeid(T))->addMember();
        m_entityArchetypes[id] = &newArchetype;

        // Move back entity to current entity position in old archetype
        for(auto& [ movedEntity, index ] : currentArchetype->f_entityComponentIndices) {
            if(index == currentIndex) {
                currentArchetype->f_entityComponentIndices[movedEntity] = currentIndex;
                break;
            }
        }

        // Remove current entity from old archetype
        currentArchetype->f_entityComponentIndices.erase(id);
        for(auto& [ _, pool ] : currentArchetype->f_componentStorage)
            pool->removeMember(currentIndex);

        return getComponentFromEntity<T>(id);
    }

    template<typename T>
    T& Registry::getComponentFromEntity(EntityID id) {
        Archetype* archetype = m_entityArchetypes.at(id);

        size_t index = archetype->f_entityComponentIndices.at(id);
        return *static_cast<T*>(archetype->f_componentStorage.at(typeid(T))->getMember(index));
    }

    template<typename T>
    void Registry::removeComponentFromEntity(EntityID id) {
        Archetype* currentArchetype = m_entityArchetypes.at(id);
        size_t currentIndex = currentArchetype->f_entityComponentIndices.at(id);

        if(currentArchetype->f_archetypeID.find(typeid(T)) == currentArchetype->f_archetypeID.end())
            return;

        Archetype& newArchetype = getOrCreateArchetype<T, false>(*currentArchetype);

        // Put current entity in new archetype
        newArchetype.f_entityComponentIndices.emplace(id, newArchetype.f_entityComponentIndices.size());
        for(auto& [ type, _ ] : currentArchetype->f_componentStorage) {
            if(type == typeid(T)) continue;
            const void* component = currentArchetype->f_componentStorage.at(type)->getMember(currentIndex);
            newArchetype.f_componentStorage.at(type)->addMember(component);
        }
        m_entityArchetypes[id] = &newArchetype;

        // Move back entity to current entity position in old archetype
        for(auto& [ movedEntity, index ] : currentArchetype->f_entityComponentIndices) {
            if(index == currentIndex) {
                currentArchetype->f_entityComponentIndices[movedEntity] = currentIndex;
                break;
            }
        }

        // Remove current entity from old archetype
        currentArchetype->f_entityComponentIndices.erase(id);
        for(auto& [ _, pool ] : currentArchetype->f_componentStorage)
            pool->removeMember(currentIndex);
    }


    template<typename T, bool adding>
    Archetype& Registry::getOrCreateArchetype(Archetype& oldArchetype) {
        ArchetypeID newID = oldArchetype.f_archetypeID;
        if constexpr (adding) newID.insert(typeid(T)); else newID.erase(typeid(T));
        if(const auto it = m_archetypes.find(newID); it != m_archetypes.end())
            return it->second;

        m_archetypes.emplace(newID, adding ? oldArchetype.copyWith<T>() : oldArchetype.copyWithout<T>());
        return m_archetypes.at(newID);
    }
}