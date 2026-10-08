# Vulkan morph backend

This directory contains the optional Vulkan adapter for RogueEngine's backend-neutral MorphBuffer.

VulkanMorphBuffer owns a Vulkan storage buffer and exposes GPU-friendly morph slice and active-morph metadata.

Enable it with ROGUE_ENABLE_VULKAN. The initial implementation uses host-visible coherent memory so the adapter stays small and easy to validate. A later allocator can move the same interface to device-local memory with a staging upload.
