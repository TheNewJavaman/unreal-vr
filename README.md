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

[![](https://mermaid.ink/img/pako:eNrNWG1P40YQ_isrnxCtxCHKFVHlw0kQCO2JE2kC6D7ky8aeOG42Xt96DaGI_97ZF7-vnaScquZO-GWfnZ2ZnWdm1q-ezwPwBt7BwessJiSKIzkg-paQQ7mENRwOyOGcpnB4RKqvH6mI6JxBiuOvdgTHAipWX1Ekvl1QlsJRMTKn_ioUPIsDJfHD4lT9OyzHYy7hchUOOeNCI87Pzi8uPjUQ97CRJWShf00hXAQgStCnq19Pzi4rIOpLLgyqD7AK9ej1yfnF2ag5ukUPjbmNYsgxM-_Dif5VQIzOgV3yTc3q0elw-NvIhdpmlgbW9WovyXmyBYKqR09URjzexZE5tNeENApjynrWNACnYgbzpi745-3gYBan8D2D2IeriIaCrtVQQoWM_CihsSRrGsWEpuQhFkDZ44RMQTxFPjRxconjgULe67sx56wDmkQJKOBYXTswjIcKcsvDMIrDLtRGKNC3TqXQoGQZ-alC3eT3HViIcSWt1w1dA7k2jwVYwRUfCH8Cob1yZKcMyC_0mPyBXCdDLrRcu5Wgce2ZA3LF2IWU1F8SnzIGgcKokY-fPxtHDrTAmiwzUJeWg6cSLcn3IEHXK5h5_IgyzaLqnQBfEhHO6U8nR8T-Pz45-9kEBvrDbl8eTFZGqdZtlEqIyYIL8hefpzkuAIeeBD2kb-zFWqj232Gfel23zgDvElwvH1TXijZ_ZpDBFz7vcJRTq9ItRn4hdk9H2WX1o9HKyKt4yOfrNY0Dl5dygzp8hPHvcBG-Vc94-WE-0AvlQmseqEzMw3gvpxgttfwRy9IlmWeLBQiHL6xZTVf0Um5uKafIulMYTUFKTCWpmhVRFv0NgbIHoidDP7uHxv67WCUnxxQnuxvMfd-GlBugxdbC1BE_e22JVXQjythSv0Loptid0nsKbNIL5tkU0hQLlBs1gVCFvsC-J8kkWWLkMxCpGzxEe_E5faaJv1Q1RkDKM-FDgd-IujsaQVPqaq3Kk32HbflwW51yolXK5zG-V4a2tXJN-53zVcWSBCdBXGiQ43qtaWpnbcrD3WmRGWwrlk_SaoWqmGGRW-U4M9qrTF2wIw_sw9RTLI4PSaCmt7haLtTW_h51JktlQlkire4lS5XEe2vc1qL7NZOw-Q_4mlvQmFwauxdnm1ZMnyOJfcNw_EDgBTrixTh8zFPYEjWFRxUnb0A6p2xEL_8MUatWu-PRRoGPeyYoGd097goVXOq-OMfzpCBDz-SE0Rd8S6N1aWyw65KM-7Uld2BNSY5OMlVZ08uZecGZb5NaWFbzhCsVjU3uaRKnyEEldaZ5vhqX6ep_Q6HSoMb0qv0_gkY3ThrpmpfN15EcCZuzOgiBoUhuYSGrQpql7hLrIvaDVUmtaohHAYxYJF_aG6yWpWh6jIdJkmZiQX2oELHquqEuYx0m7FIOJ2aVRcZY6gvApvZ7Vh4NqnVN6bVDVWt0FdrDHbYox06icNnr2es4qPu14rSd24d31rdPx9ge8URHPrN5ySHQcQK8gsYJEFiKGNGQUZ4UWi0trlvrYd1drNXO1VA6W0pr9L5HxUYJKpfdo2WZwBqfdb-iU9i_blpaPaFTnd1IoFVKKom1H86Apv095Hs6QtOOO2zp6t1zhXo67foE9aXH2ciTDhovMxnw59hxPti3hdeHRad1xRHRfXB0HF8rYku2tOSW5GrTa8gwH9cQ7c8Enaf6Vol0Li9rDGp_1_nC1XmCc2bfpI3vMn261GVvyXLekYd9D44E3sDTX6xnnv4yPfPU59YAFjTD7ObN4jeEZro3uQ4iyYU3MJ-mPZpJPn2JfW8gRQY5yH5WtKi3fwBNqHgp)](https://mermaid.live/edit#pako:eNrNWG1P40YQ_isrnxCtxCHKFVHlw0kQCO2JE2kC6D7ky8aeOG42Xt96DaGI_97ZF7-vnaScquZO-GWfnZ2ZnWdm1q-ezwPwBt7BwessJiSKIzkg-paQQ7mENRwOyOGcpnB4RKqvH6mI6JxBiuOvdgTHAipWX1Ekvl1QlsJRMTKn_ioUPIsDJfHD4lT9OyzHYy7hchUOOeNCI87Pzi8uPjUQ97CRJWShf00hXAQgStCnq19Pzi4rIOpLLgyqD7AK9ej1yfnF2ag5ukUPjbmNYsgxM-_Dif5VQIzOgV3yTc3q0elw-NvIhdpmlgbW9WovyXmyBYKqR09URjzexZE5tNeENApjynrWNACnYgbzpi745-3gYBan8D2D2IeriIaCrtVQQoWM_CihsSRrGsWEpuQhFkDZ44RMQTxFPjRxconjgULe67sx56wDmkQJKOBYXTswjIcKcsvDMIrDLtRGKNC3TqXQoGQZ-alC3eT3HViIcSWt1w1dA7k2jwVYwRUfCH8Cob1yZKcMyC_0mPyBXCdDLrRcu5Wgce2ZA3LF2IWU1F8SnzIGgcKokY-fPxtHDrTAmiwzUJeWg6cSLcn3IEHXK5h5_IgyzaLqnQBfEhHO6U8nR8T-Pz45-9kEBvrDbl8eTFZGqdZtlEqIyYIL8hefpzkuAIeeBD2kb-zFWqj232Gfel23zgDvElwvH1TXijZ_ZpDBFz7vcJRTq9ItRn4hdk9H2WX1o9HKyKt4yOfrNY0Dl5dygzp8hPHvcBG-Vc94-WE-0AvlQmseqEzMw3gvpxgttfwRy9IlmWeLBQiHL6xZTVf0Um5uKafIulMYTUFKTCWpmhVRFv0NgbIHoidDP7uHxv67WCUnxxQnuxvMfd-GlBugxdbC1BE_e22JVXQjythSv0Loptid0nsKbNIL5tkU0hQLlBs1gVCFvsC-J8kkWWLkMxCpGzxEe_E5faaJv1Q1RkDKM-FDgd-IujsaQVPqaq3Kk32HbflwW51yolXK5zG-V4a2tXJN-53zVcWSBCdBXGiQ43qtaWpnbcrD3WmRGWwrlk_SaoWqmGGRW-U4M9qrTF2wIw_sw9RTLI4PSaCmt7haLtTW_h51JktlQlkire4lS5XEe2vc1qL7NZOw-Q_4mlvQmFwauxdnm1ZMnyOJfcNw_EDgBTrixTh8zFPYEjWFRxUnb0A6p2xEL_8MUatWu-PRRoGPeyYoGd097goVXOq-OMfzpCBDz-SE0Rd8S6N1aWyw65KM-7Uld2BNSY5OMlVZ08uZecGZb5NaWFbzhCsVjU3uaRKnyEEldaZ5vhqX6ep_Q6HSoMb0qv0_gkY3ThrpmpfN15EcCZuzOgiBoUhuYSGrQpql7hLrIvaDVUmtaohHAYxYJF_aG6yWpWh6jIdJkmZiQX2oELHquqEuYx0m7FIOJ2aVRcZY6gvApvZ7Vh4NqnVN6bVDVWt0FdrDHbYox06icNnr2es4qPu14rSd24d31rdPx9ge8URHPrN5ySHQcQK8gsYJEFiKGNGQUZ4UWi0trlvrYd1drNXO1VA6W0pr9L5HxUYJKpfdo2WZwBqfdb-iU9i_blpaPaFTnd1IoFVKKom1H86Apv095Hs6QtOOO2zp6t1zhXo67foE9aXH2ciTDhovMxnw59hxPti3hdeHRad1xRHRfXB0HF8rYku2tOSW5GrTa8gwH9cQ7c8Enaf6Vol0Li9rDGp_1_nC1XmCc2bfpI3vMn261GVvyXLekYd9D44E3sDTX6xnnv4yPfPU59YAFjTD7ObN4jeEZro3uQ4iyYU3MJ-mPZpJPn2JfW8gRQY5yH5WtKi3fwBNqHgp)

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
