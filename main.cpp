#include <iostream>

#include "PengInt_GUIL.hpp"


class RANDOM_BTN : public BTN {
public:
    RANDOM_BTN() : BTN({10, 20}, {60, 30}, {102, 153, 204, 255}) { }
    void OnClick() {
        std::cout << "click" << std::flush;
    }
};

int main() {
    ChangeDirectory("C:/Users/gommet/Documents/GitHub/PengInt-User-Interface-Libraries");
    Renderer renderer = Renderer(800, 800);
    RANDOM_BTN();
    renderer.Run();
    return 0;
}