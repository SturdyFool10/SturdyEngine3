//
// Created by sturd on 2/24/2025.
//

#ifndef VULKANSHADERPROVIDER_H
#define VULKANSHADERPROVIDER_H

#include <expected>
#include <string>

using std::expected;
using std::string;

namespace SFT::Shaders::VK {
class VulkanShaderProvider {
public:
  VulkanShaderProvider() = default;
  ~VulkanShaderProvider() = default;

  /**
   * \brief Compiles the given GLSL shader code.
   *
   * This function takes the shader code as a C string and compiles it.
   * If there is an error in the GLSL code, it returns an unexpected value with
   * the error message. If the compilation is successful, it returns an expected
   * value with the resulting compiled shader code as a string.
   *
   * \param shader_code The GLSL shader code to compile.
   * \return An expected containing the compiled shader code as a string if
   * successful, or an unexpected containing the error message if there was an
   * error.
   */
  auto compile_shader(const char *shader_code) -> expected<string, string>;
};

} // namespace SFT::Shaders::VK

#endif // VULKANSHADERPROVIDER_H
