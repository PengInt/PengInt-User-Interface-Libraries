#ifndef PENGINT_FILLIP_HPP
#define PENGINT_FILLIP_HPP


#include <chrono>
#include <cstdlib>
#include <filesystem>

#include "PengInt_R-GUIL.hpp"


class ObjectTransform {
public:
    std::array<float, 4> Rotation;
    std::array<float, 3> Position;
    ObjectTransform() : Rotation({0,0,0,0}), Position({0,0,0}) {}
};

typedef PengIntShaderStructs::Object ReferenceObject;
class MeshObject {
    ReferenceObject* RefMesh;
    ObjectTransform ObjTransform;
public:
    MeshObject(ReferenceObject* refobj) : RefMesh(refobj), ObjTransform(ObjectTransform()) { }
};

namespace {
    std::string CurrentProject_fp = "Project";
    UIElementArray* TopBar_File_OnClickArray;
    UIElementArray* TopBar_Edit_OnClickArray;
    UIElementArray* TopBar_VCS_OnClickArray;
    std::string CompilerPath = "C:\\Program Files\\JetBrains\\CLion 2025.2.4\\bin\\cmake\\win\\x64\\bin\\cmake.exe";
    void CopyAssets() {
        std::filesystem::path src_dir = std::filesystem::current_path() / CurrentProject_fp / "Workspace" / "Assets";
        std::filesystem::path tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "Assets";
        try {
            if (std::filesystem::exists(src_dir)) {
                if (std::filesystem::exists(tgt_dir)) std::filesystem::remove_all(tgt_dir);
                auto options = std::filesystem::copy_options::recursive;
                std::filesystem::copy(src_dir, tgt_dir, options);
            } else std::cerr << "Error: Source directory doesn't exist: " << src_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }
        src_dir = std::filesystem::current_path() / "shaders";
        tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "shaders";
        try {
            if (std::filesystem::exists(src_dir)) {
                if (std::filesystem::exists(tgt_dir)) std::filesystem::remove_all(tgt_dir);
                auto options = std::filesystem::copy_options::recursive;
                std::filesystem::copy(src_dir, tgt_dir, options);
            } else std::cerr << "Error: Source directory doesn't exist: " << src_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }
        src_dir = std::filesystem::current_path() / "Roboto_Mono";
        tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "Roboto_Mono";
        try {
            if (std::filesystem::exists(src_dir)) {
                if (std::filesystem::exists(tgt_dir)) std::filesystem::remove_all(tgt_dir);
                auto options = std::filesystem::copy_options::recursive;
                std::filesystem::copy(src_dir, tgt_dir, options);
            } else std::cerr << "Error: Source directory doesn't exist: " << src_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }
        src_dir = std::filesystem::current_path() / "Legal Notices";
        tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "Legal Notices";
        try {
            if (std::filesystem::exists(src_dir)) {
                if (std::filesystem::exists(tgt_dir)) std::filesystem::remove_all(tgt_dir);
                auto options = std::filesystem::copy_options::recursive;
                std::filesystem::copy(src_dir, tgt_dir, options);
            } else std::cerr << "Error: Source directory doesn't exist: " << src_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    void Compile() {
        #if defined(_WIN32)
            std::string cmake_exe = "cmd /c \"\"" + CompilerPath + "\"";
        #elif defined(__linux__)
            std::string cmake_exe = "\"" + CompilerPath + "\"";
        #endif
        std::filesystem::path ProjectAbsPath = std::filesystem::current_path() / CurrentProject_fp;
        std::string proj_dir = ProjectAbsPath.string();
        std::string build_dir = (ProjectAbsPath / "Compiled Projects").string();
        std::cout << "Config (CMake) for: " << proj_dir << std::endl;
        #if defined(_WIN32)
            std::string cfg_cmd = cmake_exe + " -S \"" + proj_dir + "\" -B \"" + build_dir + "\"\"";
        #elif defined(__linux__)
            std::string cfg_cmd = cmake_exe + " -S \"" + proj_dir + "\" -B \"" + build_dir + "\"";
        #endif
        int cfg_result = std::system(cfg_cmd.c_str());
        if (cfg_result != 0) {
            std::cerr << "Error: Config (CMake) failed!" << std::endl;
            return;
        }
        std::cout << "Config (CMake) successful! \nStarting compilation." << std::endl;
        #if defined(_WIN32)
            std::string build_cmd = cmake_exe + " --build \"" + build_dir + "\" --config Release\"";
        #elif defined(__linux__)
            std::string build_cmd = cmake_exe + " --build \"" + build_dir + "\" --config Release";
        #endif
        int build_result = std::system(build_cmd.c_str());
        if (build_result == 0) std::cout << "Compilation Successful!"<< std::endl;
        else std::cerr << "Error: Compilation failed during build." << std::endl;
        CopyAssets();
    }
    class TopBar_File_CompileProject : public UITextButton {
    public:
        TopBar_File_CompileProject() : UITextButton({0, 0, 175, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Compile Project", {255, 0, 0, 255}, true, "Compile Project Button (File Menu, Top Bar)") {}
        void OnClick() override {
            Compile();
        }
    };
    class TopBar_File_CompileProjectAndRun : public UITextButton {
    public:
        TopBar_File_CompileProjectAndRun() : UITextButton({0, 0, 175, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Compile Project and Execute", {255, 0, 0, 255}, true, "Compile Project and Execute Button (File Menu, Top Bar)") {}
        void OnClick() override {
            Compile();
            std::filesystem::path ProjectAbsPath = std::filesystem::current_path() / CurrentProject_fp;
            #if defined(_WIN32)
                std::string command = "cmd /c \"cd /d \"\"" + (ProjectAbsPath/"Compiled Projects" / "Release").string() + "\" && \"" + (ProjectAbsPath/"Compiled Projects" / "Release" / "UserProject.exe").string() + "\"\"";
            #elif defined(__linux__)
                std::string command = "cd \"" + (ProjectAbsPath/"Compiled Projects" / "Release").string() + "\" && ./UserProject";
            #endif
            int run_result = std::system(command.c_str());
            if (run_result == 0) std::cout << "Execution Successful!" << std::endl;
            else std::cerr << "Error: Execution failed." << std::endl;
        }
    };
    class TopBar_File : public UITextButton {
    public:
        TopBar_File() : UITextButton({0, 0, 55, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "File", {255, 0, 0, 255}, true, "File Button (Top Bar)") {
            TopBar_File_OnClickArray = new UIElementArray(true, 0, {
                new UITextButton({0, 35, 135, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "New Project", {255, 0, 0, 255}, true, "New Project Button (File Menu, Top Bar)"),
                new UITextButton({0, 0, 145, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Load Project", {255, 0, 0, 255}, true, "Load Project Button (File Menu, Top Bar)"),
                new TopBar_File_CompileProject(),
                new TopBar_File_CompileProjectAndRun(),
                new UITextButton({0, 0, 95, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Settings", {255, 0, 0, 255}, true, "Settings Button (File Menu, Top Bar)"),
            });
            TopBar_File_OnClickArray->ToggleVisibility();
        }
        void OnClick() override {
            TopBar_File_OnClickArray->ToggleVisibility();
            if ((*TopBar_Edit_OnClickArray)[0]->Visible) TopBar_Edit_OnClickArray->ToggleVisibility();
            if ((*TopBar_VCS_OnClickArray)[0]->Visible) TopBar_VCS_OnClickArray->ToggleVisibility();
        }
    };
    class TopBar_Edit : public UITextButton {
    public:
        TopBar_Edit() : UITextButton({0, 0, 75, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Edit", {0, 0, 255, 255}, true, "Edit Button (Top Bar)") {
            TopBar_Edit_OnClickArray = new UIElementArray(true, 0, {
                new UITextButton({0, 35, 75, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Search", {0, 0, 255, 255}, true, "Search Button (Edit Menu, Top Bar)"),
                new UITextButton({0, 0, 45, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Cut", {0, 0, 255, 255}, true, "Cut Button (Edit Menu, Top Bar)"),
                new UITextButton({0, 0, 55, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Copy", {0, 0, 255, 255}, true, "Copy Button (Edit Menu, Top Bar)"),
                new UITextButton({0, 0, 65, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Paste", {0, 0, 255, 255}, true, "Paste Button (Edit Menu, Top Bar)"),
            });
            TopBar_Edit_OnClickArray->ToggleVisibility();
        }
        void OnClick() override {
            if ((*TopBar_File_OnClickArray)[0]->Visible) TopBar_File_OnClickArray->ToggleVisibility();
            TopBar_Edit_OnClickArray->ToggleVisibility();
            if ((*TopBar_VCS_OnClickArray)[0]->Visible) TopBar_VCS_OnClickArray->ToggleVisibility();
        }
    };
    class TopBar_VCS : public UITextButton {
    public:
        TopBar_VCS() : UITextButton({0, 0, 195, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Version Control", {0, 255, 0, 255}, true, "VCS Button (Top Bar)") {
            TopBar_VCS_OnClickArray = new UIElementArray(true, 0, {
                new UITextButton({0, 35, 75, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Commit", {0, 255, 0, 255}, true, "Commit Button (VCS Menu, Top Bar)"),
                new UITextButton({0, 0, 55, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Push", {0, 255, 0, 255}, true, "Push Button (VCS Menu, Top Bar)"),
                new UITextButton({0, 0, 55, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Pull", {0, 255, 0, 255}, true, "Pull Button (VCS Menu, Top Bar)"),
                new UITextButton({0, 0, 105, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Configure", {0, 255, 0, 255}, true, "Configure Button (VCS Menu, Top Bar)"),
            });
            TopBar_VCS_OnClickArray->ToggleVisibility();
        }
        void OnClick() override {
            if ((*TopBar_File_OnClickArray)[0]->Visible) TopBar_File_OnClickArray->ToggleVisibility();
            if ((*TopBar_Edit_OnClickArray)[0]->Visible) TopBar_Edit_OnClickArray->ToggleVisibility();
            TopBar_VCS_OnClickArray->ToggleVisibility();
        }
    };
}

class FillipGameEngineWindow : public Renderer {
    Texture FillipLogo_WT;
    std::chrono::steady_clock::time_point start_time_chrono;
    UIElementArray* TopBarArray;
    void FillipSetup() {
        start_time_chrono = std::chrono::steady_clock::now();
        FillipLogo_WT = LoadTexture("Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
        TopBarArray = new UIElementArray(false, 0, {
            new TopBar_File(),
            new TopBar_Edit(),
            new TopBar_VCS()
        });
    }
    virtual void Fillip_OnRun() {}
    virtual void Fillip_OnUpdate(float dt, float t) {}
    void OnUpdate_GUI(float dt, float t) override {
        if (IsWindowResized()) {
            (*TopBar_File_OnClickArray)[0]->POS.x = (*TopBarArray)[0]->POS.x;
            TopBar_File_OnClickArray->UpdateSpacing();
            (*TopBar_Edit_OnClickArray)[0]->POS.x = (*TopBarArray)[1]->POS.x;
            TopBar_Edit_OnClickArray->UpdateSpacing();
            (*TopBar_VCS_OnClickArray)[0]->POS.x = (*TopBarArray)[2]->POS.x;
            TopBar_VCS_OnClickArray->UpdateSpacing();
        }
        Vector2 MouseM = GetMouseDelta();
        float mvtmult = 1;
        if (IsKeyDown(KEY_LEFT_SHIFT)) mvtmult = 2;
        if (IsKeyDown(KEY_W)) {
            CameraPosition[2] += mvtmult*2*dt*cos(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_A)) {
            CameraPosition[2] -= mvtmult*2*dt*sin(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_S)) {
            CameraPosition[2] -= mvtmult*2*dt*cos(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_D)) {
            CameraPosition[2] += mvtmult*2*dt*sin(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_Q)) CameraPosition[1] -= mvtmult*2*dt;
        if (IsKeyDown(KEY_E)) CameraPosition[1] += mvtmult*2*dt;
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if (IsCursorHidden()) CameraYaw -= MouseM.x*std::numbers::pi/512;
        if (IsCursorHidden()) CameraPitch -= MouseM.y*std::numbers::pi/512;
        if (CameraPitch > std::numbers::pi*0.5f) CameraPitch = std::numbers::pi*0.5f;
        else if (CameraPitch < -std::numbers::pi*0.5f) CameraPitch = -std::numbers::pi*0.5f;
        if (CameraYaw > std::numbers::pi) CameraYaw -= 2*std::numbers::pi;
        else if (CameraYaw < -std::numbers::pi) CameraYaw += 2*std::numbers::pi;
        Fillip_OnUpdate(dt, t);
    }
    void OnRun() override {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        float seconds_left = 1 - std::chrono::duration_cast<std::chrono::seconds>(now-start_time_chrono).count();
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
                DrawTextEx(Roboto_Mono, "A culmination of the Penguin Interactive Visual Libraries", {10, 760}, 30, 0, WHITE);
            EndDrawing();
        }
        seconds_left = 1;
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, {255, 255, 255, (unsigned char) (255*seconds_left)});
                DrawTextEx(Roboto_Mono, "A culmination of the Penguin Interactive Visual Libraries", {10, 760}, 30, 0, {255, 255, 255, (unsigned char) (255*seconds_left)});
            EndDrawing();
        }
        Fillip_OnRun();
    }
    void LateRun() override {
        (*TopBar_File_OnClickArray)[0]->POS.x = (*TopBarArray)[0]->POS.x;
        TopBar_File_OnClickArray->UpdateSpacing();
        (*TopBar_Edit_OnClickArray)[0]->POS.x = (*TopBarArray)[1]->POS.x;
        TopBar_Edit_OnClickArray->UpdateSpacing();
        (*TopBar_VCS_OnClickArray)[0]->POS.x = (*TopBarArray)[2]->POS.x;
        TopBar_VCS_OnClickArray->UpdateSpacing();
    }
public:
    FillipGameEngineWindow() : Renderer(800, 800, "Fillip Game Engine") {
        FillipSetup();
    }
    FillipGameEngineWindow(std::string GameName) : Renderer(800, 800, GameName) {
        FillipSetup();
    }
};

#endif //PENGINT_FILLIP_HPP