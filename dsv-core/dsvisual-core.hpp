#ifndef __DSVISUAL_CORE_HPP__DSVISUAL
#define __DSVISUAL_CORE_HPP__DSVISUAL

// std
#include <string>
#include <typeinfo>
#include <thread>
#include <chrono>
#include <mutex>

// glfw
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace dsvisual {

void glfw_error_callback(int error, const char* description) {
    DSTRUCT_ASSERT(false);
}

class Widget;
class WindowManager {
    friend class PlatformManager;
private: // bigfive
    WindowManager() = default;
    WindowManager(WindowManager const&)            = delete;
    WindowManager& operator=(WindowManager const&) = delete;
    WindowManager(WindowManager&&)                 = delete;
    WindowManager& operator=(WindowManager&&)      = delete;
    ~WindowManager() = default;
/*
    static WindowManager & getInstance() {
        static WindowManager w;
        return w;
    }
*/
    void __render();

public:

    void addWidget(Widget *wPtr) {
        std::lock_guard<std::mutex> _al(_mMutex);
        _mWidgetRenderQ.push(wPtr);
    }

    void removeWidget(Widget *wPtr) {
        std::lock_guard<std::mutex> _al(_mMutex);
        _mWidgetDestoryTree.insert(wPtr);
    }

protected:
    static std::mutex _mMutex;
    dstruct::BSTree<Widget *> _mWidgetDestoryTree;
    dstruct::Deque<Widget *> _mWidgetRenderQ;
};

std::mutex WindowManager::_mMutex; // TODO: verify/check included to multi-source file

/* --------------------------------------------------------------------------------------------------------- */

class PlatformManager {

public:
    static PlatformManager & getInstance() {
        static PlatformManager w;
        return w;
    }

    // contruct/destory seq for global obj 
    static WindowManager & getWindowManagerInstance();

    static bool windowClosed() { // only-read for __mWindow
        static bool firstAccessFlag = true;
        while (firstAccessFlag && getInstance().__mWindow == nullptr);
        firstAccessFlag = false;
        return glfwWindowShouldClose(getInstance().__mWindow);
    }
/*
    // use unique_lock move auto-lock to out-class (user)
    static std::unique_lock<std::mutex> getAutoLock() {
        std::unique_lock<std::mutex> _al(getInstance().__mWindowOwnship);
        return std::move(_al);
    }

    void setWindowDestroy() {
        __mWindowExited = true;
    }
*/

private:
    PlatformManager();
    PlatformManager(PlatformManager const&)            = delete;
    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)                 = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;
    ~PlatformManager() {
        __mWindowExited = true;
        if (__mRenderThread.joinable())
            __mRenderThread.join();
        __mWindow = nullptr;
    }

private: // platform init/deinit: ensure platform resource init/deinit in same thread
    void __platform_init() {
        
        glfwSetErrorCallback(dsvisual::glfw_error_callback);
        
        DSTRUCT_ASSERT(glfwInit());

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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
    }

    void __platform_deinit() {

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(__mWindow);
        glfwTerminate();
    }

private: // render thread
    void __window_render();

private:
    //std::mutex __mWindowOwnship;
    GLFWwindow *__mWindow;
    WindowManager __mWindowManager;
    bool __mWindowExited;
    std::thread __mRenderThread;
};


/* --------------------------------------------------------------------------------------------------------- */


class Widget {

public:
    Widget(const std::string name = "Widget", bool visible = true) :
        _mVisible { visible }, _mName { name } {
        PlatformManager::getWindowManagerInstance().addWidget(this);
    }
    
    virtual ~Widget() {
        PlatformManager::getWindowManagerInstance().removeWidget(this);
    }

public: // op for user

    bool getVisible() const { return _mVisible; }
    std::string getName() const { return _mName; }

    void setVisible(bool visible) { _mVisible = visible; }
    void setName(std::string name) { _mName = name; }

public:
    void draw() {

        if (_mVisible == false) return;

        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Begin(_mName.c_str(), &_mVisible);
        //ImGui::SetNextWindowSize(ImVec2(500, 500));
        
        // sub-class to impl
        if (ImGui::CollapsingHeader("basic info")) draw_basic_info_impl();
        if (ImGui::CollapsingHeader("data visual")) draw_visual_impl();
        if (ImGui::CollapsingHeader("control")) draw_control_impl();
        

        ImGui::End();
    }

protected: // top-down interface
    virtual void draw_basic_info_impl() { }
    virtual void draw_visual_impl() { /* */ }
    virtual void draw_control_impl() { }

protected:
    bool _mVisible;
    std::string _mName;
};

/* --------------------------------------------------------------------------------------------------------- */

// split impl from class-in to class-out, incomplete-type issue for widget->render
void WindowManager::__render() {

    // DSVisual Basic Setting
    ImGui::GetIO().FontGlobalScale = 1.5f;
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMinSize = ImVec2(400, 300);

    // Render Widget
    float winXPos = 0, winYPos = 0;
    int renderCnt = _mWidgetRenderQ.size();
    while (renderCnt--) {
        
        std::lock_guard<std::mutex> _al(_mMutex);

        Widget *widget = _mWidgetRenderQ.front();
        _mWidgetRenderQ.pop_front();
        ImGui::SetNextWindowPos(
            ImVec2(winXPos += 50, winYPos += 50),
            ImGuiCond_FirstUseEver
        );

        // check and del
        auto wdtSize = _mWidgetDestoryTree.size();
        if (wdtSize)
            _mWidgetDestoryTree.erase(widget);

        if (wdtSize == _mWidgetDestoryTree.size()) {
            widget->draw();
            _mWidgetRenderQ.push_back(widget);
        }

    }

}

/* --------------------------------------------------------------------------------------------------------- */

PlatformManager::PlatformManager() : __mWindow { nullptr }, __mWindowManager{}, __mWindowExited { false } {
    //__platform_init(); // platform resource cann't alloc in twice thread
    __mRenderThread = std::move(std::thread(&PlatformManager::__window_render, this));
}

// contruct/destory seq for global obj 
WindowManager & PlatformManager::getWindowManagerInstance() {
    return PlatformManager::getInstance().__mWindowManager;
}

// split impl from class-in to class-out, incomplete-type issue for w->render
void PlatformManager::__window_render() {
    
    int display_w, display_h;
    ImVec4 clear_color = ImVec4(0.898f, 1.0f, 0.8f, 1.0f);;

    __platform_init();
    while (!__mWindowExited) {
        // 60 fps
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60 - 10));

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
        // ImGui::ShowDemoWindow();

        // DSVisual Window
        __mWindowManager.__render();

        // Real Rendering
        ImGui::Render();
        glfwGetFramebufferSize(__mWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(__mWindow);
    }
    __platform_deinit();
}

}

#endif