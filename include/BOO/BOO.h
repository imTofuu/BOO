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

        [[nodiscard]] virtual std::shared_ptr<IComponentPool> cloneType() = 0;

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

    class Registry {
    public:

        Registry();

        EntityID createEntity();
        void destroyEntity(EntityID id);

        template<typename T>
        ComponentRef<T> addComponentToEntity(EntityID id);
        template<typename T>
        ComponentRef<T> getComponentFromEntity(EntityID id);
        template<typename T>
        ComponentRef<T> setComponentOnEntity(EntityID id, const T& component);
        template<typename T>
        bool entityHasComponent(EntityID id);
        template<typename T>
        void removeComponentFromEntity(EntityID id);

    private:

        friend class IComponentRef;

        template<typename T, bool adding>
        Archetype& getOrCreateArchetype(Archetype& oldArchetype);

        std::unordered_map<ArchetypeID, Archetype> m_archetypes;
        std::unordered_map<EntityID, Archetype*> m_entityArchetypes;

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Component References >=====================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    class IComponentRef {
    protected:

        IComponentRef(EntityID id, Registry* registry) : p_entityId(id), p_registry(registry) {}
        virtual ~IComponentRef() = default;

        template<typename T>
        T* retrieveComponent();
        template<typename T>
        [[nodiscard]] bool isValid() const;

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

        T* operator->() { return get(); }
        const T* operator->() const { return get(); }

        T& operator*() { return *get(); }
        const T& operator*() const { return *get(); }

        operator T*() { return get(); }
        operator const T*() const { return get(); }

    };

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Pointer Maps >=============================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    template<typename KType, typename VType>
    class PointerMap {
    public:

        VType* getOrNull(const KType& key);
        VType* getOrThrow(const KType& key);

        void emplace(const KType& key, VType& value);
        void put(const KType& key, VType& value);

    private:

        std::unordered_map<EntityID, VType*> m_map;

    };

}

#include "../src/BOO.tpp"