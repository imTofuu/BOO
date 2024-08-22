#include "BOO/BOO.h"

namespace BOO {

    Registry::Registry() {
        m_archetypes.emplace(ArchetypeID{}, Archetype());
    }

    EntityID Registry::createEntity() {
        static EntityID nextID;

        EntityID id = nextID++;
        m_entityArchetypes.emplace(id, &m_archetypes.at({}));
        m_archetypes.at({}).f_entityComponentIndices.emplace(id, 0);

        return id;
    }

    void Registry::destroyEntity(EntityID id) {

        for(auto& [ _, pool ] : m_entityArchetypes.at(id)->f_componentStorage) {
            pool->removeMember(m_entityArchetypes.at(id)->f_entityComponentIndices.at(id));
        }

        m_entityArchetypes.at(id)->f_entityComponentIndices.erase(id);
        m_entityArchetypes.erase(id);
    }




}