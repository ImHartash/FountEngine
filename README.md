# FountEngine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![DirectX](https://img.shields.io/badge/DirectX-11-9cf.svg)](https://en.wikipedia.org/wiki/DirectX)

**FountEngine** is a custom game engine built in C++ with DirectX 11.  
The architecture draws from the design patterns found in **Source 2**, employing a system‑manager model with strict separation of concerns.  
The project is developed as both a deep study in engine design and a foundation for a complete, self‑contained title.  
Currently a solo endeavor; collaborators with expertise in art, audio, or animation are welcome.

> **Status:** Core systems (logging, input, entity management, D3D11 initialization) are operational. The engine provides a stable foundation for further feature development.

---

## Architectural Overview

FountEngine is structured around a set of independent subsystems, each responsible for a discrete domain. The design avoids global state where practical and favours explicit lifecycle control.

- **Singleton‑style access** — Systems are accessed via `GetInstance()`; each manages its own initialization and shutdown.
- **Compile‑time entity creation** — The `CreateEntity<T>` template enforces inheritance from `IBaseEntity` through `static_assert`, preventing invalid allocations.
- **Input state tracking** — Current and previous key states are maintained, enabling proper edge detection for events like `OnKeyDown` and `OnKeyUp`.

### Core Systems

| System | Responsibility |
|--------|----------------|
| `CLogSystem` | Diagnostic output to console and optional file sinks. |
| `CInputSystem` | Keyboard and mouse state polling; previous‑frame delta. |
| `CEntitySystem` | Entity lifecycle: registration, per‑frame update, and deferred destruction. |
| `D3D11RenderSystem` | Device, swap chain, and immediate context creation; basic frame presentation. |

The design is intended to evolve toward a full `IAppSystem` registry (à la Source 2) with a central `SystemManager` controlling update order and inter‑system dependencies.

---

## Build Instructions

### Prerequisites
- **Visual Studio 2022** (v143 toolchain)
- **Windows 10 SDK** (10.0.19041.0 or later)
- **DirectX 11** runtime (included with Windows SDK)

### Steps
1. Clone the repository:  
   `git clone https://github.com/ImHartash/FountEngine.git`
2. Open `FountEngine.sln` in Visual Studio.
3. Select the desired configuration (`Debug` or `Release`) and platform (`x64`).
4. Build the solution (Build → Build Solution, or `Ctrl+Shift+B`).

Upon successful compilation, the executable will be placed in the `bin/` directory. Running it opens a blank window with an initialized D3D11 context and diagnostic output in the attached console.

---

## 🔗 Related Projects: FountEngine SDK

The engine is developed alongside a dedicated SDK for asset processing and pipeline management.

- **Repository:** [FountEngine_SDK](https://github.com/ImHartash/FountEngine_SDK)

### Purpose & Philosophy

Following the industry‑standard separation of source assets (e.g., `.png`, `.fbx`) and runtime data (e.g., `.fntpk`, `.fnttex`), the SDK provides command‑line tools to compile and package content for the engine. This ensures that the engine runtime consumes only optimized, platform‑ready data.

### Current Toolchain

| Tool | Description | Status |
|------|-------------|--------|
| **fntpk** | Packages asset directories into a single `.fntpk` (Fount Package) file. | Active |
| **fntmdl** | Models for engine into a single `.fntmdl` (Fount Model) file. | Active |
| **fnttex** | Custom texture format designed for optimal GPU upload and storage within the engine. | In Development |

### Roadmap Integration

The SDK is a core component of the engine's future asset pipeline. Upcoming milestones include:
- Integration of `.fntpk` mounting and loading within the engine's virtual file system.
- Implementation of a texture compiler to convert common formats (PNG, TGA) to `.fnttex`.
- Development of a mesh compiler for `.fntmdl` (Fount Model) format.

---

## Development Roadmap

### In Development (will be soon)

---

## Collaboration

The engine is being developed with a concrete game project in mind. Contributions in the following areas are of particular interest:

- 3D asset creation (environment props, characters)
- Audio design and original soundtrack composition
- Character rigging and animation

Inquiries may be directed via [GitHub Issues](https://github.com/ImHartash/FountEngine/issues) or [Pull Requests](https://github.com/ImHartash/FountEngine/pulls).

---

## License

This project is licensed under the terms of the **MIT License**.  
See the [LICENSE](LICENSE) file for details.