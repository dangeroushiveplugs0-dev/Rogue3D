# RogueEngine Importer

Recognized formats include GLTF, GLB, OBJ and BLEND.

## Blender / Diffeomorphic support

BLEND support is intentionally split from the native runtime. A Blender file is a Blender database rather than a simple interchange format, so RogueEngine should not pretend that it can safely parse arbitrary Blender internals as if they were GLB.

The planned Blend Bridge runs on a Blender-capable host and converts the Blender scene into RogueEngine's neutral AssetDocument. This can expose mesh and UV data, materials, textures, shape keys, body morphs, morph drivers where translatable, armatures, weights, animation, supported constraints, subdivision information and selected custom properties.

The Diffeomorphic repository is GPL-2.0-or-later. RogueEngine will not copy or link its Blender Python implementation into the engine core. The optional bridge is a separate compatibility component with a clean conversion boundary.
