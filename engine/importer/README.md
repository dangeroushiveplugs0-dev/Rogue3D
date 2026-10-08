# RogueEngine Importer

This module defines the portable asset-import boundary.

Recognized formats at the API level:
- GLTF
- GLB
- OBJ

The importer contract intentionally does not expose a third-party decoder's types. A mature open-source decoder can be integrated behind this boundary later while RogueEngine keeps ownership of its own mesh, material, texture, and scene data.

The eventual GLB/GLTF pipeline will preserve meshes, materials, PBR textures, UV sets, transforms, skinning data, morph targets, and scene hierarchy where supported. Raw source data should be converted into RogueEngine-owned structures rather than leaking decoder-specific memory into the runtime.
