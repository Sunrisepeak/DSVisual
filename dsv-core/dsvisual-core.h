#ifndef __DSVISUAL_CORE_H__DSVISUAL
#define __DSVISUAL_CORE_H__DSVISUAL

// std
#include <string>
#include <thread>
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
public:
    void addWidget(Widget *wPtr);
    void removeWidget(Widget *wPtr);
private:
    void __render();

protected:
    static std::mutex _mMutex;
    dstruct::BSTree<Widget *> _mWidgetDestoryTree;
    dstruct::Deque<Widget *> _mWidgetRenderQ;
};

/* --------------------------------------------------------------------------------------------------------- */

class PlatformManager {
public:
    static PlatformManager & getInstance();
    // contruct/destory seq for global obj 
    static WindowManager & getWindowManagerInstance();
    static bool windowClosed();
    static void setRootWindowName(std::string name);
private:
    PlatformManager();
    PlatformManager(PlatformManager const&)            = delete;
    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)                 = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;
    ~PlatformManager();
private: // platform init/deinit: ensure platform resource init/deinit in same thread
    void __PlatformInitCheckOnlyOnce();
    void __platformInit();
    void __platformDeinit();
private: // render thread
    void __windowRender();
private:
    GLFWwindow *__mWindow;
    WindowManager __mWindowManager;
    bool __mWindowExited;
    std::thread __mRenderThread;
};


/* --------------------------------------------------------------------------------------------------------- */


class Widget {
public:
    Widget(const std::string name = "Widget", bool visible = true, bool _mFullScreen = false);
    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete; // TODO: to support
    Widget & operator=(const Widget&) = delete;
    Widget & operator=(Widget&&) = delete; // TODO: to support
    virtual ~Widget();
public: // op for user
    bool getVisible() const;
    std::string getName() const;
    void setVisible(bool visible);
    void setName(std::string name);
public:
    void draw();
protected: // top-down interface
    virtual void _drawBasicInfoImpl() { }
    virtual void _drawVisualImpl() { /* */ }
    virtual void _drawControlImpl() { }
    virtual void _drawAboutImpl() { }
protected:
    std::string _mName;
    bool _mVisible;
    bool _mFullScreen;
};

static void glfw_error_callback(int error, const char* description);
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

}

#endif