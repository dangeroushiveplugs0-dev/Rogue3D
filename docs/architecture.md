# RogueEngine Architecture

RogueEngine is a mobile-first, cross-platform 3D creation engine. Mobile is the first-class performance target, but the core is designed to run across phones, tablets, desktop operating systems, and the web.

## Core layers

- **Verse** — application/game rules, structural state, UI-facing property data, timelines, and rig logic.
- **Mojo** — hardware-oriented numerical/deformation acceleration layer. Concrete compiler/runtime integration remains an adapter boundary until the toolchain is finalized.
- **WebAssembly** — portable high-speed delivery boundary for low-level systems where appropriate.
- **Rapier (Rust/Wasm)** — rigid bodies, joints, armature constraints, and bounding-box collision.
- **C++/Wasm FEM + XPBD** — soft-body deformation, skin/muscle motion, stretching, and secondary dynamics.

## Platform rule

The engine core must not depend on Android APIs. Platform runtimes are thin adapters:

- Android
- iOS/iPadOS
- Windows
- Linux
- macOS
- Web

Graphics backends are similarly abstracted so the editor can share the same scene/model data across Vulkan, WebGPU, Metal, Direct3D, and OpenGL ES where appropriate.

## Editor

The editor is a responsive web-desktop-style workspace with touch-safe controls. Phone layouts collapse panels; tablets and desktops expand them.

Planned systems include modeling, per-face UV editing, rigging, animation, physics, simulation, materials/shaders, node graphs, and asset import.

## Visual identity

RogueEngine uses an original dark technical interface with electric green accents. The startup experience can procedurally animate glossy green goo from the screen edges into the geometric R logo. This is an original brand treatment inspired by the project's "Rogue" name, not a Marvel asset.

Ambient effects are optional and quality-scaled for weaker devices.
