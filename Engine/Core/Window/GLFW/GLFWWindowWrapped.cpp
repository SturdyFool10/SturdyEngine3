//
// Created by sturd on 2/13/2025.
//
#include "GLFWWindowWrapped.h"

#include <assert.h>

#include "GLFW/glfw3.h"

void GLFWErrorPrinter(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}



namespace SFT {
    namespace Window {
        namespace GLFW {
            auto GLFWWindowWrapped::Create(const int width, const int height, const string& title) -> expected<void, string> {
                glfwSetErrorCallback(GLFWErrorPrinter);
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                this->m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
                if (!this->m_window) {
                    return std::unexpected("Failed to create window");
                }
                return {};
            }

            auto GLFWWindowWrapped::Destroy() -> void {
                glfwDestroyWindow(this->m_window);
                glfwTerminate();
            }

            auto GLFWWindowWrapped::GetNativeWindowHandle() -> void*  {
                #ifdef _WIN32
                    return glfwGetWin32Window(this->m_window);
                #elif defined(__APPLE__)
                    return glfwGetCocoaWindow(this->m_window);
                #elif defined(__linux__)
                    #ifdef __WAYLAND__
                        return glfwGetWaylandWindow(this->m_window);
                    #else
                        return glfwGetX11Window(this->m_window);
                    #endif
                #endif
            }
            auto GLFWWindowWrapped::ProcessEvents() -> void {
                glfwPollEvents();
            }
            auto GLFWWindowWrapped::should_close() -> bool {
                return glfwWindowShouldClose(this->m_window);
            }

            auto GLFWWindowWrapped::setBgBlur(bool blur) -> void {
                if (blur) {
                    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
                }
                else {
                    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
                }
            }
            auto GLFWWindowWrapped::getAPIName() -> string{
                return "GLFW";
            }
            auto GLFWWindowWrapped::get_handle()-> GLFWwindow*{
                return this->m_window;
            }
        } // GLFW
    } // Window
} // SFT