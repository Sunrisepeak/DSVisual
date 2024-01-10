#ifndef __WINDOW_MANAGER_HPP__DSVISUAL
#define __WINDOW_MANAGER_HPP__DSVISUAL

#include <dsv-core/animation.hpp>

// imgui: glfw + opengl3
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#undef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    assert(false);
}

namespace dsvisual {

class WindowManager {

public:
    static void render(AOPSeqVec &aopSeqVec) {
        static WindowManager vm;

        for (auto &aop : aopSeqVec) {
            vm.__render(aop);
        }

    }

private: // bigfive
    WindowManager() { __platformInit(); }
    WindowManager(WindowManager const&)            = delete;
    WindowManager& operator=(WindowManager const&) = delete;
    WindowManager(WindowManager&&)                 = delete;
    WindowManager& operator=(WindowManager&&)      = delete;
    ~WindowManager() { __platformDeinit(); }

    void __render(AOP &aop) {

        int display_w, display_h;
        ImVec4 clear_color = ImVec4(0.0f, 0.3f, 0.4f, 0.25f);

        // Setup DSVisual Theme
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        ImGui::StyleColorsClassic();
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);

        while (hanim::HAnimate::Status::Finished != aop.anim->status()) {
            // 60 fps
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60/*__mFPS*/));

            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // ImGui Demo
            static bool visible = true;
            static bool workArea = true; // unused
            static ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
            // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
            // Based on your use case you may want one or the other.
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(workArea ? viewport->WorkPos : viewport->Pos);
            ImGui::SetNextWindowSize(workArea ? viewport->WorkSize : viewport->Size);
            ImGui::Begin("DSVisual Canvas", &visible, flags);
            {
                hanim::HEngine::PlayFrame(*(aop.anim), *(aop.obj));
            }
            ImGui::End();

            // Real Rendering
            ImGui::Render();
            glfwGetFramebufferSize(__mWindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


            glfwSwapBuffers(__mWindow);
        }
    }

private: // platform
    void __platformInit() {

        glfwSetErrorCallback(glfw_error_callback);
        
        int ret = glfwInit();
        assert(ret == GLFW_TRUE);

        // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

        // transparency hint goes before glfwCreateWindow
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        __mWindow = glfwCreateWindow(1920, 1080, "DSVisual", nullptr, nullptr);

        assert(__mWindow != nullptr);

        glfwMakeContextCurrent(__mWindow);
        glfwSwapInterval(1); // Enable vsync
        //glfwSetKeyCallback(__mWindow, dsvisual::glfw_key_callback); // regisater key-callback

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        //ImNodes::CreateContext(); // imnodes init
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(__mWindow, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void __platformDeinit() {
        if (__mWindow) {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            //ImNodes::DestroyContext(); // imnodes deinit
            ImGui::DestroyContext();

            glfwDestroyWindow(__mWindow);
            glfwTerminate();
        }
        __mWindow = nullptr;
    }

private:
    GLFWwindow *__mWindow;
};

};

#endif