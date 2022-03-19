# UnrealVR

Adds virtual reality support to flatscreen Unreal Engine games

Sneak peek below 👀

![](preview.png)

## Contribute

If you have any ideas for features, or want to implement a feature yourself, please email me or send a DM on Discord to
TheNewJavaman#3966.

I currently use Rider 2022.1 EAP to develop this project (with Visual Studio 2022 build tools). You can get a
free license of Rider by downloading the EAP version via JetBrains Toolbox. Versions <2022.1 don't have C++/Unreal
Engine support merged into mainline.

Note: VS 2022 is still needed for some tasks:

- Compiling HLSL shaders; there are a few custom settings, contact me for more info
- Profiling C++ code

## Rewrite Todolist

- [x] Switch to OpenXR
- [x] Optimize graphics pipeline (WIP; can always do more)
- [ ] Fix resolution
- [ ] Head tracking
- [ ] GUI for editing options/launching games
- [ ] Allow remapping actions to VR controllers
- [ ] Allow scaling world vs. virtual space
- [ ] Synchronized eye rendering
- [ ] Customizable motion controls in certain games
