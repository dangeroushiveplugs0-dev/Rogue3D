# RogueEngine Rendering

This module defines the platform-neutral rendering contract.

Planned production backends include Vulkan, WebGPU, OpenGL ES, Metal, and Direct3D 12.

The Null backend is implemented first so the architecture can be tested on CI without GPU drivers or platform SDKs.

Design goals:
- Mobile-first without being mobile-only.
- Keep platform APIs out of engine-facing interfaces.
- Allow the same render lifecycle to target different graphics APIs.
- Keep renderer ownership and frame lifecycle explicit.
- Add real GPU backends behind this contract instead of coupling the core to one API.

Future layers will add devices, resources, command submission, shaders, materials, and viewport surfaces.
