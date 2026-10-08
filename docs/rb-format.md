# RogueEngine RB format

RogueEngine .rb is the native editable asset container. Version 1 is a chunked binary envelope designed to preserve editable RogueEngine data instead of flattening everything to a final GLB.

## Header

The header is 12 bytes:
- 4-byte magic RRB1
- 4-byte little-endian version
- 4-byte little-endian chunk count

Each chunk is:
- 4-byte little-endian type
- 4-byte little-endian payload size
- raw payload bytes

## Version 1 chunk families

- MANF — format/version/name manifest
- GLVB — reserved for a GLB base payload
- SSCN — scene nodes, source metadata, and skin bindings
- MRPH — morph channels, shape keys, slider ranges, categories, body parts, and morph drivers
- RIGS — armatures, bones, rest transforms, and constraints
- ANIM — animation clips, channels, keys, and values
- PHSY — reserved for physics data
- VSRE — reserved for Verse data
- TXTR — embedded named binary blobs, currently used for texture/embedded data
- MDAT — native metadata bytes

## Safety

The reader validates the outer RB container before parsing chunks, applies count and bounds limits to nested arrays, verifies skin-binding index/weight lengths, and rejects malformed chunk payloads or trailing bytes.

## Current state

Version 1 now round-trips the currently defined AssetDocument scene, morph, rig, constraint, animation, embedded-blob, and metadata fields. GLVB, PHSY, and VSRE remain reserved until their corresponding systems are implemented.
