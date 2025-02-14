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

            virtual expected<void, string> Create(int width, int height, const string& title) = 0;
            virtual void Destroy() = 0;
            virtual void* GetNativeWindowHandle() = 0;
            virtual void ProcessEvents() = 0;
            virtual bool should_close() = 0;
    };
} // Window

#endif //WINDOW_H
