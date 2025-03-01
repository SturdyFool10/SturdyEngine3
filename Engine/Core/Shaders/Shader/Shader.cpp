//
// Created by sturd on 2/22/2025.
//

#include "Shader.h"

namespace SFT::Shaders::Shader {

Shader::Shader() {}

Shader::~Shader() {}

auto Shader::add_stage(const string &stage_name, const char *stage_code)
    -> void {
  this->shader_stages[stage_name] = stage_code;
}

auto Shader::get_stage(const string &stage_name) -> const char * {
  return this->shader_stages[stage_name];
}
} // namespace SFT::Shaders::Shader