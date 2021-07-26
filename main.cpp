#include <SFML/Graphics.hpp>

#include "manager.h"

int main()
{
    srand(time(nullptr));
    Manager m {};
    while (1) {
        m.update();
    }
    return 0;
}