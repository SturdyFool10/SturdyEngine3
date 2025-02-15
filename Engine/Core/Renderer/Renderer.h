//
// Created by sturd on 2/13/2025.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <complex>
#include <expected>

typedef std::expected<bool, std::string> (*UpdateCallback)(double deltaTime);

namespace SFT::Renderer {
    class Renderer {
        private:
            UpdateCallback m_updateCallback;
        public:
            virtual ~Renderer() {};
            virtual std::expected<void, std::string> Initialize() = 0;
            virtual void Shutdown() = 0;
            virtual std::expected<void, std::string> RenderFrame() = 0;
            virtual std::expected<void, std::string> Resize(int width, int height) = 0;
    };
} // Renderer

#endif //RENDERER_H
