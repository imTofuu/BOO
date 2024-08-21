#pragma once

#include <vector>

namespace BOO {

    // ----------------------------------------------------------------------------------------------------------------\
    //                                                                                                                 |
    // <==< Component Pools >=============================================================================================|
    //                                                                                                                 |
    // ----------------------------------------------------------------------------------------------------------------/

    class IComponentPool {
    public:

        virtual ~IComponentPool() = default;

        virtual std::shared_ptr<IComponentPool> cloneType() = 0;

        virtual size_t addMember() = 0;
        virtual size_t addMember(const void* component) = 0;

        virtual void removeMember(size_t index) = 0;

    };

    template<typename T>
    class ComponentPool final : public IComponentPool {
    public:

        std::shared_ptr<IComponentPool> cloneType() override;

        size_t addMember() override;
        size_t addMember(const void* component) override;

        T* getMember(size_t index);

        void removeMember(size_t index) override;

    private:

        std::vector<T> m_componentData;

    };

    #include "../src/BOO.tpp"

}
