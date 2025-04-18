

#ifndef STURDYENGINE_H
#define STURDYENGINE_H
#include <expected>
#define Ok(value) std::expected::expected(value);
#define Err(value) std::unexpected(value);

#include "Renderer/Renderer.h"
#include "Window/Window.h"

namespace SFT {
class SturdyEngine {
private:
  // the renderer is to be determined at runtime, so we need a pointer to the
  // base class, this also means we cannot default since the default is just a
  // spec of a general renderer and won't work
  //  ReSharper disable once CppUninitializedNonStaticDataMember
  Window::Window *window;
  Renderer::Renderer *renderer;
  void main_loop();

public:
  SturdyEngine();
  ~SturdyEngine();
  void run();
};
} // namespace SFT

#endif // STURDYENGINE_H
