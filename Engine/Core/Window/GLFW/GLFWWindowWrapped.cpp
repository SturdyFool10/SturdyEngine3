//
// Created by sturd on 2/13/2025.
//

#include "GLFWWindowWrapped.h"

#include <assert.h>

#include "GLFW/glfw3.h"

#include <spdlog/spdlog.h>

void GLFWErrorPrinter(int error, const char *description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}
#define IS_WAYLAND() std::getenv("WAYLAND_DISPLAY") != nullptr;

namespace SFT::Window::GLFW {
/*!
 * @brief Creates a window using GLFW
 * @param width The width of the window
 * @param height The height of the window
 * @param title The title of the window
 * @param use_transparency Whether or not to use a transparent framebuffer, this
 * is not supported on all platforms, but if you want to use a blurred window,
 * this must be enabled during creation
 * @return On success, returns void, on failure, returns unexpected with error
 * message
 */
auto GLFWWindowWrapped::Create(const int width, const int height,
                               const string &title, bool use_transparency)
    -> expected<void, string> {
  glfwSetErrorCallback(GLFWErrorPrinter);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  if (use_transparency) {
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  }
  this->m_window =
      glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!this->m_window) {
    return std::unexpected("Failed to create window");
  }
  return {};
}
/*!
 * @brief Destroys the window
 */
auto GLFWWindowWrapped::Destroy() -> void {
  glfwDestroyWindow(this->m_window);
  glfwTerminate();
}
/*!
 * @brief Exposes the native window handle to the consumer of this class
 * @return On success, returns OsWindowHandle, on failure, returns
 * unexpected with error message
 */
auto GLFWWindowWrapped::GetNativeWindowHandle()
    -> expected<OsWindowHandle, string> {
#ifdef _WIN32
  OsWindowHandle handle = glfwGetWin32Window(this->m_window);
  if (handle == nullptr) {
    return unexpected("Failed to get window handle");
  }
  if (handle == reinterpret_cast<OsWindowHandle>(GLFW_NOT_INITIALIZED)) {
    return unexpected("GLFW not initialized");
  }
  if (handle == reinterpret_cast<OsWindowHandle>(GLFW_NO_CURRENT_CONTEXT)) {
    return unexpected("No current context");
  }
  return handle;
#elif defined(__APPLE__)
  OsWindowHandle handle = glfwGetCocoaWindow(this->m_window);
  if (handle == nullptr) {
    return unexpected("Failed to get window handle");
  }
  return handle
#elif defined(__linux__)
  if IS_WAYLAND () {
    OsWindowHandle handle = glfwGetWaylandWindow(this->m_window);
    if (handle == nullptr) {
      return unexpected("Failed to get window handle");
    }
    return handle;
  } else {
    OsWindowHandle handle = glfwGetX11Window(this->m_window);
    if (handle == nullptr) {
      return unexpected("Failed to get window handle");
    }
    return handle;
  }
#endif
}
/*!
 * @brief Processes events for the window
 */
auto GLFWWindowWrapped::ProcessEvents() -> void { glfwPollEvents(); }
/*!
 * @brief Checks if the window should close
 * @return true if the window should close, false otherwise
 */
auto GLFWWindowWrapped::should_close() -> bool {
  return glfwWindowShouldClose(this->m_window);
}
/*!
 * @brief Sets the background blur of the window
 * @param blur Whether to blur the background
 */
auto GLFWWindowWrapped::setBgBlur(bool blur) -> expected<void, string> {
  auto handle_result = GetNativeWindowHandle();
  if (!handle_result) {
    string msg = "Failed to get window handle";
    spdlog::critical(msg);
    return unexpected(msg);
  }
  OsWindowHandle handle = handle_result.value(); // Always use this

#ifdef _WIN32
  HWND hwnd = handle; // Retrieved via glfwGetWin32Window
  if (!hwnd)
    return unexpected("Failed to get window handle");

  HRESULT hr;
  MARGINS margins;
  // Use DwmExtendFrameIntoClientArea with different margins based on blur state
  if (blur) {
    margins = {-1, -1, -1, -1}; // Extend fully into the client area
    hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (SUCCEEDED(hr)) {
      // Define backdrop types (if not already defined)
      enum DwmSystemBackdropType {
        DWMSBT_DISABLE = 1,         // No backdrop
        DWMSBT_MAINWINDOW = 2,      // Mica
        DWMSBT_TRANSIENTWINDOW = 3, // Acrylic
        DWMSBT_TABBEDWINDOW = 4     // Tabbed
      };
      DwmSystemBackdropType backdropType = DWMSBT_TRANSIENTWINDOW;
      hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType,
                                 sizeof(backdropType));
      if (FAILED(hr))
        return unexpected("Failed to set window attribute for blur");
    } else {
      return unexpected("Failed to extend frame into client area for blur");
    }
  } else {
    margins = {1, 1, 1, 1}; // Reset margins to disable the extended frame
    hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (SUCCEEDED(hr)) {
      enum DwmSystemBackdropType {
        DWMSBT_DISABLE = 1,
        DWMSBT_MAINWINDOW = 2,
        DWMSBT_TRANSIENTWINDOW = 3,
        DWMSBT_TABBEDWINDOW = 4
      };
      DwmSystemBackdropType backdropType = DWMSBT_DISABLE;
      hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType,
                                 sizeof(backdropType));
      if (FAILED(hr))
        return unexpected("Failed to disable blur attribute");
    } else {
      return unexpected(
          "Failed to extend frame into client area to disable blur");
    }
  }
#elif defined(__linux__)
  // Linux (Wayland/X11)
  if (IS_WAYLAND()) {
    string msg = "Wayland blur is not yet supported directly.\n";
    // Wayland blur handling (to be implemented)
    spdlog::critical(msg);
    return unexpected(msg);
  } else {
    // X11 KDE KWin blur (needs casting)
    Display *display = glfwGetX11Display();
    Window window = reinterpret_cast<Window>(handle); // Cast from void*
    if (!display || !window)
      return;

    Atom atom = XInternAtom(display, "_KDE_NET_WM_BACKGROUND_BLUR", False);
    if (atom == None)
      return;

    unsigned long value = blur ? 1 : 0;
    XChangeProperty(display, window, atom, XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<unsigned char *>(&value), 1);
    XFlush(display);
  }

#elif defined(__APPLE__)
  // macOS (Core Animation Blur)
  NSWindow *nsWindow = static_cast<NSWindow *>(handle); // No casting needed
  if (!nsWindow)
    return;

  if (blur) {
    [nsWindow setStyleMask:([nsWindow styleMask] |
                            NSWindowStyleMaskFullSizeContentView)];
    [nsWindow setOpaque:NO];
    [nsWindow setBackgroundColor:[NSColor clearColor]];
  } else {
    [nsWindow setOpaque:YES];
    [nsWindow setBackgroundColor:[NSColor windowBackgroundColor]];
  }

#else
  // if not linux, windows, or mac
  string msg = "Blur not supported on this platform.\n";
  spdlog::critical(msg);
  return unexpected(msg);
#endif
  return {};
}
/*!
 * @brief Gets the name of the API used to create the window, this allows for a
 * generic way to tell that we used this class from the base class
 * @return The name of the API
 */
auto GLFWWindowWrapped::getAPIName() -> string { return "GLFW"; }
/*!
 * @brief Gets the handle to the window
 * @return The handle to the window
 */
auto GLFWWindowWrapped::get_handle() -> GLFWwindow * { return this->m_window; }
} // namespace SFT::Window::GLFW
