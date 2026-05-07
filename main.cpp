#include <iostream>

#include "PengInt_GUIL.hpp"


/*class RANDOM_BTN : public BTN {
public:
    RANDOM_BTN() : BTN({10, 20}, {60, 30}, {102, 153, 204, 255}) { }
    void OnClick() {
        std::cout << "click" << std::flush;
    }
};*/

int main() {
    // Only keep following line for running from school PC
    //ChangeDirectory("C:/Users/gommet/Documents/GitHub/PengInt-User-Interface-Libraries");
    Renderer renderer = Renderer(800, 800);
    //RANDOM_BTN();
    std::vector<float> vertices = {1, 1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1};
    std::vector<int> triangles = {0, 1, 2, 255, 0, 0, 0, 1, 2, 3, 0, 255, 0, 0, 2, 3, 4, 0, 0, 255, 0, 3, 4, 5, 255, 255, 0, 0, 4, 5, 0, 0, 255, 255, 0, 5, 0, 1, 255, 0, 255, 0};
    PengIntShaderStructs::Object(0, 0, 0, vertices, triangles);
    renderer.Run();
    return 0;
}