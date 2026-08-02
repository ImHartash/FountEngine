<div align="center">

# FountEngine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C%2B%2B-26-blue.svg?style=for-the-badge)](https://en.wikipedia.org/wiki/C%2B%2B26)
[![DirectX](https://img.shields.io/badge/DirectX-11-9cf.svg?style=for-the-badge)](https://en.wikipedia.org/wiki/DirectX)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg?style=for-the-badge)](#)

</div>

**FountEngine** is a custom C++26 game engine built on Direct3D 11. Its architecture is loosely inspired by Source 2's system‑manager model: independent, singleton‑style subsystems with explicit lifecycles, slot‑based object pools instead of raw allocation, and a strict split between editable source assets and runtime‑optimized binary formats.

The engine is developed solo, as both a study in engine architecture and the foundation of a complete title. It is paired with a separate content pipeline, [FountEngine SDK](https://github.com/ImHartash/FountEngine_SDK), which compiles art assets into the engine's native runtime formats.

`Русский раздел этого README находится в конце файла → `[Перейти](#русская-версия)

---

## Table of Contents

- [Current State](#current-state)
- [Architecture](#architecture)
  - [System Manager](#system-manager)
  - [Entity System](#entity-system)
  - [Resource System](#resource-system)
  - [File System & Virtual Mounting](#file-system--virtual-mounting)
  - [Rendering Pipeline](#rendering-pipeline)
  - [Input & Logging](#input--logging)
- [Native File Formats](#native-file-formats)
  - [.fntmdl — Model](#fntmdl--model)
  - [.fnttex — Texture](#fnttex--texture)
  - [.fntmat — Material](#fntmat--material)
  - [.fntpk — Package](#fntpk--package)
- [Building](#building)
- [Usage Examples](#usage-examples)
- [FountEngine SDK](#fountengine-sdk)
- [Contributing](#contributing)
- [License](#license)
- [Русская версия](#русская-версия)

---

## Current State

> The engine currently boots into a Direct3D 11 window, mounts a resource package, spawns a single rotating entity built from a compiled model + material, and renders it with a minimal lit/textured pixel shader. Everything below is implemented and functional unless explicitly marked otherwise.

| Area | Status |
| --- | --- |
| Windowing, timer, logging, input polling | ✅ Implemented |
| Entity system (slot-based, handle-safe) | ✅ Implemented |
| Resource system with reference counting | ✅ Implemented |
| Virtual file system with `.fntpk` mounting | ✅ Implemented |
| Native binary formats (`.fntmdl`, `.fnttex`, `.fntmat`) | ✅ Implemented |
| Static-buffer renderer with per-object constant buffer | ✅ Implemented |
| Free-fly camera | ✅ Implemented |
| Simple Lighting (Phong model) | ⏳ In active development |

## Architecture

FountEngine avoids global state where practical. Every subsystem is owned by `CSystemManager`, exposed through a raw global pointer (`g_pXxxSystem`) for cheap access, and initialized/torn down in a fixed order.

```
CSystemManager
 ├─ CLogSystem        — console + file logging, leveled macros
 ├─ CInputSystem       — keyboard/mouse polling with edge detection
 ├─ CFileSystem        — physical + packed (.fntpk) file access
 ├─ CResourceSystem     — reference-counted resource cache
 └─ CEntitySystem      — slot-based entity pool

CGraphicsContext (singleton) — D3D11 device/swapchain/render states
CRenderer (singleton)        — static VB/IB, per-object CB, draw calls
```

### System Manager

`CSystemManager::Initialize()` constructs all five core systems as `unique_ptr` members and publishes raw pointers to globals (`g_pEntitySystem`, `g_pFileSystem`, `g_pInputSystem`, `g_pLogSystem`, `g_pResourceSystem`). This keeps call sites terse (`g_pLogSystem->LogF(...)`) without exposing ownership.

### Entity System

Entities are **not** heap-allocated ad hoc. `CEntitySystem` owns a fixed `std::array<EntitySlot_t, 16384>` free list; `CreateEntity<T>()` is a template that:

- `static_assert`s `T` derives from `IBaseEntity`;
- pops a free slot and bumps its serial number;
- constructs `T` in place via `std::make_unique<T>(args...)`;
- hands back a `CEntityHandle` that packs `index | (serial << 15)`.

This means a stale handle to a destroyed-and-recycled slot is detected by serial mismatch rather than silently pointing at the wrong entity — the same generational-index pattern used by ECS frameworks and Source's `CBaseHandle`.

```cpp
class IBaseEntity {
public:
    virtual void Update(float flDeltaTime) = 0;
    virtual void OnSpawn() = 0;
    virtual void OnDestroy() = 0;
    CEntityHandle GetHandle() const;
};
```

`CBaseModelEntity` extends this with a position, a rotation (`Angle_t`), and handles to a model + material resource — the base class for anything rendered. `CCubeEntity` is the current concrete example: it resolves `models/test_cube.fntmdl` and `materials/test_cube.fntmat` on spawn and spins slowly on its Y axis every frame.

### Resource System

`CResourceSystem::LoadResource(path)` is format-agnostic at the call site: it reads the raw bytes via the file system, inspects the first 4 bytes as a magic number, and dispatches to the matching loader:

| Magic (LE) | ASCII | Loader |
| --- | --- | --- |
| `0x4D544E46` | `FNTM` | `LoadModel` → `CModelResourceData` |
| `0x54544E46` | `FNTT` | `LoadTexture` → `CTextureResourceData` (creates a D3D11 SRV) |
| `0x544D5446` | `FTMT` | `LoadMaterial` → `CMaterialResourceData` |

Resources are cached by path in an `unordered_map<path, CResourceHandle>` backed by a slot vector with reference counting — requesting an already-loaded path just increments a counter and returns the existing handle; `Unload()` decrements it and frees the slot at zero.

### File System & Virtual Mounting

`CFileSystem::ReadFile()` checks mounted `.fntpk` packages first (in mount order), and falls back to the physical `game/resources/` directory. This lets the same resource path resolve either to a loose file on disk during development or to an entry inside a packed archive in a shipped build, with no code changes elsewhere in the engine.

### Rendering Pipeline

`CRenderer` maintains **one** static vertex buffer and **one** static index buffer for the whole scene; each loaded model is appended into them once (`AddToStaticBuffers`) and remembered as a `{vertexOffset, indexOffset, indexCount}` triple in a `GPUCache` map keyed by resource pointer. Per frame:

1. `UpdateSceneComponents` ticks the camera and any per-entity logic.
2. Entities are bucketed into opaque/transparent render lists.
3. For each item, a single `BufferPerObject_t` constant buffer (world-view-projection matrix + packed material data — ambient/diffuse/specular/shininess/opacity) is updated and the draw call issued against the shared buffers.

The vertex/pixel shader pair (`vertex_shader.hlsl` / `pixel_shader.hlsl`, sharing declarations from `common.hlsl`) is intentionally minimal: transform to clip space, pass through normal/UV, sample the diffuse texture, and apply material opacity. Lighting, normal-mapping, and shadowing are not yet implemented.

`CGraphicsContext` owns the device/swapchain/render-target/depth-stencil and three pre-built states each for blend, depth, and rasterizer, selected per-material via `EMaterialBlendMode` / `EMaterialCullMode` / `EMaterialDepthMode`.

### Input & Logging

`CInputSystem` double-buffers key and mouse-button state (`current` vs `previous` arrays of `KEY_COUNT`/`MOUSE_BUTTON_COUNT`) so that `IsKeyPressed`/`IsKeyReleased` can detect edges rather than just held state, plus raw-input mouse delta and cursor locking for camera control.

`CLogSystem` writes leveled, timestamped messages to both a colored console and a log file, guarded by a mutex for thread-safety. The `LOG_INFO` / `LOG_WARNING` / `LOG_ERROR` / `LOG_FATAL` / `LOG_DEBUG` macros automatically capture `__FILE__`/`__LINE__`.

## Native File Formats

All formats are produced exclusively by [FountEngine SDK](https://github.com/ImHartash/FountEngine_SDK) — the engine only ever reads them.

### .fntmdl — Model

Static geometry, produced from Wavefront `.obj` (geometry only — no materials or animation are carried over).

```cpp
struct FNTMDL_HEADER {
    uint32_t nMagic;   // 'FNTM'
    uint32_t nVersion;
    uint32_t nVertexCount;
    uint32_t nIndexCount;
    uint32_t nVertexOffset;
    uint32_t nIndexOffset;
    uint32_t nFlags;
};
```

Followed by a vertex array (position, normal, UV) and a `uint32_t` index array.

### .fnttex — Texture

Uncompressed pixel data with a precomputed mip chain, produced from `.png` / `.jpg` / `.bmp` / `.tga` via `stb_image` + `stb_image_resize2`.

```cpp
struct FNTTEX_HEADER {
    uint32_t nMagic;      // 'FNTT'
    uint32_t nVersion;
    uint32_t nWidth;
    uint32_t nHeight;
    uint32_t nMipLevels;
    uint32_t nDXFormat;   // DXGI_FORMAT, default R8G8B8A8_UNORM (28)
};
```

### .fntmat — Material

A compiled version of a human-authored `.txt` material description (`$DiffuseTexture`, `$Ambient`, `$Diffuse`, `$Specular`, `$Shininess`, blend/cull/depth mode keywords).

```cpp
struct FNTMAT_HEADER { uint32_t nMagic; uint32_t nVersion; };

struct FNTMAT_DATA {
    uint32_t nBlendMode, nCullMode, nDepthMode;
    float flAmbient[3], flDiffuse[3], flSpecular[3];
    float flShininess, flOpacity;
    char  szDiffuseTexture[260];
};
```

### .fntpk — Package

An uncompressed, seek-friendly archive bundling any number of compiled assets for distribution.

```cpp
struct FNTPK_HEADER {
    uint32_t nMagic;        // 'FNTP'
    uint32_t nVersion;
    uint32_t nFlags;        // reserved
    uint32_t nCRC;          // reserved
    uint32_t nBlocksCount;
};

struct FNTPK_BLOCK {
    char     szResourceName[256]; // relative path used as the lookup key
    uint64_t nBlockDataOffset;
    uint32_t nBlockDataSize;
};
```

The engine's `CFntPackFile` loads the whole block table into an `unordered_map<name, entry>` on mount, then seeks + reads individual entries on demand.

## Building

### Prerequisites

- **Visual Studio 2022** (v143 toolchain)
- **Windows 10 SDK** 10.0.19041.0 or later
- DirectX 11 runtime (included with the Windows SDK)

### Steps

```bash
git clone https://github.com/ImHartash/FountEngine.git
```

1. Open `FountEngine.sln` in Visual Studio.
2. Select configuration (`Debug` / `Release`) and platform (`x64`).
3. Build the solution (**Build → Build Solution**, or `Ctrl+Shift+B`).
4. Make sure a `game/resources/` folder exists next to the executable (the engine auto-creates it, but it needs to contain your compiled assets or a mounted `.fntpk`).

Running the executable opens a 1280×720 window titled "FountEngine Test", initializes the D3D11 context, and — with the sample assets in place — renders a slowly rotating cube.

## Usage Examples

Creating an entity:

```cpp
CCubeEntity* pCube = g_pEntitySystem->CreateEntity<CCubeEntity>();
// OnSpawn() runs automatically and resolves its model/material resources.
```

Loading a resource explicitly:

```cpp
CResourceHandle hModel = g_pResourceSystem->LoadResource("models/test_cube.fntmdl");
CModelResourceData* pModel = g_pResourceSystem->GetResource<CModelResourceData>(hModel);
```

Mounting a package before reading from it:

```cpp
g_pFileSystem->MountPakFile("game.fntpk"); // relative to game/resources/
```

## FountEngine SDK

The engine never reads authoring formats directly. The companion [FountEngine SDK](https://github.com/ImHartash/FountEngine_SDK) provides command-line tools that convert source assets into the formats above:

| Tool | Converts |
| --- | --- |
| `FountModelConverter` | Wavefront `.obj` → `.fntmdl` |
| `FountEngineTextureConverter` | `.png` / `.jpg` / `.bmp` / `.tga` → `.fnttex` (with mip generation) |
| `FountEngineMaterialBacker` | Text material description → `.fntmat` |
| `FountEnginePacker` | A directory of compiled assets → a single `.fntpk` |

See the SDK's own [README](https://github.com/ImHartash/FountEngine_SDK#readme) for exact CLI syntax and the full format spec from the tooling side.

## Contributing

This is primarily a solo project, but contributions are welcome, especially in areas outside core engine code:

- 🎨 3D asset creation (props, characters, environments)
- 🎵 Audio design and original soundtrack composition
- 🎬 Character rigging and animation

Reach out via [Issues](https://github.com/ImHartash/FountEngine/issues) or [Pull Requests](https://github.com/ImHartash/FountEngine/pulls).

## License

Licensed under the **MIT License**. See [LICENSE](https://github.com/ImHartash/FountEngine/blob/main/LICENSE) for details.

---

## Русская версия

**FountEngine** — собственный игровой движок на C++26 и Direct3D 11. Архитектура частично вдохновлена моделью систем-менеджеров из Source 2: независимые подсистемы в стиле синглтонов с явным жизненным циклом, слоты вместо произвольных `new`/`delete`, и жёсткое разделение редактируемых исходных ассетов и рантайм-форматов.

Движок разрабатывается одним человеком — как учебный проект по архитектуре движков и одновременно основа для полноценной игры. В паре с ним разрабатывается [FountEngine SDK](https://github.com/ImHartash/FountEngine_SDK) — набор консольных утилит, компилирующих исходные ассеты в нативные бинарные форматы движка.

### Что уже работает

- Окно, таймер, логирование, опрос ввода
- Слотовая entity-система с безопасными хендлами (index + serial)
- Resource-система с ref-counting и кэшированием по пути
- Виртуальная файловая система с монтированием `.fntpk`-пакетов
- Нативные бинарные форматы `.fntmdl` / `.fnttex` / `.fntmat` / `.fntpk`
- Рендерер на общих статических буферах со per-object константным буфером
- Свободная камера

**Не реализовано:** процедурная генерация геометрии (куб/грид — сейчас заглушка), сжатие текстур, скелетная анимация.

### Сборка

Требуется Visual Studio 2022 (v143), Windows 10 SDK 10.0.19041.0+, DirectX 11 runtime.

```bash
git clone https://github.com/ImHartash/FountEngine.git
```

Откройте `FountEngine.sln`, выберите конфигурацию (`Debug`/`Release`) и платформу `x64`, соберите решение (`Ctrl+Shift+B`). Рядом с исполняемым файлом должна быть папка `game/resources/` со скомпилированными ассетами или смонтированным `.fntpk`.

### Форматы файлов

Подробное описание бинарных форматов (`.fntmdl`, `.fnttex`, `.fntmat`, `.fntpk`) — см. английский раздел выше, структуры полей идентичны для обеих версий README.

### Участие в проекте

Открыт вклад в направлениях вне основного кода движка: 3D-ассеты, звук/музыка, риггинг и анимация персонажей. Пишите через [Issues](https://github.com/ImHartash/FountEngine/issues) или [Pull Requests](https://github.com/ImHartash/FountEngine/pulls).

### Лицензия

Проект распространяется под лицензией **MIT** — см. [LICENSE](https://github.com/ImHartash/FountEngine/blob/main/LICENSE).
