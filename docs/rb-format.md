# RogueEngine RB format

RogueEngine .rb is the native editable asset container. Version 1 uses a compact chunked binary envelope so the format can grow without making the mobile runtime depend on Blender.

## Header

The header is 12 bytes: 4-byte magic RRB1, 4-byte version, and 4-byte chunk count. Integers are little-endian.

## Chunks

Each chunk contains a 4-byte type, a 4-byte unsigned byte count, then the raw payload.

Reserved chunk families are MANF (manifest), GLVB (GLB base payload), SSCN (scene), MRPH (morphs and shape keys), RIGS (rigs), ANIM (animation), PHSY (physics), VSRE (Verse data), TXTR (textures), and MDAT (metadata).

Version 1 currently writes and reads the container and manifest. The remaining chunk serializers will be added as the corresponding RogueEngine asset systems mature.

The container validates the magic, version, chunk count, payload bounds, and trailing bytes before accepting an asset.
