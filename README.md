# MiladExternal

External cheat for Roblox (`RobloxPlayerBeta.exe`) — memory reading, DirectX 11 overlay menu, file-based configs, and a local HTTP bridge for AI tooling.

## Features

**Combat — Aim**
- Aimbot with FOV circle (size, color, fill), smoothing, 8 aim targets
- Aim methods: Memory, Viewport, Raycast, PF Silent + Magic Bullet
- Prediction: global X/Y, per-weapon Fallen ballistics (auto-detect, BV override, gravity mult)
- Triggerbot with adjustable delay, Deadzone, Spread modifier + Hit Chance
- Target checks: visible, knock, forcefield, spectate, min-health, auto-switch, sticky aim
- Silent tracer + prediction line overlays

**Visual — ESP**
- Boxes (static/dynamic, filled, gradient), names, distance, health bar, tool, flags
- Skeleton (thickness + outline), head dot, view direction
- Mesh chams (DX11 shader modes), dead/local-player filters
- World: ores, plants, animals, soldiers, tools (3D box) with per-type filters
- Lighting: clock time, brightness, ambient/outdoor ambient, fog, exposure, skybox presets, wireframe

**Mics — Movement & Local**
- Fly (4 modes, speed, vertical boost, damping), Noclip (2 modes), FOV changer
- BunnyHop, WalkSpeed, Gravity, HipHeight, JumpPower, Spiderman, No Fall Damage
- Hitbox expander (XYZ + team/knock checks), tickrate, freecam
- Team check, stream-proof overlay, per-feature keybinds (Hold / Toggle / Always)

**Menu & Overlay**
- Custom immediate-mode menu on a transparent DX11 overlay, `INSERT` to toggle
- Keybind list + watermark bars (watermark elements are pickable: brand, FPS, version, players, ping, clock, binds)
- Explorer (instance tree, scripts, decompile/disassemble hooks), Players list
- UI theme is fully editable in-app (background, panels, controls, accent, text)

**Configs**
- Save / Load / Export (clipboard) / Import, stored as `.config` next to the exe in `configs\`
- `default.config` auto-loads on startup — set it once and forget it

**MCP Bridge**
- Local HTTP server on `127.0.0.1:3847` (toggle in Settings → Windows)
- Endpoints: `/health` `/info` `/children` `/search` `/path` — query the live DataModel (`addr` / `path` / `name` params), built for AI-assisted reversing sessions

## Build

- Windows 10/11, Visual Studio with the **v145** platform toolset + Windows 10 SDK
- Open `External.sln`, select **Release | x64**, build
- Run `x64\Release\External.exe` **as administrator** with Roblox open

## Layout

```
External/
  main.cpp                  console status board, elevation, crash handler
  ext/imgui/                dear imgui (bundled)
  ext/stb/                  stb_image (bundled)
  src/
    core/app/               init, main loop, service refresh
    core/cache/             player / PF / CB / ops / workspace caches
    core/features/mcp/      MCP bridge (protocol, handlers, server)
    core/features/mesh/     mesh parsing, cache, chams, DX shader
    core/features/worldgeo/ wireframe world geometry

    core/functions/aim/     aimbot, prediction, silent methods
    core/functions/combat/  combat loop
    core/functions/explorer/ instance explorer + dex assets
    core/functions/freecam/ freecam
    core/functions/mics/    local + misc menus and ticks
    core/functions/movement/ movement loop
    core/functions/settings/ all menu pages + config save/load
    core/functions/visual/  ESP renderer
    core/functions/world/   world visuals + lighting loop
    core/globals/           shared DataModel/workspace/camera state
    core/keys/              keybind gating (hold/toggle/always)
    core/logger/            desktop log + crash reports
    core/net/               ping
    core/tp_handler/        teleport handler thread
    core/variables/         every tunable, single header
    memory/                 process attach + RPM/WPM wrapper
    render/                 overlay window, D3D11, custom menu widgets
    sdk/                    offsets, math, RbxInstance helpers
```

## Controls

| Action | Default |
|---|---|
| Menu | `INSERT` |
| Aimbot | Right mouse (hold) |
| Triggerbot | `M5` |

Everything rebindable in-app with Hold / Toggle / Always modes.
