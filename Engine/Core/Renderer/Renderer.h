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
            virtual expected<void, string> Initialize() = 0;
            virtual void Shutdown() = 0;
            virtual expected<void, string> RenderFrame() = 0;
            virtual expected<void, string> Resize(int width, int height) = 0;
            virtual void SetWindow(Window::Window* window) = 0;
    };
} // Renderer

#endif //RENDERER_H
