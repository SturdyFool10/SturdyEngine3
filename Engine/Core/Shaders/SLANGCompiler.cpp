//
// Created by sturd on 2/14/2025.
//

#include "SLANGCompiler.h"


namespace SFT::Shaders {
	SLANGCompiler::SLANGCompiler() {
        slang::createGlobalSession(&session);
    }
}