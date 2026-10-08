# RogueEngine Materials

RogueEngine uses a standard physically based material foundation and exposes additional response controls above it.

Base PBR properties include base color, metallic, roughness, normal strength, occlusion, and emission.

RogueEngine extensions currently reserved in the material model:
- Reflectiveness — additional reflective response control.
- Wetness — can increase specular response and reduce effective roughness.
- Organic — material classification/input for future skin, tissue, plant, and biological shading.
- Dryness — can drive rougher, less reflective surface response and future micro-surface effects.

These values are data-level controls for now. Backend shaders will translate them into lighting calculations. This lets RogueEngine build its own PBR layer on top of established models instead of replacing them.

Future extensions can include subsurface scattering, clear coat, sheen, anisotropy, transmission, thin film, fuzz/fabric, fluid films, and custom shader-node graphs.
