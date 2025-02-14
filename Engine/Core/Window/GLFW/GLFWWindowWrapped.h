//
// Created by sturd on 2/13/2025.
//

#ifndef GLFW_H
#define GLFW_H
#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
    #ifdef __WAYLAND__
        #define GLFW_EXPOSE_NATIVE_WAYLAND
    #else
        #define GLFW_EXPOSE_NATIVE_X11
    #endif
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../Window.h"

namespace SFT {
    namespace Window {
        namespace GLFW {
            class GLFWWindowWrapped : public Window {
                private:
                    GLFWwindow* m_window;
                public:
                    virtual ~GLFWWindowWrapped() {};
                    expected<void, string> Create(int width, int height, const string& title) override;
                    void Destroy() override;
                    void* GetNativeWindowHandle() override;
                    void ProcessEvents() override;
                    bool should_close() override;
            };
        } // GLFW
    } // Window
} // SFT

#endif //GLFW_H
