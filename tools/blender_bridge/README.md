# RogueEngine Blender Bridge

This executable bridge runs inside Blender and converts a .blend scene into a neutral RogueEngine JSON package.

Preserved data includes scene hierarchy, transforms, armatures, bone parents, shape keys, morph ranges/current values, optional Diffeomorphic-style morph metadata, shape-key drivers, constraints, and basic source metadata.

Diffeomorphic source code is not imported or linked. The bridge reads the Blender result through Blender's public Python API, keeping the native RogueEngine runtime independent of Blender and the Diffeomorphic GPL component.

Example:

    blender -b character.blend --python rogue_export.py -- --output character.roguebridge.json

The next native stage is parsing this package into AssetDocument and packaging it as .rb.