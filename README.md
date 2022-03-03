# UnrealVR

Adds virtual reality support to flatscreen Unreal Engine games

Sneak peek below 👀

![](preview.png)

## Rewrite Todolist

- [x] Switch to OpenXR
  - [x] Use OpenXR
  - [x] Hook swapchain creation
  - [x] Set up UML for loader DLL
  - [x] Projection view matrix, switch eye locations
  - [x] Revert namespace refactorings, use classes instead
  - [x] Hook present
  - [x] Hook VSSetConstantBuffers
  - [x] Get rid of UML mod, only use loader DLL
  - [ ] Hook projection view matrix updates
    - Get the buffer handle via CreateBuffer
      - Find buffer where description has width 64 bytes
      - Store handle
    - Hook the buffer updates via UpdateSubresource
      - If buffer handle is the same, modify the updated data
      - UE4.25 does not use v1, only default
  - [ ] Finish initializing OpenXR
  - [ ] Submit frames to OpenXR
- [ ] Head tracking
  - [ ] Get input from OpenXR
  - [ ] Projection view matrix transformations on present
- [ ] GUI for editing options/launching games
  - WinUI3? Looks much nicer than Win32Forms or UWP
- [ ] Allow remapping actions to VR controllers
  - [ ] Get input from OpenXR
  - [ ] Forward button presses/movement to game
    - If player is looking to the side, rotate joystick input so that a forward press goes in head direction
- [ ] Allow scaling world vs. virtual space
  - [ ] Scale projection view matrix by custom scalar
  - [ ] Sync with present
- [ ] Synchronized eye rendering
  - [ ] Hook D3D11 get device context, return a deferred context that is recording
  - [ ] Play back the recording on UnrealVR's immediate context (x2, once for each eye)
  - [ ] Reset deferred context recording
- [ ] Customizable motion controls in certain games
  - [ ] Katana slash in Ghostrunner
    - No clue how this will work