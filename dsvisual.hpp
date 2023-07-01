#ifndef __DS_VISUAL_HPP__
#define __DS_VISUAL_HPP__

#include <dstruct.hpp>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace dsvisual {

void glfw_error_callback(int error, const char* description) {
    DSTRUCT_ASSERT(false);
}

class PlatformManager {


public:
    static PlatformManager & getInstance() {
        static PlatformManager w;
        DSTRUCT_ASSERT(w.__mWindow != nullptr);
        return w;
    }

    GLFWwindow * getWindow() const {
        return __mWindow;
    }

public:
    PlatformManager() {
        glfwSetErrorCallback(dsvisual::glfw_error_callback);
        
        DSTRUCT_ASSERT(glfwInit());

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        //GLFWwindow* __mWindow = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr); // fuck the line
        __mWindow = glfwCreateWindow(1280, 720, "DSVisual", nullptr, nullptr);

        DSTRUCT_ASSERT(__mWindow != nullptr);
        
        glfwMakeContextCurrent(__mWindow);
        glfwSwapInterval(1); // Enable vsync


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(__mWindow, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        DSTRUCT_ASSERT(__mWindow != nullptr);
    }

    PlatformManager(PlatformManager const&)            = delete;
    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)                 = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;

    ~PlatformManager() {
        
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(__mWindow);
        glfwTerminate();
    }

private:
    GLFWwindow *__mWindow;
};


struct Widget {
    virtual void draw() { };
};

class Window {

public:
    Window() = default;

public:
    void addWidget(Widget *wPtr) {
        _mWidgetRenderQ.push(wPtr);
    }

    bool closed() {
        return glfwWindowShouldClose(PlatformManager::getInstance().getWindow());
    }

    void render() {

        auto window = PlatformManager::getInstance().getWindow();;

        DSTRUCT_ASSERT(window != nullptr);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        while (!_mWidgetRenderQ.empty() && !closed()) {
            Widget *w = _mWidgetRenderQ.front();
            _mWidgetRenderQ.pop_front();
            w->draw();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

protected:
    dstruct::Deque<Widget *> _mWidgetRenderQ;
};

template <typename T, size_t N>
class Array : private dstruct::Array<T, N>, public Widget {
public:
    void draw() override {
        
        static bool show_demo_window = true;
        static bool show_another_window = true;

        auto io = ImGui::GetIO();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;
            char title[20] = { "Hello, world!" };

            title[0] += counter % 3;

            ImGui::Begin(title);                // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window! %p", this);
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

    }

};


}

#endif