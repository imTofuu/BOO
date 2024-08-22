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

    registry.addComponentToEntity<t1>(entity1);
    t1& t1Comp = registry.getComponentFromEntity<t1>(entity1);
    t1Comp.b = 'j';

    t2& t2Comp = registry.addComponentToEntity<t2>(entity1);

    registry.removeComponentFromEntity<t1>(entity1);

    std::cin.get();
}
