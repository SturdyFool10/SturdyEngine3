//
// Created by sturd on 2/22/2025.
//

#ifndef SHADER_H
#define SHADER_H
#include <map>
#include <string>

using std::map;
using std::string;

namespace SFT::Shaders::Shader {

class Shader {
private:
  map<string, const char *> shader_stages;

public:
  Shader();
  ~Shader();
  auto add_stage(const string &stage_name, const char *stage_code) -> void;
  auto get_stage(const string &stage_name) -> const char *;
};

} // namespace SFT::Shaders::Shader

#endif // SHADER_H
