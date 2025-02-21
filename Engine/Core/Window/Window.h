//
// Created by sturd on 2/13/2025.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <expected>
#include <string>

using std::expected;
using std::string;


namespace SFT::Window {
    class Window {
        public:
            virtual ~Window() {};

            virtual auto Create(int width, int height, const string& title) -> expected<void, string> = 0;
            virtual auto Destroy() -> void = 0;
            virtual auto GetNativeWindowHandle() -> void*  = 0;
            virtual auto ProcessEvents() -> void = 0;
            virtual auto should_close() -> bool = 0;
            virtual auto setBgBlur(bool blur) -> void = 0;
            virtual auto getAPIName() -> string = 0;

    };
} // Window

#endif //WINDOW_H
