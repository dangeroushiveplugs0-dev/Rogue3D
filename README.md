# RogueEngine

**RogueEngine** is a mobile-first, cross-platform 3D creation engine. The repository is **Rogue3D**.

The engine is designed for phones and tablets first without sacrificing compatibility with desktop and web platforms.

## Direction

RogueEngine combines:

- High-performance native and WebAssembly systems
- Verse for application/game state, structural logic, UI-facing properties, timelines, and rig logic
- Mojo as a hardware-oriented numerical acceleration boundary
- Rapier through Rust/Wasm for rigid bodies, joints, armature constraints, and bounding-box collision
- C++/Wasm FEM + XPBD for soft-body, skin, muscle, stretching, and secondary dynamics
- A portable rendering/runtime abstraction
- A responsive touch-first, web-desktop-style editor

## Planned editor

Modeling, per-face UV editing, topology tools, shape keys, rigging, IK/FK, animation, physics, soft-body simulation, materials, shader/material node graphs, visual scripting, and asset import.

## Visual identity

RogueEngine has an original dark technical UI with electric-green accents. Its signature startup animation is a GPU-friendly glossy green goo effect that enters from the screen edges and assembles into the geometric **R** logo before revealing the editor.

The visual system is inspired by the project's "Rogue" name but does not depend on Marvel artwork or assets.

## Platform philosophy

Mobile-first does not mean mobile-only:

- Android phones and tablets
- iPhone/iPad
- Windows
- Linux
- macOS
- Web/Wasm

Platform-specific runtimes should remain thin adapters around the portable engine core.

See [docs/architecture.md](docs/architecture.md) and [docs/visual-identity.md](docs/visual-identity.md).
