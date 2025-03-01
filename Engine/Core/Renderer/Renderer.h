//
// Created by sturd on 2/13/2025.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <complex>
#include <expected>
#include "Core/Window/Window.h"

using std::expected;
using std::string;

typedef expected<bool, string> (*UpdateCallback)(double deltaTime);

namespace SFT::Renderer {
    class Renderer {
        private:
            UpdateCallback m_updateCallback;
        public:
            virtual ~Renderer() {};
            virtual auto Initialize() -> expected<void, string> = 0;
            virtual auto Shutdown() -> void = 0;
            virtual auto RenderFrame() -> expected<void, string> = 0;
            virtual auto Resize(int width, int height) -> expected<void, string> = 0;
            virtual auto SetWindow(Window::Window* window) -> void = 0;
            virtual auto getAPIName() -> string = 0;
    };
} // Renderer

#endif //RENDERER_H
