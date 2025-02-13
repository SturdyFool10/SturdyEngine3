# OpenGL Readme

Welcome to the OpenGL renderer: powering SturdyEngine to be compatible with as many places as possible.

## Problems and Incompatibility

Due to OpenGL being sunsetted in late 2018, the best we can do is OpenGL 4.5, this means that the OpenGL renderer cannot
do some of the things that the Vulkan, WebGPU, or DirectX renderers can do

### Known Incompatibilities

- No support for ray tracing
- No support for mesh shaders
- No support for Multithreading
- No support for async compute
- No support for async transfer

these issues aren't fixable, and as such, OpenGL working in theory is only there to allow for apps running SturdyEngine to run on platforms with no support for Vulkan or DX12(eg. a 15 year old laptop with no driver updates since 2018)

### A statement about performance
OpenGL is inherently single threaded, this means that whereas a modern PC has somewhere between 8-48 threads meaning it can handle that same number of tasks at the same time, OpenGL can only use at most 3 threads on the engine side of things(a game logic thread, and a render thread, and a IO thread), Vulkan and DX12 scale and work with many threads which makes them more efficient before we even look at hardware **your app will perform worse and there's nothing I can do about that**

## In short
OpenGL is a last resort, it's not good, it's not efficient, it's not fast, it's not modern, it's not good for anything other than running on old hardware, and even then it's not good for that either. If you can, use Vulkan, WebGPU or DX12, if you can't, use OpenGL, if you can't use OpenGL, you're out of luck, this renderer is the final fallback, and its not going to have the features of the more modern renderers