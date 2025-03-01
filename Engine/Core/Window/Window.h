//
// Created by sturd on 2/13/2025.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <expected>
#include <string>

using std::expected;
using std::string;

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

namespace SFT::Window {
/*!
 * @brief The Window class is an abstract class that represents a window, this
 * is used to abstract the window creation and destruction process, there are
 * ways to figure out which subclass is used, see the getAPIName function
 */
class Window {
public:
  virtual ~Window() {};
  /*!
   *
   * @param width
   * @param height
   * @param title
   * @param use_transparency default is false
   * @return
   */
  virtual auto Create(int width, int height, const string &title,
                      bool use_transparency = false)
      -> expected<void, string> = 0;
  /*!
   * @brief Destroys the window
   */
  virtual auto Destroy() -> void = 0;
  /*!
   * @brief Exposes the native window handle to the consumer of this class
   * @return On success, returns OsWindowHandle, on failure, returns
   * unexpected with error message
   */
  virtual auto GetNativeWindowHandle() -> expected<OsWindowHandle, string> = 0;
  /*!
   * @brief Processes events for the window
   */
  virtual auto ProcessEvents() -> void = 0;
  /*!
   * @brief Checks if the window should close
   * @return true if the window should close, false otherwise
   */
  virtual auto should_close() -> bool = 0;
  /*!
   * @brief Sets the background blur of the window
   * @param blur true to blur the background, false to not blur the background
   * @return On success, returns void, on failure, returns unexpected with error
   * message within a string
   */
  virtual auto setBgBlur(bool blur) -> expected<void, string> = 0;
  /*!
   * @brief Gets the name of the API used to create the window
   * @return the name of the API used to create the window
   */
  virtual auto getAPIName() -> string = 0;
};
} // namespace SFT::Window

#endif // WINDOW_H
