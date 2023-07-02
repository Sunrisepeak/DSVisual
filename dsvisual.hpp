#ifndef __DS_VISUAL_HPP__
#define __DS_VISUAL_HPP__

// std
#include <string>
#include <typeinfo>
#include <thread>
#include <chrono>
#include <mutex>

// DStruct
#include <dstruct.hpp>

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

private:
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

class Widget;
class Window {

private: // bigfive
    Window() { std::thread(&Window::render, this).detach(); }
    Window(Window const&)            = delete;
    Window& operator=(Window const&) = delete;
    Window(Window&&)                 = delete;
    Window& operator=(Window&&)      = delete;

    void render();

public:
    static Window & getInstance() {
        static Window w;
        return w;
    }

    void addWidget(Widget *wPtr) {
        std::lock_guard<std::mutex> _al(_mMutex);
        _mWidgetRenderQ.push(wPtr);
    }

    void removeWidget(Widget *wPtr) {
        std::lock_guard<std::mutex> _al(_mMutex);
        _mWidgetDestoryTree.insert(reinterpret_cast<size_t>(wPtr));
    }

    bool closed() {
        return glfwWindowShouldClose(PlatformManager::getInstance().getWindow());
    }

protected:
    static std::mutex _mMutex;
    dstruct::BSTree<size_t> _mWidgetDestoryTree;
    dstruct::Deque<Widget *> _mWidgetRenderQ;
};

std::mutex Window::_mMutex;

class Widget {

public:
    Widget(const std::string name = "Widget", bool visible = true) :
        _mVisible { visible }, _mName { name } {
        Window::getInstance().addWidget(this);
    }
    
    virtual ~Widget() {
        Window::getInstance().removeWidget(this);
    }

public: // op

    bool getVisible() const { return _mVisible; }
    std::string getName() const { return _mName; }

    void setVisible(bool visible) { _mVisible = visible; }
    void setName(std::string name) { _mName = name; }

public:
    void draw() {

        if (_mVisible == false) return;

        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Begin(_mName.c_str(), &_mVisible);
        ImGui::SetNextWindowSize(ImVec2(500, 500));
        
        // sub-class to impl
        draw_impl();

        ImGui::End();
    }

protected: // top-down interface
    virtual void draw_impl() { /* */ };

protected:
    bool _mVisible;
    std::string _mName;
};


// split impl from class-in to class-out, incomplete-type issue for w->render
void Window::render() {
    while (true) {
        
        // 60 fps
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60 - 10));

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


        // DSVisual Basic Setting
        ImGui::ShowDemoWindow();
        ImGui::GetIO().FontGlobalScale = 1.5f;
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize = ImVec2(400, 300);

        // Render Widget
        float winXPos = 0, winYPos = 0;
        int renderCnt = _mWidgetRenderQ.size();
        while (renderCnt-- && !closed()) {
            
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
                _mWidgetDestoryTree.erase(reinterpret_cast<size_t>(widget));

            if (wdtSize == _mWidgetDestoryTree.size()) {
                widget->draw();
                _mWidgetRenderQ.push_back(widget);
            }

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
}




template <typename T, size_t N>
class Array : public dstruct::Array<T, N>, public Widget {

protected:
    using DStruct = dstruct::Array<T, N>;

public:
    Array() {
        _mName = std::string("Array-") + std::to_string(reinterpret_cast<size_t>(this));
        _mStartIndex = 0;
        _mHighlightIndex = -1;
    }

public: // for user
    T & operator[](int index) {
        if (index < 0)
            index = N + index;
        _mHighlightIndex = index;
        return DStruct::operator[](index);
    }

    T operator[](int index) const {
        if (index < 0)
            index = N + index;
        _mHighlightIndex = index;
        return DStruct::operator[](index);
    }

protected: // interface impl
    void draw_impl() override {
        ImGuiStyle& style = ImGui::GetStyle();
        const float buttonWidth = 50.0f;
        const float buttonHeight = 50.0f;
        const float buttonSpacing = 2.0f;
        const float windowWidth = ImGui::GetWindowWidth();

        auto oldSpacing = style.ItemSpacing;

        style.ItemSpacing = ImVec2(10, 10);
    
        if (ImGui::CollapsingHeader("Member Info")) {
            ImGui::Text("this: %p", this); ImGui::Separator();
            ImGui::Text("_mSize: %ld", N); ImGui::Separator();
            ImGui::Text("_mC Address: %p", this->_mC); ImGui::Separator();
        }
        
        if (ImGui::CollapsingHeader("Data Visual")) {
            for (int i = _mStartIndex; i < N; i++) {
                auto remindWidth = windowWidth - ImGui::GetCursorPosX(); 
                if (i > 0 && remindWidth < buttonWidth + buttonSpacing) {
                    ImGui::Button(std::to_string((*this)[i]).c_str(), ImVec2(remindWidth - buttonSpacing, buttonHeight));
                    break;
                } else {
                    if (i == _mHighlightIndex) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.7f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
                    }
                    ImGui::Button(std::to_string(DStruct::operator[](i)).c_str(), ImVec2(buttonWidth, buttonHeight));
                    if (i == _mHighlightIndex) { ImGui::PopStyleColor(3); /*_mHighlightIndex = -1;*/ }
                    ImGui::SameLine(0, buttonSpacing);
                }
            }
            ImGui::Separator();
            ImGui::SetNextItemWidth(windowWidth);
            ImGui::SliderInt("", &_mStartIndex, 0, N - 1, "Start Index %d"); ImGui::Separator();
        }

        
        style.ItemSpacing = oldSpacing;

    }
protected:
    int _mHighlightIndex;
    int _mStartIndex;
};


}

#endif