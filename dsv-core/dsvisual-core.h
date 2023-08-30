#ifndef __DSVISUAL_CORE_H__DSVISUAL
#define __DSVISUAL_CORE_H__DSVISUAL

// std
#include <string>
#include <thread>
#include <mutex>

// DStruct
#include <dstruct.hpp>

// Hanim
#include <Hanim.hpp>

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

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// imgui extend
#include <imnodes.h>

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
    static void waitWindowClosed(unsigned int ms = 100);
    static void setRootWindowName(std::string name);
    static void setRootWindowSize(int width, int height);
private:
    PlatformManager();
    PlatformManager(PlatformManager const&)            = delete;
    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)                 = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;
    ~PlatformManager();
private: // platform init/deinit: ensure platform resource init/deinit in same thread
    static void __PlatformInitCheckOnlyOnce();
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
    Widget(const std::string name = "Widget", bool visible = false, bool _mFullScreen = false);
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
    void setSize(float w, float h);
    void setPos(float x, float y);
public:
    void draw();
protected: // top-down interface
    virtual void _drawBasicInfoImpl() { }
    virtual void _drawVisualImpl() { /* */ }
    virtual void _drawControlImpl() { }
    virtual void _drawAboutImpl() { }
protected:
    void _setAnimate(hanim::HAnimate &anim, hanim::HObject &hObj);
    void _playAnimate();
protected:
    std::string _mName;
    float _mX, _mY;
    float _mW, _mH;
    bool _mVisible;
    bool _mFullScreen;
private:
    std::mutex __mHanimMutex;
    hanim::HAnimate *__mAnimPtr;
    hanim::HObject *__mHObjPtr;
};

static void glfw_error_callback(int error, const char* description);
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

}

#endif