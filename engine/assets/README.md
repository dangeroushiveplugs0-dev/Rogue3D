# Gearsin Asset Document

AssetDocument is the shared high-level representation between import and export.

The native RB pipeline now provides:
- a validated chunk container
- AssetDocument serialization
- AssetDocument deserialization
- scene and source metadata preservation
- shape keys, morph channels, slider ranges and morph drivers
- armatures, bones and constraints
- animation clips and keys
- embedded binary blobs
- native metadata

The container is intentionally extensible. GLB payloads, physics state, Verse data and other future systems have reserved RB chunk families so those features can be added without replacing the file format.
