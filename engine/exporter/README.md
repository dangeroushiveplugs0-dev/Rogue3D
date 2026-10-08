# RogueEngine Exporter

The exporter boundary is format-neutral. Native RB is RogueEngine's editable asset/container format.

RB is designed to retain GLB-compatible scene and mesh data plus RogueEngine-only information such as shape keys, body morphs, animation metadata, physics/deformation settings, Verse properties, and embedded resources.

Standard exports such as GLB/GLTF/OBJ and media exports such as MP4/WebM/image sequences will use dedicated exporters. Video encoding remains separate from the renderer through a frame-capture/media-encoding pipeline.
