#include <iostream>
#include <filesystem>

#include "QED_R-GUIL.hpp"
#include "Workspace/Code/main.hpp"


class GameRenderer : public Renderer {
    std::filesystem::path cwd;
public:
    GameRenderer() : Renderer(800, 800, "UserProject") { cwd = std::filesystem::current_path(); std::cout << cwd.string() << std::endl; UserGameInstance->SRC_Init(); }
    void OnRun() override {
        std::string f_ext = ".json";
        std::filesystem::path mat_dir = cwd / "Workspace" / "Assets" / "Immediate" / "Materials";
        try {
            if (std::filesystem::exists(mat_dir) && std::filesystem::is_directory(mat_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(mat_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadMaterialFromJSON(entry.path().string().c_str());
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << mat_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        std::filesystem::path mod_dir = cwd / "Workspace" / "Assets" / "Immediate" / "Models";
        try {
            if (std::filesystem::exists(mod_dir) && std::filesystem::is_directory(mod_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(mod_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadObjectFromJSON(entry.path().string().c_str(), "");
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << mod_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        std::filesystem::path ls_dir = cwd / "Workspace" / "Assets" / "Immediate" / "Light Sources";
        try {
            if (std::filesystem::exists(ls_dir) && std::filesystem::is_directory(ls_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(ls_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadLightSourceFromJSON(entry.path().string().c_str());
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << ls_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        UserGameInstance->SRC_OnRun();
        CameraPosition = UserGameInstance->CameraPosition;
        CameraPitch = UserGameInstance->CameraPitch;
        CameraYaw = UserGameInstance->CameraYaw;
        CameraRoll = UserGameInstance->CameraRoll;
    }
    void OnUpdate_GUI(float dt, float t) override {
        UserGameInstance->OnUpdate(dt);
        CameraPosition = UserGameInstance->CameraPosition;
        CameraPitch = UserGameInstance->CameraPitch;
        CameraYaw = UserGameInstance->CameraYaw;
        CameraRoll = UserGameInstance->CameraRoll;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "Loading Application" << std::endl;
    try {
        std::filesystem::current_path(std::filesystem::absolute(argv[0]).parent_path());
        std::cout << "Debug: switch cwd. Current cwd: " << std::filesystem::current_path().string() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to switch cwd to argv[0]: " << e.what() << std::endl;
    }
    GameRenderer GameWindow = GameRenderer();
    GameWindow.Run();
    return 0;
}