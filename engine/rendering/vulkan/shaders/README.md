# Vulkan shaders

RogueEngine keeps GLSL source beside the Vulkan backend and compiles it to SPIR-V in CI.

Current shader set:
- `mesh.vert` — mesh vertex transform and world-space outputs.
- `mesh.frag` — first-pass PBR-style material response.
- `morph_evaluator.comp` — GPU morph evaluation.

The runtime should load SPIR-V generated from these sources; generated binaries are build artifacts and are intentionally not committed.

## Validation

The repository workflow `.github/workflows/vulkan.yml` compiles all three shaders with `glslc`, then configures and builds the Vulkan backend with CMake and runs the native tests.

Android packaging can consume the same SPIR-V artifacts later, keeping shader source and validation identical between desktop CI and the Android runtime.
