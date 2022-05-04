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

<!-- Click on the link to edit the diagram, then update it here -->

[![](https://mermaid.ink/img/pako:eNqtWG1P4zgQ_itWVog7iUUcHOLUDytBebmTWIFaQPuhX9xkmlpN7aztsOUQ__3GjvPupO3uFURo_Hj8POOZ8STvQSgiCEbBwcH7jBPCONMjYv8l5FAvYQ2HI3I4pwoOj0j99guVjM4TUDj-7kZwLKJy9RVN4t0FTRQclSNzGq5iKTIeGYufFqfm57Aa50LD1Soei0RIi7g4v7i8PGshnmCjK8jCftpGhIxAVqCz6z9Pzq9qIBpqIXPUEGAV29Gbk4vL89v26BYeFnPPOBSYWfDpxH5qoITOIbkSm4bq29Px-K9bH2qbLAts8uouKUS6BYLU2SvVTPBdHFlAByUoFnOaDKyZA7zEcsyHueCfj4ODGVfwPQMewjWjsaRrM5RSqVnIUso1WVPGCVXkmUugycuETEG-shDauJSlYHCP5tqDSURsIPcijhmP-1AbaUDfeldClumShcqg7or_e7DAcSXL646ugdzkX0uwgZsgJ-IVpJV65KaMyB_0mPyDCWyVGaDbH7C47swRuU6SS61puCQhTRKIDMaMfP7yxdgYWXMNS4XpylIOfEiBl4Pm-hlt5IuYOxJCTWQ8p7-dHBH3e3xy_nu-u6ifXKo3HhYBYQ0UHO6Z0mh8ISQJxXpNeaQKXAQdYgTdYXW4i5ODG-lRg3fNd7w06dbsFq7bU4Ex6Va9TTK1JPNssQDpIe44tHkPbvTcbXSMIbLT9kxBawxgZWYxmrB_ITKKgL3mm-4cnnvggZuU8EzxxpR1cLlZLe8VfPb0ntu0jaz2zHxKs5vSkZVQA55qzCSTiAqUwrLkR00gNiEl8bRLM02WGFEJSOUHj7GI4Hf1g6bh0lQWCUpkMoQSv5HN4Gntb8XVqSqqQY-2YrhLp5roSIWC430jtMvKN-1vIVY1JSlOAl4yKHCDatrsnKYiMr2K8sEusWKSpWVCmWAVXBW4fHSQTNOwJ2n3SapTrJ7PaWRFttOqWqjL_gk5k6WRUNVQx71KKFPLn5y4rVX5a6ZhU0-tYqlWclWs9kyv9oLTH0zjGTB-fCbwBj1bm_vmUSjYssGleJM-d6C9UzZyMFXynKrr9oeO27AQ3SspuX142RUqhbaNS4EXaRm3A5PThL7hXcrWldho1yUTETaW3CHAqzjujft6gA-G97wM72-TRhjWU9pXNR7zMtGO8bJcVFE-LUrLY1VZ9o_2auVWvNeJ_j8Rf-eNeHuSZPM107fSVYKe2MWoIfew0HUj7QPkCk8b7F7qljpnDHZgGFyYJ2owrlxCoXiOjTlRmVzQEGo5U3fe2B4OPRJ2OWQm-SqLLElUKAFbsO8ZjXynheG1w1nROquth3u0GMdOWLwc9OwNj5p-rTlt50P5F0-Ns2NsOkRqIz1xJcRj0NN4X0Or8QZ8XCZCtmxUfW2np8N1G02cv41z7HyNmrdVc6J_rmMr3FKtukcfMIE1frdNgC02P90JdBotL53dcsBSSmslcBieAFXDjdmvtFl5j-vR0tcQF4QG2tfmBPPQ7O2OSU8WLzMdiR_c03Tv2xfbhyWvuvIRyf_g5HmAq5mtkqVjt8qtbnaNEyzHDUT3qbb3EXRrWQmOAuwJcCQKRoF93TYL7Gu1WWDeFUWwoBmWk2DGPxCa2XP7JmJayGCUv1cLaKbFFDMwGGmZQQFy70Qc6uM_FCxfEQ)](https://mermaid.live/edit#pako:eNqtWG1P4zgQ_itWVog7iUUcHOLUDytBebmTWIFaQPuhX9xkmlpN7aztsOUQ__3GjvPupO3uFURo_Hj8POOZ8STvQSgiCEbBwcH7jBPCONMjYv8l5FAvYQ2HI3I4pwoOj0j99guVjM4TUDj-7kZwLKJy9RVN4t0FTRQclSNzGq5iKTIeGYufFqfm57Aa50LD1Soei0RIi7g4v7i8PGshnmCjK8jCftpGhIxAVqCz6z9Pzq9qIBpqIXPUEGAV29Gbk4vL89v26BYeFnPPOBSYWfDpxH5qoITOIbkSm4bq29Px-K9bH2qbLAts8uouKUS6BYLU2SvVTPBdHFlAByUoFnOaDKyZA7zEcsyHueCfj4ODGVfwPQMewjWjsaRrM5RSqVnIUso1WVPGCVXkmUugycuETEG-shDauJSlYHCP5tqDSURsIPcijhmP-1AbaUDfeldClumShcqg7or_e7DAcSXL646ugdzkX0uwgZsgJ-IVpJV65KaMyB_0mPyDCWyVGaDbH7C47swRuU6SS61puCQhTRKIDMaMfP7yxdgYWXMNS4XpylIOfEiBl4Pm-hlt5IuYOxJCTWQ8p7-dHBH3e3xy_nu-u6ifXKo3HhYBYQ0UHO6Z0mh8ISQJxXpNeaQKXAQdYgTdYXW4i5ODG-lRg3fNd7w06dbsFq7bU4Ex6Va9TTK1JPNssQDpIe44tHkPbvTcbXSMIbLT9kxBawxgZWYxmrB_ITKKgL3mm-4cnnvggZuU8EzxxpR1cLlZLe8VfPb0ntu0jaz2zHxKs5vSkZVQA55qzCSTiAqUwrLkR00gNiEl8bRLM02WGFEJSOUHj7GI4Hf1g6bh0lQWCUpkMoQSv5HN4Gntb8XVqSqqQY-2YrhLp5roSIWC430jtMvKN-1vIVY1JSlOAl4yKHCDatrsnKYiMr2K8sEusWKSpWVCmWAVXBW4fHSQTNOwJ2n3SapTrJ7PaWRFttOqWqjL_gk5k6WRUNVQx71KKFPLn5y4rVX5a6ZhU0-tYqlWclWs9kyv9oLTH0zjGTB-fCbwBj1bm_vmUSjYssGleJM-d6C9UzZyMFXynKrr9oeO27AQ3SspuX142RUqhbaNS4EXaRm3A5PThL7hXcrWldho1yUTETaW3CHAqzjujft6gA-G97wM72-TRhjWU9pXNR7zMtGO8bJcVFE-LUrLY1VZ9o_2auVWvNeJ_j8Rf-eNeHuSZPM107fSVYKe2MWoIfew0HUj7QPkCk8b7F7qljpnDHZgGFyYJ2owrlxCoXiOjTlRmVzQEGo5U3fe2B4OPRJ2OWQm-SqLLElUKAFbsO8ZjXynheG1w1nROquth3u0GMdOWLwc9OwNj5p-rTlt50P5F0-Ns2NsOkRqIz1xJcRj0NN4X0Or8QZ8XCZCtmxUfW2np8N1G02cv41z7HyNmrdVc6J_rmMr3FKtukcfMIE1frdNgC02P90JdBotL53dcsBSSmslcBieAFXDjdmvtFl5j-vR0tcQF4QG2tfmBPPQ7O2OSU8WLzMdiR_c03Tv2xfbhyWvuvIRyf_g5HmAq5mtkqVjt8qtbnaNEyzHDUT3qbb3EXRrWQmOAuwJcCQKRoF93TYL7Gu1WWDeFUWwoBmWk2DGPxCa2XP7JmJayGCUv1cLaKbFFDMwGGmZQQFy70Qc6uM_FCxfEQ)

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
