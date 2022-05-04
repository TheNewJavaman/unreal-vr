[![](https://img.shields.io/static/v1?label=Homepage&message=unrealvr.dev&logo=&color=2b2b2b)](https://unrealvr.dev/)
[![](https://img.shields.io/static/v1?label=Discord&message=Flatscreen%20To%20VR&color=5865F2&logo=Discord&logoColor=white)](https://discord.gg/6SEy6WmP5t)
[![](https://img.shields.io/static/v1?label=Patreon&message=gpizarro&color=FF424D&logo=Patreon&logoColor=white)](https://www.patreon.com/gpizarro)

# UnrealVR

**No releases yet**

Adds virtual reality support to flatscreen Unreal Engine games

## Features

- **Stereoscopic Rendering**: Presents the game in full 3D, not a flatscreen in VR
- **6 Degrees of Freedom**: Headset movement and rotation is tracked in-game
- **Configurable Profiles**: Save and share your settings for each game

## Coming Soon

- **Input Binding**: Map VR controller buttons to game actions
- **Controller Tracking**: Link hand movement to in-game objects
- **Synchronized Rendering**: Renders both eyes at the same time, reducing discomfort

## Preview

![](preview.png)

## Contribute

If you have any ideas for features or want to implement a feature yourself, please email me or send a DM on Discord to
TheNewJavaman#3966.

## Service Structure

### Service Explanations

| Service  | Description                                                                                  |
|----------|----------------------------------------------------------------------------------------------|
| UnrealVR | Handles mod logic at a high level                                                            |
| Pipe     | Communicates with the app UI (launcher)                                                      |
| Logging  | Flushes the buffered log to the pipe                                                         |
| Graphics | Interacts with D3D11 (and D3D12 in the future)                                               |
| XR       | Interacts with OpenXR                                                                        |
| Engine   | Interacts with the application's game engine (Unreal Engine 4/5, maybe others in the future) |

### Service Logic

<!--
https://mermaid.live/
Copy from/paste to ./service_structure.mmd
-->

[![](https://mermaid.ink/img/pako:eNqtWG1P4zgQ_itWVog7qYs4WMSpH1YqhXInsQK1gPZDv7jJNLWa2lnHYcsh_vuNHefdSdtlCyI0fjzzjOfFY795vgjAG3pHR29zTgjjTA2J-ZeQY7WCDRwPyfGCJnA8INXXz1QyuoggwfE3O4JjAZXrbygS3y5plMCgGFlQfx1KkfJAS_y0PNM_x-U4Fwqu1uFYREIaxOXF5Wh03kA8wlaVkKX5NIUIGYAsQefXX04vriog6ishM1QfYB2a0ZvTy9HFpDm6g4fB3DEOOWbufTo1nwoooguIrsS2ZvXkbDz-e-JC7TLLAOu82iqFiHdAkDp7oYoJvs9C5tBeExIWchr16MwATmIZ5l0_8M_70dGcJ_AjBe7DNaOhpBs9FFOpmM9iyhXZUMYJTcgTl0Cj5ymZgXxhPjRxMYtB4x70swMTiVBD7kQYMh52obZSg753akKW8Yr5iUbd5v93YIGjJsPrlm6A3GRfC7CG6yAn4gWkMXVgpwzJXyfkX8xfY5jGWfeAgbUnDsl1FI2Uov6K-DSKINAYPfL561ctY2jE1STloktJGfA-Bl4M6udnlJEp0W8k-IrIcEH_OB0Q-3tyevFn5lw0n4ySV-7n8WAE5BzuWKJQ-FJI4ovNhvIgyXEBtIgRXA1jh31Yc9CPDmvwrf6Ojzrditx86Q60QIu0WidRmqzIIl0uQTqIWw5N3n1-PrN-DjFA9vLODJTC8E30LEYj9h8E2iBgL5nP7XpnC3DPdUI4pjhDyqxv4avG4uV8Dlw867OtLF2mP4XYbbGOpaEaPFOYRzoNE0gSLEpu1BRCHVES97o4VWSFARWBTNzgMZYQ_J78pLG_0nVFQiJS6UOB38p67DTcW3K1VuW1oMO2fLhNp5z4jxDrCqUYOQEvROW4XlpNNZZcHmFOatlgm1g-ydDSMUmwmK1zXDbaS6Yu2JF8hyTHOT0hT3FgjGzmR6mozf4ROZOVNqGshZZ7mRm6JD9a43ZW12-pgm01R3JVjSwpWR2YJ02Fs59MYS0fPzwReAVHNt2CehAJ9CSUM3JdId4ImGzFncJ3hY11lo9LKymZ3D_vC5VCmd4jx4u4iNmeyXFEX_EtZZsyQoN9VUbCr6ncI7jLGO6M-Wpw94b2ogjt79NaCFbT2VUxHrIS0YzvolSUET7Ly8pDWVUOj_RScyPWq0R_T7Tf9kX7RNoS0BHuGDLkDpaqKqG5BVzhfoHtR1VSa5fAFgojC6M_6Q0qywot59hYkySVS-rDx3eSseAovsPafXaUaUZomUZR4kvAdutHSoPfsaOYfTldbNhOXzT2cjOnY6W026YsXPX67YYHda9VXLL3Un9wM_pygk2JiE0SRbY6OQQ6-vJraPTlgIdpImRDRtn2tno-1Ftr8txtnmXnauScrZw1-tc6unxZSq0H7BNT2OB301uYOvbLDUYrfZx09ksbQymuVNd-eAQUveibdNUdarud_HCuuWzpSrKcUE97W5-gj9TO7pl0ZPEqVYH4yR1N-aHVzpylnNYVJyj3ucpxvquILZOlJbfMrXZ2jSMs9jVE-9DbeULdWVa8gYftBo4E3tAzl3Fzz1y6zT19kxTAkqZYTrxBNpTfh-A2sGShxtgLvLm3YVIKOdK3UUk22dzKzb38PgVVpaaluAkYgrxhdmvn0VSJGWawN1QyhRxkb1ws6v1_upF5eA)](https://mermaid.live/edit#pako:eNqtWG1P4zgQ_itWVog7qYs4WMSpH1YqhXInsQK1gPZDv7jJNLWa2lnHYcsh_vuNHefdSdtlCyI0fjzzjOfFY795vgjAG3pHR29zTgjjTA2J-ZeQY7WCDRwPyfGCJnA8INXXz1QyuoggwfE3O4JjAZXrbygS3y5plMCgGFlQfx1KkfJAS_y0PNM_x-U4Fwqu1uFYREIaxOXF5Wh03kA8wlaVkKX5NIUIGYAsQefXX04vriog6ishM1QfYB2a0ZvTy9HFpDm6g4fB3DEOOWbufTo1nwoooguIrsS2ZvXkbDz-e-JC7TLLAOu82iqFiHdAkDp7oYoJvs9C5tBeExIWchr16MwATmIZ5l0_8M_70dGcJ_AjBe7DNaOhpBs9FFOpmM9iyhXZUMYJTcgTl0Cj5ymZgXxhPjRxMYtB4x70swMTiVBD7kQYMh52obZSg753akKW8Yr5iUbd5v93YIGjJsPrlm6A3GRfC7CG6yAn4gWkMXVgpwzJXyfkX8xfY5jGWfeAgbUnDsl1FI2Uov6K-DSKINAYPfL561ctY2jE1STloktJGfA-Bl4M6udnlJEp0W8k-IrIcEH_OB0Q-3tyevFn5lw0n4ySV-7n8WAE5BzuWKJQ-FJI4ovNhvIgyXEBtIgRXA1jh31Yc9CPDmvwrf6Ojzrditx86Q60QIu0WidRmqzIIl0uQTqIWw5N3n1-PrN-DjFA9vLODJTC8E30LEYj9h8E2iBgL5nP7XpnC3DPdUI4pjhDyqxv4avG4uV8Dlw867OtLF2mP4XYbbGOpaEaPFOYRzoNE0gSLEpu1BRCHVES97o4VWSFARWBTNzgMZYQ_J78pLG_0nVFQiJS6UOB38p67DTcW3K1VuW1oMO2fLhNp5z4jxDrCqUYOQEvROW4XlpNNZZcHmFOatlgm1g-ydDSMUmwmK1zXDbaS6Yu2JF8hyTHOT0hT3FgjGzmR6mozf4ROZOVNqGshZZ7mRm6JD9a43ZW12-pgm01R3JVjSwpWR2YJ02Fs59MYS0fPzwReAVHNt2CehAJ9CSUM3JdId4ImGzFncJ3hY11lo9LKymZ3D_vC5VCmd4jx4u4iNmeyXFEX_EtZZsyQoN9VUbCr6ncI7jLGO6M-Wpw94b2ogjt79NaCFbT2VUxHrIS0YzvolSUET7Ly8pDWVUOj_RScyPWq0R_T7Tf9kX7RNoS0BHuGDLkDpaqKqG5BVzhfoHtR1VSa5fAFgojC6M_6Q0qywot59hYkySVS-rDx3eSseAovsPafXaUaUZomUZR4kvAdutHSoPfsaOYfTldbNhOXzT2cjOnY6W026YsXPX67YYHda9VXLL3Un9wM_pygk2JiE0SRbY6OQQ6-vJraPTlgIdpImRDRtn2tno-1Ftr8txtnmXnauScrZw1-tc6unxZSq0H7BNT2OB301uYOvbLDUYrfZx09ksbQymuVNd-eAQUveibdNUdarud_HCuuWzpSrKcUE97W5-gj9TO7pl0ZPEqVYH4yR1N-aHVzpylnNYVJyj3ucpxvquILZOlJbfMrXZ2jSMs9jVE-9DbeULdWVa8gYftBo4E3tAzl3Fzz1y6zT19kxTAkqZYTrxBNpTfh-A2sGShxtgLvLm3YVIKOdK3UUk22dzKzb38PgVVpaaluAkYgrxhdmvn0VSJGWawN1QyhRxkb1ws6v1_upF5eA)

## Improvements

There are so many features I want to add, and so many refactorings that could improve the code.

- **Input Binding**: Map VR controller buttons to game actions via keyboard/mouse/gamepad input. Add a button to reset
  the view rotation/position.
- **Controller Tracking**: Link hand movement to in-game objects. Make them rotate and move according to how your
  hands move. Allow configurable offsets for the UI so that it feels like your hand is holding a gun, for example.
- **Synchronized Rendering**: Renders both eyes at the same time, reducing discomfort. This one isn't as important
  because the mod seems to perform pretty well so far. But at lower framerates, some people may experience discomfort is
  the positions of objects aren't aligned for the left/right eyes.
- **Optimization**: Do more profiling to increase framerate. VR should be accessible to as many people as possible, so
  we as a community need to make sure that our mods aren't too limited by hardware.
- **VR For All**: Abstract the graphics, VR, and game engine services into interfaces that can be implemented for other
  graphics APIs/VR runtimes/game engines; this will definitely be necessary for D3D12, and if I want to add support
  Vulkan/OpenGL, it'll be even more important. Probably won't be used for other game engines, but if someone uses a game
  engine written in any language that can interop with C++, imagine the possibilities! Probably not useful for VR,
  though, since OpenXR is the future... hopefully there won't ever be a replacement VR runtime that I'll have to add
  support for.
- **UE Version Detector**: Reduces the configuration necessary for new games. Would use the same information as the
  Details pane in File Explorer to determine the Unreal Engine version.
- **Console Output**: Enable/disable the log window; also, don't use a basic terminal for output, show it in the
  nice-looking app UI.
- **Unreal Engine 5**: It's not too different from UE4, so add support to UML if Russell.J doesn't have time
- **Build System**: Use meson so it's faster and cross-platform. Would also reduce dependence on Rider/Visual Studio
- **Stop Using UML**: Build a home-grown solution that's better for this specific use-case
- **Dependency Injection**: Inject service class instances at runtime so there aren't as many static members

### Development Tools

- **Rider 2022.1 EAP**: For writing C++ code; it has a much better code formatter than Visual Studio, is much faster,
  and has superior refactoring capabilities. Also supports Python for the rizin script. Get it for free via the early
  access program in the Jetbrains Toolbox app
- **Visual Studio 2022 Preview**: For writing C# code or HLSL shaders; Rider doesn't support WinUI nor Hot Reload; also,
  VS supports profiling both the CPU and GPU for C++, compiling HLSL, and packaging the final app. Get it for free via
  the Visual Studio installer
- **Sourcetree**: My favorite Git GUI; the main thing is its support for multiple stashes; get it for free from
  Atlassian
- **Rizin**: Used to reverse engineer UE/game binaries. Get it for free from GitHub

### Rizin Cheatsheet

My most-used commands for reverse-engineering game binaries

- `[command]?`: Show the help menu
- `o [filename]`: Open a binary (exe, dll, lib)
- `ipd [filename]`: Open a debug symbols file (pdb)
- `Po [filename]`: Open a project (rzdb)
- `Ps [filename]`: Save project to file (rzdb)
- `aa`: Run quick analysis on the binary (better for no PDB)
- `aaa`: Run longer analysis on the binary (better for PDB, includes function autonaming)
- `aflq~[function]`: Search for a function
- `s [address|function]`: Go to an address or function
- `zaf [function] [signature]`: Create a signature for the current function
- `z`: List signatures
- `zos [filename]`: Save signatures to file (sdb)
- `zj | python rizin_to_uml.py [numbytes]`: Print signatures in UML format; see `rizin_to_uml.py`
