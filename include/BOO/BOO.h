#pragma once

#include <typeindex>
#include <unordered_set>
#include <vector>

template<>
struct std::hash<std::unordered_set<std::type_index>> {
    size_t operator()(const std::unordered_set<std::type_index>& archetypeID) const noexcept {
        size_t hash = 0;
        for(const auto& type : archetypeID) {
            hash ^= std::hash<std::type_index>{}(type) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

namespace BOO {

    using EntityID = uint64_t;

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Component Pools >==========================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    class IComponentPool {
    public:

        virtual ~IComponentPool() = default;

        virtual std::shared_ptr<IComponentPool> cloneType() = 0;

        virtual size_t addMember() = 0;
        virtual size_t addMember(const void* component) = 0;

        virtual void* getMember(size_t index) = 0;

        virtual void removeMember(size_t index) = 0;

    };

    template<typename T>
    class ComponentPool final : public IComponentPool {
    public:

        std::shared_ptr<IComponentPool> cloneType() override;

        size_t addMember() override;
        size_t addMember(const void* component) override;

        void* getMember(size_t index) override;

        void removeMember(size_t index) override;

    private:

        std::vector<T> m_componentData;

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Archetypes >===============================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    using ArchetypeID = std::unordered_set<std::type_index>;

    struct Archetype {

        template<typename T>
        Archetype copyWith();
        template<typename T>
        Archetype copyWithout();

        ArchetypeID f_archetypeID;
        std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> f_componentStorage;
        std::unordered_map<EntityID, size_t> f_entityComponentIndices;

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Registry >=================================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    class IComponentRef;
    template<typename T>
    class ComponentRef;
    template<typename... T>
    class QueryResult;

    class Registry {
    public:

        Registry();

        EntityID createEntity() { return createEntity(0); }
        EntityID createEntity(EntityID parentID);
        void destroyEntity(EntityID id);

        [[nodiscard]] EntityID getEntityParent(EntityID id) const { return m_entityParents.at(id); }
        void setEntityParent(EntityID id, EntityID parentID);
        [[nodiscard]] const std::vector<EntityID>& getEntityChildren(EntityID id) { return m_entityChildren.at(id); }

        template<typename T>
        ComponentRef<T> addComponentToEntity(EntityID id);
        template<typename T>
        [[nodiscard]] ComponentRef<T> getComponentFromEntity(EntityID id);
        template<typename T>
        ComponentRef<T> setComponentOnEntity(EntityID id, const T& component);
        template<typename T>
        [[nodiscard]] bool entityHasComponent(EntityID id);
        template<typename T>
        void removeComponentFromEntity(EntityID id);

        template<typename... T>
        [[nodiscard]] QueryResult<T...> queryAll();
        template<typename... T>
        [[nodiscard]] QueryResult<T...> queryMatch();
        template<typename... T>
        [[nodiscard]] QueryResult<T...> queryAny();

    private:

        friend class IComponentRef;

        template<typename T, bool adding>
        Archetype& getOrCreateArchetype(Archetype& oldArchetype);

        std::unordered_map<ArchetypeID, Archetype> m_archetypes;
        std::unordered_map<EntityID, Archetype*> m_entityArchetypes;

        std::unordered_map<EntityID, std::vector<EntityID>> m_entityChildren;
        std::unordered_map<EntityID, EntityID> m_entityParents;

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Query Result >=============================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    template<typename... T>
    class QueryResult {
    public:

        void add(EntityID id, Registry* registry);

        using ComponentVecType = std::vector<std::tuple<ComponentRef<T>...>>;

        [[nodiscard]] typename ComponentVecType::iterator begin() { return m_components.begin(); }
        [[nodiscard]] typename ComponentVecType::iterator end() { return m_components.end(); }
        [[nodiscard]] typename ComponentVecType::const_iterator begin() const { return m_components.begin(); }
        [[nodiscard]] typename ComponentVecType::const_iterator end() const { return m_components.end(); }
        [[nodiscard]] typename ComponentVecType::const_iterator cbegin() const { return m_components.cbegin(); }
        [[nodiscard]] typename ComponentVecType::const_iterator cend() const { return m_components.cend(); }

    private:

        ComponentVecType m_components;

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Component References >=====================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    class IComponentRef {
    protected:

        IComponentRef(EntityID id, Registry* registry) : p_registry(registry), p_entityId(id) {}
        virtual ~IComponentRef() = default;

        template<typename T>
        T* retrieveComponent();
        template<typename T>
        bool isValid() const;

    private:

        Registry* p_registry;
        EntityID p_entityId;

    };

    template<typename T>
    class ComponentRef final : public IComponentRef {
    public:

        ComponentRef(EntityID id, Registry* registry) : IComponentRef(id, registry) {}

        [[nodiscard]] bool valid() const { return isValid<T>(); }

        [[nodiscard]] T* get() { return retrieveComponent<T>(); }
        [[nodiscard]] const T* get() const { return retrieveComponent<T>(); }

        [[nodiscard]] T* operator->() { return get(); }
        [[nodiscard]] const T* operator->() const { return get(); }

        [[nodiscard]] T& operator*() { return *get(); }
        [[nodiscard]] const T& operator*() const { return *get(); }

        [[nodiscard]] operator T*() { return get(); }
        [[nodiscard]] operator const T*() const { return get(); }

    };
}

#include "../src/BOO.tpp"