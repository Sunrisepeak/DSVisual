#include "dsvisual-core.h"

//#include <iostream>
#include <typeinfo>
#include <chrono>

namespace dsvisual {

static void glfw_error_callback(int error, const char* description) {
    DSTRUCT_ASSERT(false);
}

/* --------------------------------------------------------------------------------------------------------- */

std::mutex WindowManager::_mMutex; // TODO: verify/check included to multi-source file

void WindowManager::addWidget(Widget *wPtr) {
    std::lock_guard<std::mutex> _al(_mMutex);
    _mWidgetRenderQ.push(wPtr);
}

void WindowManager::removeWidget(Widget *wPtr) {
    std::lock_guard<std::mutex> _al(_mMutex);
    _mWidgetDestoryTree.push(wPtr);
}

// split impl from class-in to class-out, incomplete-type issue for widget->render
void WindowManager::__render() {

    // DSVisual Basic Setting
    ImGui::GetIO().FontGlobalScale = 1.8f;
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

        auto it = _mWidgetDestoryTree.find(widget);

        if (it == _mWidgetDestoryTree.end()) {
            widget->draw();
            _mWidgetRenderQ.push_back(widget);
        } else {
            _mWidgetDestoryTree.erase(it);
        }

    }

}

/* --------------------------------------------------------------------------------------------------------- */

PlatformManager::PlatformManager() : __mWindow { nullptr }, __mWindowManager{}, __mWindowExited { false } {
    //__platformInit(); // platform resource cann't alloc in twice thread
    __mRenderThread = std::move(std::thread(&PlatformManager::__windowRender, this));
}

PlatformManager::~PlatformManager() {
    __mWindowExited = true;
    if (__mRenderThread.joinable())
        __mRenderThread.join();
    __mWindow = nullptr;
}

PlatformManager & PlatformManager::getInstance() {
    static PlatformManager w;
    return w;
}

// contruct/destory seq for global obj 
WindowManager & PlatformManager::getWindowManagerInstance() {
    return PlatformManager::getInstance().__mWindowManager;
}

bool PlatformManager::windowClosed() { // only-read for __mWindow
    static bool firstAccessFlag = true;
    while (firstAccessFlag && getInstance().__mWindow == nullptr);
    firstAccessFlag = false;
    if (getInstance().__mWindowExited) return true;
    return glfwWindowShouldClose(getInstance().__mWindow);
}

void PlatformManager::__platformInit() {
    
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
    //glfwSetKeyCallback(__mWindow, dsvisual::glfw_key_callback); // regisater key-callback

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

void PlatformManager::__platformDeinit() {

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(__mWindow);
    glfwTerminate();

}

// split impl from class-in to class-out, incomplete-type issue for w->render
void PlatformManager::__windowRender() {
    
    int display_w, display_h;
    ImVec4 clear_color = ImVec4(0.898f, 1.0f, 0.8f, 1.0f);;

    __platformInit();
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
        //ImGui::ShowDemoWindow();

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

        if (PlatformManager::windowClosed()) __mWindowExited = true;
    }
    __platformDeinit();
}

/* --------------------------------------------------------------------------------------------------------- */

Widget::Widget(const std::string name, bool visible) : _mVisible { visible }, _mName { name } {
    PlatformManager::getWindowManagerInstance().addWidget(this);
}

Widget::~Widget() {
    PlatformManager::getWindowManagerInstance().removeWidget(this);
}

bool Widget::getVisible() const { return _mVisible; }
std::string Widget::getName() const { return _mName; }
void Widget::setVisible(bool visible) { _mVisible = visible; }
void Widget::setName(std::string name) { _mName = name; }

void Widget::draw() {

    if (_mVisible == false) return;

    // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Begin(_mName.c_str(), &_mVisible);
    //ImGui::SetNextWindowSize(ImVec2(500, 500));
    // sub-class to impl
    if (ImGui::CollapsingHeader("basic info")) _drawBasicInfoImpl();
    if (ImGui::CollapsingHeader("data visual")) _drawVisualImpl();
    if (ImGui::CollapsingHeader("control")) _drawControlImpl();

    ImGui::End();
}

}