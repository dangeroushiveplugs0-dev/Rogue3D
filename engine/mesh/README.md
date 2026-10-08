# RogueEngine Mesh

The mesh module is the first geometry representation used by RogueEngine.

Current foundation:
- Indexed vertices.
- Position, normal, and UV coordinates.
- 32-bit triangle indices.
- Basic topology/index validation.
- Clear ownership of CPU-side mesh data.

The data layout is intentionally small and portable. Future layers will add submeshes, material slots, tangents, colors, skin weights, morph targets, editable topology, per-face UV seams, mesh optimization, and GPU upload buffers.
