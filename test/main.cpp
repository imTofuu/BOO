#include <BOO/BOO.h>

#include <iostream>

struct t1 {
    int a = 0;
    char b = '\0';
};

int main() {
    t1 comp;
    comp.b = 'a';

    BOO::ComponentPool<t1> pool;
    for(int i = 0; i < 4; i++) {
        pool.addMember();
    }
    pool.addMember(&comp);

    pool.removeMember(2);

    std::shared_ptr<BOO::ComponentPool<t1>> pool2 =
        std::static_pointer_cast<BOO::ComponentPool<t1>>(pool.cloneType());

    std::cin.get();
}
