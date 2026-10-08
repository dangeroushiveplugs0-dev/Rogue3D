# RogueEngine Blender Bridge Format

The package is versioned with schema = rogueengine.bridge and schema_version = 1.

Scene data contains nodes, armatures, morph channels and constraints. Shape keys store per-vertex XYZ deltas from Basis, slider ranges, current value, category and body-part metadata. Driver records preserve target paths, expressions and source variables.

Diffeomorphic-created morphs commonly become Blender shape keys, rig properties, drivers, or combinations. RogueEngine therefore targets the Blender data result rather than embedding Diffeomorphic's implementation.

Unknown future fields should be ignored by importers. Importers must validate array lengths, counts, numeric ranges and object references before accepting bridge data.