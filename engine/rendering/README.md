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


## Morph buffer foundation

The backend-neutral MorphBuffer packs morph deltas into a contiguous byte buffer and keeps per-morph slice metadata. It supports dense and sparse morphs, Float32 and Float16 payloads, and a compact active-morph list. It does not create or bind a graphics API resource; Vulkan, WebGPU, OpenGL ES, Metal, and Direct3D12 backends can upload the packed bytes to their own storage-buffer or equivalent resource. This keeps morph evaluation independent from any one graphics API.
