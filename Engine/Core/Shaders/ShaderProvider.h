//
// Created by sturd on 2/24/2025.
//

#ifndef SHADERPROVIDER_H
#define SHADERPROVIDER_H
#include <expected>
#include <string>

using std::expected;
using std::string;

namespace SFT::Shaders {

class ShaderProvider {
public:
  ShaderProvider() = default;
  ~ShaderProvider() = default;

  /*# compile_shader

   */
  virtual auto compile_shader(const char *shader_code)
      -> expected<string, string>;
};

} // namespace SFT::Shaders

#endif // SHADERPROVIDER_H
