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
            expected<void, string> GLFWWindowWrapped::Create(const int width, const int height, const string& title) {
                assert(glfwInit() == GLFW_TRUE);
                glfwSetErrorCallback(GLFWErrorPrinter);
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                this->m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
                if (!this->m_window) {
                    return std::unexpected("Failed to create window");
                }
                return {};
            }

            void GLFWWindowWrapped::Destroy() {
                glfwDestroyWindow(this->m_window);
                glfwTerminate();
            }

            void* GLFWWindowWrapped::GetNativeWindowHandle() {
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
            void GLFWWindowWrapped::ProcessEvents() {
                glfwPollEvents();
            }
            bool GLFWWindowWrapped::should_close() {
                return glfwWindowShouldClose(this->m_window);
            }
        } // GLFW
    } // Window
} // SFT