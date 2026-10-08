# Squeak3D

**Gearsin** is the engine inside **Squeak3D**: a mobile-first, cross-platform 3D creation engine.

The engine is designed for phones and tablets first without sacrificing compatibility with desktop and web platforms.

## Direction

Gearsin combines:

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

Squeak3D uses a playful **Squirrel Girl**-themed woodland-tech direction. The startup and ambient motion language uses **tiny gears** as a recurring motif. Gear animations remain GPU-friendly, optional, quality-scaled, and reduced or disabled for reduced-motion settings.

The previous Rogue/electric-green/geometric-R/goo branding is retired.

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
