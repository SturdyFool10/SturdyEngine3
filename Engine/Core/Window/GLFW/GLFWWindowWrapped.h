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
#include <cassert>
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
                    ~GLFWWindowWrapped() override {
                        assert(glfwInit() == GLFW_TRUE);
                    };
                    auto Create(int width, int height, const string& title) -> expected<void, string> override;
                    auto Destroy() -> void override;
                    auto GetNativeWindowHandle() -> void* override;
                    auto ProcessEvents() -> void override;
                    auto should_close() -> bool override;
                    auto setBgBlur(bool blur) -> void override;
                    auto getAPIName() -> string override;
                    auto get_handle() -> GLFWwindow*;
            };
        } // GLFW
    } // Window
} // SFT

#endif //GLFW_H
