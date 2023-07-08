#ifndef __DSVISUAL_CORE_HPP__DSVISUAL
#define __DSVISUAL_CORE_HPP__DSVISUAL

// std
//#include <iostream>
#include <string>
#include <typeinfo>
#include <thread>
#include <chrono>
#include <mutex>

#include <dstruct.hpp>

// glfw
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace dsvisual {

static void glfw_error_callback(int error, const char* description);
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
        _mWidgetDestoryTree.push(wPtr);
    }

protected:
    static std::mutex _mMutex;
    dstruct::BSTree<Widget *> _mWidgetDestoryTree;
    dstruct::Deque<Widget *> _mWidgetRenderQ;
};

std::mutex WindowManager::_mMutex; // TODO: verify/check included to multi-source file

/* --------------------------------------------------------------------------------------------------------- */

class PlatformManager {
/*
    friend void
    glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
    struct KeyData {
        int key;
        int scancode;
        int action;
    };
*/
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
        if (getInstance().__mWindowExited) return true;
        return glfwWindowShouldClose(getInstance().__mWindow);
    }
/*
    // use unique_lock move auto-lock to out-class (user)
    static std::unique_lock<std::mutex> getAutoLock() {
        std::unique_lock<std::mutex> _al(getInstance().__mPlatformMutex);
        return std::move(_al);
    }

    static KeyData getInputKey() {
        KeyData kd { -1, -1, -1};
        {
            std::unique_lock<std::mutex> _al(getInstance().__mPlatformMutex);
            if (!getInstance().__mKeyDataQueue.empty()) {
                kd = getInstance().__mKeyDataQueue.front();
                getInstance().__mKeyDataQueue.pop();
            }
        }
        return kd;
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
    void __platformInit() {
        
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

    void __platformDeinit() {

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(__mWindow);
        glfwTerminate();

    }

private: // render thread
    void __windowRender();

private:
    //std::mutex __mWindowOwnship;
    GLFWwindow *__mWindow;
    WindowManager __mWindowManager;
    bool __mWindowExited;
    std::thread __mRenderThread;
    //std::mutex __mPlatformMutex;
    //dstruct::Queue<KeyData> __mKeyDataQueue;
};


/* --------------------------------------------------------------------------------------------------------- */


class Widget {

public:
    Widget(const std::string name = "Widget", bool visible = true) :
        _mVisible { visible }, _mName { name } {
        PlatformManager::getWindowManagerInstance().addWidget(this);
    }

    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete; // TODO: to support
    Widget & operator=(const Widget&) = delete;
    Widget & operator=(Widget&&) = delete; // TODO: to support

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
        if (ImGui::CollapsingHeader("basic info")) _drawBasicInfoImpl();
        if (ImGui::CollapsingHeader("data visual")) _drawVisualImpl();
        if (ImGui::CollapsingHeader("control")) _drawControlImpl();
        

        ImGui::End();
    }

protected: // top-down interface
    virtual void _drawBasicInfoImpl() { }
    virtual void _drawVisualImpl() { /* */ }
    virtual void _drawControlImpl() { }

protected:
    bool _mVisible;
    std::string _mName;
};


template <typename T, typename DSType>
class _PrimitiveIterator : public dstruct::PrimitiveIterator<T> {
private:
    using __Self = _PrimitiveIterator;
    using DStruct = dstruct::PrimitiveIterator<T>;

public: // bigfive
    _PrimitiveIterator(T *ptr, DSType *dsPtr) : DStruct(ptr), _mADSPtr { dsPtr } { }

public: // base
    typename DStruct::ReferenceType operator*() const {
        _mADSPtr->_updateIterator(*this, true);
        return DStruct::operator*();
    };

public: // ForwardIterator
    __Self& operator++() { DStruct::_mPointer++; _mADSPtr->_updateIterator(*this); return *this; }
    __Self operator++(int) {
        auto old = *this;
        DStruct::_mPointer++;
        _mADSPtr->_updateIterator(*this);
        return old;
    }
public: // BidirectionalIterator
    __Self& operator--() { DStruct::_mPointer--; _mADSPtr->_updateIterator(*this); return *this; }
    __Self operator--(int) {
        auto old = *this;
        DStruct::_mPointer--;
        _mADSPtr->_updateIterator(*this);
        return old;
    }
public: // RandomIterator
    __Self operator+(const int &n) const { return DStruct::_mPointer + n; };
    __Self operator-(const int &n) const { return DStruct::_mPointer -n; };

protected:
    DSType *_mADSPtr;
};

/* --------------------------------------------------------------------------------------------------------- */

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

// contruct/destory seq for global obj 
WindowManager & PlatformManager::getWindowManagerInstance() {
    return PlatformManager::getInstance().__mWindowManager;
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

static void glfw_error_callback(int error, const char* description) {
    DSTRUCT_ASSERT(false);
}

/*
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    PlatformManager::KeyData kd { 0 };
    if (action == GLFW_PRESS) {
        kd.key = key;
        kd.scancode = scancode;
        kd.action = action;
        auto _al = PlatformManager::getAutoLock();
        PlatformManager::getInstance().__mKeyDataQueue.push(kd);
    }
}
*/
}

#endif