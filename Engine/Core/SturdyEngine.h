

#ifndef STURDYENGINE_H
#define STURDYENGINE_H
#include "Renderer/Renderer.h"

namespace SFT {
    class SturdyEngine {
        private:
        //the renderer is to be determined at runtime, so we need a pointer to the base class, this also means we cannot default since the default is just a spec of a general renderer and won't work
        // ReSharper disable once CppUninitializedNonStaticDataMember
        Renderer::Renderer* renderer;
        public:
        SturdyEngine();
    };
} // SFT

#endif //STURDYENGINE_H
