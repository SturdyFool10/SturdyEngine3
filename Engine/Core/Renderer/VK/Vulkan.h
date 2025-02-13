//
// Created by sturd on 2/13/2025.
//

#ifndef VULKAN_H
#define VULKAN_H
#include <complex>
#include <expected>

#include "../Renderer.h"


namespace SFT::Renderer::VK {
    class Vulkan : public Renderer {
        private:

        public:
            Vulkan();
            ~Vulkan() override;
            std::expected<void, std::string> Initialize() override;
            void Shutdown() override;
            std::expected<void, std::string> RenderFrame() override;
            std::expected<void, std::string> Resize(int width, int height) override;
    };
} // VK

#endif //VULKAN_H
