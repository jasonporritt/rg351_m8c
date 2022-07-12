# m8 Headless for RG351 devices

This setup should work for a variety of rg351 devices, including the rg351m, rg351mp, and rg351v. It requires a custom firmware install. Instructions will focus on ArkOS but it may also work with 351Elec or TheRA with a few tweaks.

Source files are included in the releases, so it may be possible to compile on desktop systems, consoles, and other devices.

## Download a release here

https://github.com/jasonporritt/rg351_m8c/releases

Download one of the rg351_m8c releases from the link above if you don't want to compile m8c yourself.

## Setup

1. Install ArkOS following their directions. https://github.com/christianhaitian/arkos/wiki
2. Copy the M8 folder into /roms/ports/ directory (or /roms2/ports if you're using the second SD card on an rg351mp)
3. Put the SD card in your device and boot it up
4. Under Ports you should now see an m8 item with a few scripts
  - SETUP folder
    - GAMEPAD-CONFIG -  see Features below 
    - SETUP - will install libserialport and add your user to the proper group to interact with serial devices.
    - SETUP_PULSE - installs pulseaudio if you prefer that audio subsystem
    - _setup_build_tools.sh - install headers and build tools for compiling m8c
    - _build_m8c.sh - build the binary
  - M8 - launches m8c with cpu locked to powersave (uses alsaloop for audio) 
  - M8_PERFORMANCE - launches m8c with cpu locked to "performance" (uses alsaloop for audio) 
  - M8_PULSE - run m8c using pulseaudio
  - MIDI_CONNECT - automatically map every MIDI device onto every other MIDI device. 
  - MIDI_DISCONNECT - disconnects all mappings on all midi devices. 
5. Connect your device to the internet (Wifi dongle or built-in wifi). It's required for SETUP so that packages can be installed, but not for running m8c itself.
6. Run the SETUP or SETUP_PULSE task and then reboot the device to ensure the user group changes take effect
7. Now try running M8 or M8_PULSE. If you're lucky it'll all work!

## Building m8c on the device

You can run the provided scripts to rebuild m8c on the device.

- `_setup_build_tools.sh` - MUST BE ONLINE to install build tools
- `_build_m8c.sh` 

I recommend running them in a terminal session so you can see the output, but you're welcome to try your luck from the menu.

## Features
### Emulation Station friendly shell scripts
Includes scripts to run it directly from EmulationStation. 
Currently tested in ArkOS, but should work with 351ELEC or TheRA and 
possibly other rpi-based distributions, desktop systems, consoles, and other devices.

### setup/gamepad_config
`gamepad_config` is a console based program that lets you see what joysticks 
and game controllers are detected by SDL and configure SDL mappings. 
In most cases, once running, the program can be controlled exclusively by the
connected joystick/gamepad.

Includes a config helper for m8c that will translate m8c
button inputs to the proper values and output a snippet that can be dropped 
in to the m8c config.ini file found in `/home/<user>/.local/share/m8c/`

### midi_connect
Automatically connect midi usb devices using ALSA (aconnect)
- midi_connect.sh - automatically map every MIDI device onto every other MIDI device.
- midi_disconnect.sh - disconnects all mappings on all midi devices.

## Problems

### No audio

The pulseaudio setup uses device numbers to avoid needing to look up device names. It has worked consistently for me but you may need to use different device numbers or names depending on your setup. You can see device names and numbers using these commands:

- sudo pulseaudio --start &
- sudo pacmd list-sources | grep -e 'name:' -e 'index:' -e device.string
- sudo pacmd list-sinks | grep -e 'name:' -e 'index:'

Also check alsamixer settings. You may have a device muted or set to low volume.

### Audio glitches

Update to latest OS. In ArkOS you can do Options > Update while connected to the internet.

If wifi is enabled you can sometimes hear audible wifi noise on the bus. Try disabling wifi before running m8c.

Try running M8_PERFORMANCE to lock the cpu to performance mode.

If the alsaloop setup (default) has audio glitches or crashes, you can use the pulseaudio variant of the commands.

- Connect your device to the internet
- Run the setup_pulse script
- Then you can run the M8_pulse script to run M8 using the pulseaudio subsystem
