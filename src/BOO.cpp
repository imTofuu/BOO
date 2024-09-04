#include "BOO/BOO.h"

namespace BOO {

    Registry::Registry() {
        m_archetypes.emplace(ArchetypeID{}, Archetype());
    }

    EntityID Registry::createEntity(EntityID parentID) {
        static EntityID nextID = 1;

        EntityID id = nextID++;
        m_entityArchetypes.emplace(id, &m_archetypes.at({}));
        m_archetypes.at({}).f_entityComponentIndices.emplace(id, 0);

        m_entityChildren.emplace(id, std::vector<EntityID>());
        if(parentID != 0) m_entityChildren.at(parentID).push_back(id);
        m_entityParents.emplace(id, parentID);

        return id;
    }

    void Registry::destroyEntity(EntityID id) {
        for(auto& [ _, pool ] : m_entityArchetypes.at(id)->f_componentStorage) {
            pool->removeMember(m_entityArchetypes.at(id)->f_entityComponentIndices.at(id));
        }

        for(EntityID childID : m_entityChildren.at(id)) {
            destroyEntity(childID);
        }
        m_entityChildren.erase(id);

        EntityID parentID = m_entityParents.at(id);

        if(m_entityChildren.find(parentID) != m_entityChildren.end()) {
            auto& siblings = m_entityChildren.at(parentID);
            siblings.erase(std::remove(siblings.begin(), siblings.end(), id), siblings.end());
        }

        m_entityParents.erase(id);

        m_entityArchetypes.at(id)->f_entityComponentIndices.erase(id);
        m_entityArchetypes.erase(id);
    }

    void Registry::setEntityParent(EntityID id, EntityID parentID) {
        if(m_entityParents.at(id) == parentID) return;
        if(m_entityParents.at(id) != 0) {
            auto& siblings = m_entityChildren.at(m_entityParents.at(id));
            siblings.erase(std::remove(siblings.begin(), siblings.end(), id), siblings.end());
        }

        m_entityParents.at(id) = parentID;
        m_entityChildren.at(parentID).push_back(id);
    }


}