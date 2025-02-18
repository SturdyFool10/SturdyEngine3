//
// Created by sturd on 2/14/2025.
//

#ifndef SLANGCOMPILER_H
#define SLANGCOMPILER_H

#include "slang/slang.h"
#include "slang/slang-com-ptr.h"
#include "slang/slang-com-helper.h"

namespace SFT::Shaders {
    class SLANGCompiler {
        private:
            SlangSession* session;
        public:
            SLANGCompiler();
            ~SLANGCompiler();
    };
} // Shaders

#endif //SLANGCOMPILER_H
