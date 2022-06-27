# auto_midi_connect

Automatically connect midi usb devices using ALSA (aconnect)

https://github.com/smootalicious/auto_midi_connect  
This is a fork of  https://github.com/stevelittlefish/auto_midi_connect modified to use with desktop systems, consoles, and handheld devices running firmware utilizing Emulation Station such as 351ELEC, ArkOS, TheRA.

Tested with RG351v.

Scripts have been modified to place in a "ports" directory and run directly from the device.

Scripts could aslo could also work with a RetroPie installation.

For raspberry pi users please consider the original linked above.

- midi_connect.sh - automatically map every MIDI device onto every other MIDI device.
- midi_disconnect.sh - disconnects all mappings on all midi devices.
