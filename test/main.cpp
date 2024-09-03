#include <BOO/BOO.h>

#include <iostream>

struct t1 {
    int a = 0;
    char b = '\0';
};

struct t2 {
    float c = 0;
};

int main() {

    BOO::Registry registry;

    BOO::EntityID entity1 = registry.createEntity();
    BOO::ComponentRef<t1> ref = registry.addComponentToEntity<t1>(entity1);
    ref->b = 'h';

    for(int i = 0; i < 100000; i++) {
        BOO::EntityID id = registry.createEntity();
        registry.addComponentToEntity<t1>(id);
        registry.addComponentToEntity<t2>(id);
    }

    for(int i = 0; i < 10; i++) {
        BOO::EntityID id = registry.createEntity();
        registry.addComponentToEntity<t1>(id);
    }

    for(int i = 0; i < 10; i++) {
        BOO::EntityID id = registry.createEntity();
        registry.addComponentToEntity<t2>(id);
    }

    for(auto& [ t1Comp, t2Comp ] : registry.queryAny<t1, t2>()) {
        if(t1Comp) t1Comp->a = 53634;
    }

    std::cin.get();
}
