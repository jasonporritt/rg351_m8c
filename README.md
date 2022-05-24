# m8 Headless for RG351 devices

This setup should work for a variety of rg351 devices, including the rg351m, rg351mp, and rg351v. It requires a custom firmware install. Instructions will focus on ArkOS but it may also work with 351Elec or TheRA.

1. Install ArkOS following their directions. https://github.com/christianhaitian/arkos/wiki
2. Copy the M8 folder into /roms/ports/ directory (or /roms2/ports if you're using the second SD card on an rg351mp)
3. Put the SD card in your device and boot it up
4. Under Ports you should now see an m8 item with a few scripts
  - SETUP - will install pulseaudio and add your user to the proper group to interact with serial devices.
  - M8 - launches m8c
5. Run the SETUP task and then reboot the device to ensure the user group changes take effect
6. Now try running M8. If you're lucky it'll all work!

## Problems

### No audio

The audio device names appear to be different for each Teensy device and for different rg351 devices. You'll need to find the device names using the commands below.

If you don't hear any audio you may have different names for the audio devices. You can use these commands to get the device names AFTER you've started pulseaudio using
  - start pulseaudio: `sudo pulseaudio --start`
  - get source names: `pacmd list-sources | grep -e 'name:' -e 'index:' -e device.string`
  - get sink names: `pacmd list-sinks | grep -e 'name:' -e 'index:'`

Update the pulseaudio config file at _other_files/pulseaudioconfig.pa with the device names you find.
