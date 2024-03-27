# glassd
perform emulated keyboard shortcuts and common desktop interactions from the touchpad
### status: proof-of-concept in development.
Currently only a stub is available. For testing purposes, it demonstrates uinput usage by creating a virtual keyboard and pressing F on it once every second.


### expected parts of the system:
- **input emulator:** simulate keyboard key-presses (enabled by libevdev-uinput)
- **active application scanner:** figure out what application is focused by window manager and notifies the input emulator to gesture-to-keyboard mappings for different applications (dbus interfaces and IPC sockets from wayland compositors usually allow this)
- **touchpad gesture detector:** interpret various multitouch gestures on a touch-sensitive surface for mapping by the input emulator (will use omniglass backend)
