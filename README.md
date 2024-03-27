# glassd
perform emulated keyboard shortcuts and common desktop interactions from the touchpad
### status: proof-of-concept in development.
Currently only the top-edge of the touchpad is mapped to menu navigation. **run build.sh, execute the resulting file and slide on the top edge of the touchpad for menu navigation (tab/shift+tab)**

### requirements
- standard linux environment (as in: some libc implementation and linux input headers)
- omniGlass libraries (touchpad gesture detection)
- libevdev, with the uinput headers (linux input device emulation)


### expected parts of the system:
- **input emulator:** simulate keyboard key-presses (enabled by libevdev-uinput)
- **active application scanner:** figure out what application is focused by window manager and notifies the input emulator to gesture-to-keyboard mappings for different applications (dbus interfaces and IPC sockets from wayland compositors usually allow this)
- **touchpad gesture detector:** interpret various multitouch gestures on a touch-sensitive surface for mapping by the input emulator (will use omniglass backend)
