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

    t1 newComponent;
    newComponent.b = 'j';

    registry.setComponentOnEntity(entity1, newComponent);

    std::cin.get();
}
