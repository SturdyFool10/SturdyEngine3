//
// Created by sturd on 2/13/2025.
//

#ifndef GLFW_H
#define GLFW_H
// Platform-specific includes
#ifdef _WIN32
#include <Windows.h>
#include <dwmapi.h>
#include <winuser.h>
#pragma comment(lib, "dwmapi.lib") // Link DWM API automatically
#elif defined(__linux__)
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elif defined(__APPLE__)
#include <Cocoa/Cocoa.h>
#endif

// Platform-specific extensions for GLFW
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "../Window.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <cassert>
#include <expected>
#include <iostream>
#include <string>

/*!
 * @brief OsWindowHandle is a type that represents a window handle for the
 * operating system, in linux we don't know just from the OS type what type the
 * window handle is, and I don't want to need to recompile for wayland or x11 so
 * we use a `void*` and cast it to the correct type when we need it, for the
 * rest of the operating systems, this is a known type at compile time just from
 * the OS type, so on windows it is a `HWND` and on macOS it is a `NSWindow*`
 */
#ifdef __linux__
using OsWindowHandle = void *;
#elif defined(_WIN32)
#include <Windows.h>
using OsWindowHandle = HWND; // Windows
#elif defined(__APPLE__)
#include <AppKit/AppKit.h>
using OsWindowHandle = NSWindow *; // macOS
#endif

using std::cerr;
using std::expected;
using std::string;
using std::unexpected;

namespace SFT {
namespace Window {
namespace GLFW {
class GLFWWindowWrapped : public Window {
private:
  GLFWwindow *m_window;

public:
  ~GLFWWindowWrapped() override { assert(glfwInit() == GLFW_TRUE); };
  auto Create(int width, int height, const string &title, bool use_transparency)
      -> expected<void, string> override;
  auto Destroy() -> void override;
  auto GetNativeWindowHandle() -> expected<OsWindowHandle, string> override;
  auto ProcessEvents() -> void override;
  auto should_close() -> bool override;
  auto setBgBlur(bool blur) -> expected<void, string> override;
  auto getAPIName() -> string override;
  auto get_handle() -> GLFWwindow *;
};
} // namespace GLFW
} // namespace Window
} // namespace SFT

#endif // GLFW_H
