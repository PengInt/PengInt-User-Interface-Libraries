#ifndef PENGINT_FILLIP_HPP
#define PENGINT_FILLIP_HPP


#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <array>
#include <memory>
#include <fstream>

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
    int __debug_count = 0;
    void Debug(const std::string what) { std::cout << "FGE DEBUG [ID " << __debug_count << "]: " << what << std::endl; __debug_count++; }
    std::filesystem::path CurrentProject_fp;
    std::string CompilerPath = "C:\\Program Files\\JetBrains\\CLion 2025.2.4\\bin\\cmake\\win\\x64\\bin\\cmake.exe";
    std::string run_cmd(const std::string& cmd) {
        std::array<char, 128> buf;
        std::string res;
        #if defined(_WIN32)
            std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        #else
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        #endif
        if (!pipe) return "";
        while (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
            res += buf.data();
        }
        if (!res.empty() && res.back() == '\n') res.pop_back();
        if (!res.empty() && res.back() == '\r') res.pop_back();
        return res;
    }
    void AutodetectCompiler() {
        if (const char* cxx_env = std::getenv("CXX")) if (std::filesystem::exists(cxx_env)) { Debug("Compiler path auto-detected in Environment Variables: " + std::string(cxx_env)); CompilerPath = cxx_env; return; }
        #if defined(_WIN32)
            std::vector<std::string> compilers = {"g++.exe", "clang++.exe", "cl.exe"};
        #else
            std::vector<std::string> compilers = {"g++", "clang++"};
        #endif
        for (const std::string& c : compilers) {
            #if defined(_WIN32)
                std::string path = run_cmd("where " + c);
            #else
                std::string path = run_cmd("which " + c);
            #endif
            if (!path.empty() && std::filesystem::exists(path)) { Debug("Compiler path auto-detected in PATH: " + path); std::cout << std::endl; CompilerPath = path; return; }
        }
        Debug("No compiler auto-detected, please insert compiler path into [Fillip Variables/compilerpath.txt] to be able to compile your projects.");
        CompilerPath = "";
    }
    void LoadCompiler() {
        std::filesystem::path txt = std::filesystem::current_path() / "Fillip Variables" / "compilerpath.txt";
        if (!txt.has_parent_path()) std::filesystem::create_directories(txt.parent_path());
        std::fstream file(txt, std::ios::in | std::ios::out);
        if (!file.is_open()) { Debug("Failed to open or create [Fillip Variables/compilerpath.txt] ..."); return; }
        std::uintmax_t size = std::filesystem::file_size(txt);
        std::string buf(size, '\0');
        if (file) file.read(buf.data(), size);
        file.close();
        if (std::filesystem::exists(buf)) { CompilerPath = buf; Debug("Compiler path loaded: " + buf); return; }
        AutodetectCompiler();
        file.clear();
        file.open(txt, std::ios::in | std::ios::out | std::ios::trunc);
        file << CompilerPath;
        file.close();
    }
    UIElementArray* TopBarArray;
    void LOAD_EVERYTHING() {
        std::string f_ext = ".json";
        std::filesystem::path mat_dir = CurrentProject_fp / "Workspace" / "Assets" / "Immediate" / "Materials";
        try {
            if (std::filesystem::exists(mat_dir) && std::filesystem::is_directory(mat_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(mat_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadMaterialFromJSON(entry.path().string().c_str());
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << mat_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        std::filesystem::path mod_dir = CurrentProject_fp / "Workspace" / "Assets" / "Immediate" / "Models";
        try {
            if (std::filesystem::exists(mod_dir) && std::filesystem::is_directory(mod_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(mod_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadObjectFromJSON(entry.path().string().c_str(), "");
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << mod_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        std::filesystem::path ls_dir = CurrentProject_fp / "Workspace" / "Assets" / "Immediate" / "Light Sources";
        try {
            if (std::filesystem::exists(ls_dir) && std::filesystem::is_directory(ls_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(ls_dir)) {
                    if (std::filesystem::is_regular_file(entry) && entry.path().extension() == f_ext) LoadLightSourceFromJSON(entry.path().string().c_str());
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << ls_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }
    void CopyAssets() {
        std::filesystem::path src_dir = std::filesystem::current_path() / CurrentProject_fp / "Workspace" / "Assets";
        std::filesystem::path tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "Workspace" / "Assets";
        try {
            if (std::filesystem::exists(src_dir)) {
                std::filesystem::path workspace_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "Workspace";
                if (std::filesystem::exists(workspace_dir)) std::filesystem::remove_all(workspace_dir);
                std::filesystem::create_directory(workspace_dir);
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
        src_dir = std::filesystem::current_path() / "images";
        tgt_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects" / "Release" / "images";
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
        std::filesystem::path compiled_dir = std::filesystem::current_path() / CurrentProject_fp / "Compiled Projects";
        if (std::filesystem::exists(compiled_dir)) std::filesystem::remove_all(compiled_dir);
        std::filesystem::create_directory(compiled_dir);
        #if defined(_WIN32)
            std::string cmake_exe = "cmd /c cmake";
        #elif defined(__linux__)
            std::string cmake_exe = "cmake";
        #endif
        std::filesystem::path ProjectAbsPath = std::filesystem::current_path() / CurrentProject_fp;
        std::string proj_dir = ProjectAbsPath.string();
        #if defined(_WIN32)
            std::string build_dir = (ProjectAbsPath / "Compiled Projects").string();
        #else
            std::string build_dir = (CurrentProject_fp / "Compiled Projects").string();
        #endif
        Debug("Config (CMake) for: " + proj_dir);
        #if defined(_WIN32)
            std::string cfg_cmd = cmake_exe + " -S \"" + proj_dir + "\" -B \"" + build_dir + "\"\"";
        #elif defined(__linux__)
            std::string cfg_cmd = cmake_exe + " -DCMAKE_BUILD_TYPE=Release -S \"" + CurrentProject_fp.string() + "\" -B \"" + build_dir + "\"";
        #endif
        int cfg_result = std::system(cfg_cmd.c_str());
        if (cfg_result != 0) {
            std::cerr << "Error: Config (CMake) failed!" << std::endl;
            return;
        }
        Debug("Config (CMake) successful! \nStarting compilation.");
        #if defined(_WIN32)
            std::string build_cmd = cmake_exe + " --build \"" + build_dir + "\" --config Release\"";
        #elif defined(__linux__)
            std::string build_cmd = cmake_exe + " --build \"" + build_dir + "\"";
        #endif
        int build_result = std::system(build_cmd.c_str());
        if (build_result == 0) std::cout << "Compilation Successful!"<< std::endl;
        else std::cerr << "Error: Compilation failed during build." << std::endl;
        CopyAssets();
        Debug("If any errors occured during the CMake configuration or the building of the project, please remove all contents of [Fillip Variables/compilerpath.txt] or delete the file, or replace its contents with a path towards your C++ compiler.");
    }
    void DestroyUIElements(std::vector<UIElement*>& remove) {
        for (UIElement* ptr : remove) delete ptr;
        std::erase_if(UIElements, [&remove](UIElement* ptr) {
            return std::find(remove.begin(), remove.end(), ptr) != remove.end();
        });
        std::erase_if(UIButtons, [&remove](UIElement* ptr) {
            return std::find(remove.begin(), remove.end(), ptr) != remove.end();
        });
        remove.clear();
    }
    void LoadExplorerContents(UIElementArrayScrolling* Explorer, bool set_scale) {
        try {
            DestroyUIElements(Explorer->Contents);
            std::filesystem::path fp = std::any_cast<std::filesystem::path>(Explorer->BoundingBox->SpecialMap["cwd"]);
            if (std::filesystem::exists(fp) && std::filesystem::is_directory(fp)) {
                if (!std::filesystem::equivalent(fp, CurrentProject_fp)) Explorer->push_back(new UITextButton({0, 400, 200, 20}, {255, 255, 255, 255}, 25, {0, 0, 0, 0}, "..", {0, 0, 0, 0}, true, [](UIElement* thisbtn) {
                    UIElementArrayScrolling* ex = std::any_cast<UIElementArrayScrolling*>(thisbtn->SpecialMap["Explorer"]);
                    ex->BoundingBox->SpecialMap["cwd"] = std::any_cast<std::filesystem::path>(thisbtn->SpecialMap["target"]);
                    LoadExplorerContents(std::any_cast<UIElementArrayScrolling*>(thisbtn->SpecialMap["Explorer"]), true);
                    return false;
                }, {{"Explorer", Explorer}, {"target", (fp/"..").lexically_normal()}}, "Explorer Element - .."));
                for (const auto& entry : std::filesystem::directory_iterator(fp)) {
                    if (entry.exists()) {
                        std::string text;
                        if (std::filesystem::is_directory(entry)) text = entry.path().filename().string() + "/";
                        else text = entry.path().filename().string();
                        Explorer->push_back(new UITextButton({0, 400, 200, 20}, {255, 255, 255, 255}, 25, {0, 0, 0, 0}, text, {0, 0, 0, 0}, true, [](UIElement* thisbtn) {
                            UIElementArrayScrolling* ex = std::any_cast<UIElementArrayScrolling*>(thisbtn->SpecialMap["Explorer"]);
                            ex->BoundingBox->SpecialMap["cwd"] = std::any_cast<std::filesystem::path>(thisbtn->SpecialMap["target"]);
                            LoadExplorerContents(std::any_cast<UIElementArrayScrolling*>(thisbtn->SpecialMap["Explorer"]), true);
                            return false;
                        }, {{"Explorer", Explorer}, {"target", entry.path()}}, "Explorer Element - " + text));
                    }
                }
                if (set_scale) SetUIScale_Selective(Explorer->Contents);
                Explorer->UpdateBoundingBox();
            } else std::cerr << "Specified path does not exist or is not a directory: " << fp.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }
}

class FillipGameEngineWindow : public Renderer {
    Texture FillipLogo_WT;
    std::chrono::steady_clock::time_point start_time_chrono;
    void FillipSetup() {
        start_time_chrono = std::chrono::steady_clock::now();
        FillipLogo_WT = LoadTexture("images/Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
    }
    void LoadProjectMenu() {
        if (WindowShouldClose()) return;
        CLEAR_BACKHROUND = true;
        RESET_UI();
        PengIntShaderStructs::DESTROY_EVERYTHING();
        UIElementArray* LoadProjects = new UIElementArray(true, 5);
        std::filesystem::path proj_dir = std::filesystem::current_path() / "Projects";
        try {
            if (std::filesystem::exists(proj_dir) && std::filesystem::is_directory(proj_dir)) {
                for (const auto& entry : std::filesystem::directory_iterator(proj_dir)) {
                    if (entry.is_directory()) LoadProjects->push_back(new UITextButton({50, 50, 0, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, entry.path().filename().string(), {255, 255, 255, 255}, true, [](UIElement* thisbtn) {
                        CurrentProject_fp = "Projects/" + std::any_cast<std::string>(thisbtn->SpecialMap["ProjectName"]);
                        return true;
                    }, {{"ProjectName", entry.path().filename().string()}}, "LoadProject - " + entry.path().filename().string()));
                }
            } else std::cerr << "Specified path does not exist or isn't a directory: " << proj_dir.string() << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        SetUIScale();
        while (!WindowShouldClose()) {
            float dt = GetFrameTime();
            if (UI_LOOP(dt, 0)) break;
        }
        RESET_UI();
        LOAD_EVERYTHING();
        UIElementArray* TopBar_File_OnClickArray;
        UIElementArray* TopBar_Edit_OnClickArray;
        UIElementArray* TopBar_VCS_OnClickArray;
        TopBarArray = new UIElementArray(false, 0, {
            new UITextButton({0, 0, 55, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "File", {255, 0, 0, 255}, true, [](UIElement* thisbtn) {
                std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_File_OnClickArray"])->ToggleVisibility();
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_Edit_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_Edit_OnClickArray"])->ToggleVisibility();
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_VCS_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_VCS_OnClickArray"])->ToggleVisibility();
                return false;
            }, {}, "File Button (Top Bar)"),
            new UITextButton({0, 0, 75, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Edit", {0, 0, 255, 255}, true, [](UIElement* thisbtn) {
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_File_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_File_OnClickArray"])->ToggleVisibility();
                std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_Edit_OnClickArray"])->ToggleVisibility();
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_VCS_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_VCS_OnClickArray"])->ToggleVisibility();
                return false;
            }, {}, "Edit Button (Top Bar)"),
            new UITextButton({0, 0, 195, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Version Control", {0, 255, 0, 255}, true, [](UIElement* thisbtn) {
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_File_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_File_OnClickArray"])->ToggleVisibility();
                if ((*std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_Edit_OnClickArray"]))[0]->Visible) std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_Edit_OnClickArray"])->ToggleVisibility();
                std::any_cast<UIElementArray*>(thisbtn->SpecialMap["TopBar_VCS_OnClickArray"])->ToggleVisibility();
                return false;
            }, {}, "VCS Button (Top Bar)")
        });
        TopBar_File_OnClickArray = new UIElementArray(true, 0, {
            new UITextButton({0, 20, 165, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Reload Project", {255, 0, 0, 255}, true, [](UIElement* thisbtn) {
                PengIntShaderStructs::DESTROY_EVERYTHING();
                LOAD_EVERYTHING();
                return false;
            }, {}, "Reload Project Button (File Menu, Top Bar)"),
            new UITextButton({0, 0, 135, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "New Project", {255, 0, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "New Project Button (File Menu, Top Bar)"),
            new UITextButton({0, 0, 145, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Load Project", {255, 0, 0, 255}, true, [](UIElement* thisbtn) { return true; }, {}, "Load Project Button (File Menu, Top Bar)"),
            new UITextButton({0, 0, 175, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Compile Project", {255, 0, 0, 255}, true, [](UIElement* thisbtn) {
                Compile();
                return false;
            }, {}, "Compile Project Button (File Menu, Top Bar)"),
            new UITextButton({0, 0, 175, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Compile Project and Execute", {255, 0, 0, 255}, true, [](UIElement* thisbtn) {
                Compile();
                std::filesystem::path ProjectAbsPath = std::filesystem::current_path() / std::any_cast<std::filesystem::path>(thisbtn->SpecialMap["CurrentProject_fp"]);
                #if defined(_WIN32)
                    std::string command = "cmd /c \"cd /d \"\"" + (ProjectAbsPath/"Compiled Projects" / "Release").string() + "\" && \"" + (ProjectAbsPath/"Compiled Projects" / "Release" / "UserProject.exe").string() + "\"\"";
                #elif defined(__linux__)
                    std::string command = "cd \"" + (ProjectAbsPath/"Compiled Projects" / "Release").string() + "\" && ./UserProject";
                #endif
                int run_result = std::system(command.c_str());
                if (run_result == 0) std::cout << "Execution Successful!" << std::endl;
                else std::cerr << "Error: Execution failed." << std::endl;
                return false;
            }, {{"CurrentProject_fp", CurrentProject_fp}}, "Compile Project and Execute Button (File Menu, Top Bar)"),
            new UITextButton({0, 0, 95, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Settings", {255, 0, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Settings Button (File Menu, Top Bar)"),
        });
        TopBar_File_OnClickArray->ToggleVisibility();
        TopBar_Edit_OnClickArray = new UIElementArray(true, 0, {
            new UITextButton({0, 20, 75, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Search", {0, 0, 255, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Search Button (Edit Menu, Top Bar)"),
            new UITextButton({0, 0, 45, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Cut", {0, 0, 255, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Cut Button (Edit Menu, Top Bar)"),
            new UITextButton({0, 0, 55, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Copy", {0, 0, 255, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Copy Button (Edit Menu, Top Bar)"),
            new UITextButton({0, 0, 65, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Paste", {0, 0, 255, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Paste Button (Edit Menu, Top Bar)"),
        });
        TopBar_Edit_OnClickArray->ToggleVisibility();
        TopBar_VCS_OnClickArray = new UIElementArray(true, 0, {
            new UITextButton({0, 20, 75, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Commit", {0, 255, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Commit Button (VCS Menu, Top Bar)"),
            new UITextButton({0, 0, 55, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Push", {0, 255, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Push Button (VCS Menu, Top Bar)"),
            new UITextButton({0, 0, 55, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Pull", {0, 255, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Pull Button (VCS Menu, Top Bar)"),
            new UITextButton({0, 0, 105, 20}, {255, 255, 255, 255}, 15, {0, 0, 0, 255}, "Configure", {0, 255, 0, 255}, true, [](UIElement* thisbtn) { return false; }, {}, "Configure Button (VCS Menu, Top Bar)"),
        });
        TopBar_VCS_OnClickArray->ToggleVisibility();
        std::unordered_map<std::string, std::any> OnClickArray_SpecialMap = {{"TopBar_File_OnClickArray", TopBar_File_OnClickArray}, {"TopBar_Edit_OnClickArray", TopBar_Edit_OnClickArray}, {"TopBar_VCS_OnClickArray", TopBar_VCS_OnClickArray}};
        (*TopBarArray)[0]->SpecialMap = OnClickArray_SpecialMap;
        (*TopBarArray)[1]->SpecialMap = OnClickArray_SpecialMap;
        (*TopBarArray)[2]->SpecialMap = OnClickArray_SpecialMap;

        UIElementArray* JSON_InspectorPane/* = new UIElementArray({200, 400, 200, 400}, {0, 0, 0, 255}, {255, 255, 255, 255}, {}, "JSON Inspector Pane")*/;
        UIElementArrayScrolling* Explorer = new UIElementArrayScrolling(true, 0, new UIElement({0, 400, 200, 400}, {0, 0, 0, 255}, {255, 255, 255, 255}, {{"cwd", CurrentProject_fp}, {"CurrentProject_fp", CurrentProject_fp}, {"JSON_InspectorPane", JSON_InspectorPane}}, "File Explorer Background"));
        LoadExplorerContents(Explorer, false);

        if (!WindowShouldClose()) EXIT_TO_MENU = true;
        CLEAR_BACKHROUND = false;
        SetUIScale();
        IS_UI_ALREADY_SCALED = true;
    }
    void OnRun() override {
        LoadCompiler();
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
        LoadProjectMenu();
    }
    void LateRun() override {
        (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_File_OnClickArray"]))[0]->POS.x = (*TopBarArray)[0]->POS.x;
        std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_File_OnClickArray"])->UpdateSpacing();
        (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_Edit_OnClickArray"]))[0]->POS.x = (*TopBarArray)[1]->POS.x;
        std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_Edit_OnClickArray"])->UpdateSpacing();
        (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_VCS_OnClickArray"]))[0]->POS.x = (*TopBarArray)[2]->POS.x;
        std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_VCS_OnClickArray"])->UpdateSpacing();
    }
    virtual void Fillip_OnRun() {}
    virtual void Fillip_OnUpdate(float dt, float t) {}
    void OnUpdate_GUI(float dt, float t) override {
        Vector2 MouseM = GetMouseDelta();
        float mvtmult = 1;
        if (IsKeyDown(KEY_LEFT_SHIFT)) mvtmult = 2;
        if (IsKeyDown(KEY_W)) {
            CameraPosition[2] += mvtmult*2*dt*cosf(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*sinf(CameraYaw);
        }
        if (IsKeyDown(KEY_A)) {
            CameraPosition[2] -= mvtmult*2*dt*sinf(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*cosf(CameraYaw);
        }
        if (IsKeyDown(KEY_S)) {
            CameraPosition[2] -= mvtmult*2*dt*cosf(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*sinf(CameraYaw);
        }
        if (IsKeyDown(KEY_D)) {
            CameraPosition[2] += mvtmult*2*dt*sinf(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*cosf(CameraYaw);
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
    void LateUpdate_UI(float dt, float t) override {
        if (IsWindowResized()) {
            (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_File_OnClickArray"]))[0]->POS.x = (*TopBarArray)[0]->POS.x;
            std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_File_OnClickArray"])->UpdateSpacing();
            (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_Edit_OnClickArray"]))[0]->POS.x = (*TopBarArray)[1]->POS.x;
            std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_Edit_OnClickArray"])->UpdateSpacing();
            (*std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_VCS_OnClickArray"]))[0]->POS.x = (*TopBarArray)[2]->POS.x;
            std::any_cast<UIElementArray*>((*TopBarArray)[0]->SpecialMap["TopBar_VCS_OnClickArray"])->UpdateSpacing();
        }
    }
    void OnEnd() override {
        if (EXIT_TO_MENU && !WindowShouldClose()) LoadProjectMenu();
        else { RESET_UI(); PengIntShaderStructs::DESTROY_EVERYTHING(); }
    }
public:
    FillipGameEngineWindow() : Renderer(400, 300, "Fillip Game Engine") {
        FillipSetup();
    }
    FillipGameEngineWindow(std::string GameName) : Renderer(800, 800, GameName) {
        FillipSetup();
    }
};

#endif //PENGINT_FILLIP_HPP