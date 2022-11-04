### **RG351 M8 Headless Advanced audio**

Midi devices and/or external sound cards can be used. In most cases latency will be pretty bad and impact usability.
I haven't had very good luck sampling directly into Headless on the RG351 devices, but it might work ok on a Mac or PC.
Personally, for the best results, I create samples on the PC or other devices and copy onto the M8 sdcard.

The card index for ALSA and/or PulseAudio depends on order connected to the rg351 device.
For best results, plug in teensy first, then optionally an external sound card, then optionally a midi device.

**In ArkOS to access ssh:**
Options/Network Info - note the ip address.
Options/Enable remote services
From your PC run:
	`ssh <ip-addr> -l ark`

In the ssh terminal run
	`cd roms2/ports/M8`

**ALSA Commands:**
card# depends on order connected to the rg351
For best results, plug in teensy first, then external sound card then midi

`aplay -l` 
shows playback devices

`arecord -l`
shows capture devices

Example for aplay and arecord
	M8 is connected then external USB Audio device is connected

 	ark@rg351v:/roms2/ports/M8$ aplay -l

	**** List of PLAYBACK Hardware Devices ****
	card 0: rockchiprk817co [rockchip,rk817-codec], device 0: ff070000.i2s-rk817-hifi rk817-hifi-0 []
	  Subdevices: 0/1
	  Subdevice #0: subdevice #0
	card 1: M8 [M8], device 0: USB Audio [USB Audio]
	  Subdevices: 1/1
	  Subdevice #0: subdevice #0
	card 2: Device [USB Sound Device], device 0: USB Audio [USB Audio]
	  Subdevices: 1/1
	  Subdevice #0: subdevice #0

	ark@rg351v:/roms2/ports/M8$ arecord -l

	**** List of CAPTURE Hardware Devices ****
	card 0: rockchiprk817co [rockchip,rk817-codec], device 0: ff070000.i2s-rk817-hifi rk817-hifi-0 []
	  Subdevices: 1/1
	  Subdevice #0: subdevice #0
	card 1: M8 [M8], device 0: USB Audio [USB Audio]
	  Subdevices: 1/1
	  Subdevice #0: subdevice #0
	card 2: Device [USB Sound Device], device 0: USB Audio [USB Audio]
	  Subdevices: 1/1
	  Subdevice #0: subdevice #0

Connect M8 to rockchip out 
	`alsaloop -P hw:0,0 -C hw:1,0 -t 200000 -A 5 --rate 44100 --sync=0 -T -1 -d`

You can edit the alsaloop_wait file with the correct alsaloop command

You'll need to use some pulseaudio commands to change the active port on a USB soundcard device (i.e from mic to line in)

### **PulseAudio**

Determine active audio ports on USB audio
If you get errors, you may need to restart pulse before each command.

**Input ports**

First stop emulationstation
`ark@rg351v:/roms2/ports/M8$ sudo systemctl stop emulationstation`

```
ark@rg351v:/roms2/ports/M8$ sudo pulseaudio --start
W: [pulseaudio] main.c: This program is not intended to be run as root (unless --system is specified).


ark@rg351v:/roms2/ports/M8$ sudo pacmd
>>> list-sources
4 source(s) available.
...
      ports:
                analog-input-mic: Microphone (priority 8700, latency offset 0 usec, available: unknown)
                        properties:
                                device.icon_name = "audio-input-microphone"
                analog-input-linein: Line In (priority 8100, latency offset 0 usec, available: unknown)
                        properties:

        active port: <analog-input-linein>
...
```		
to change active input port:
`>>> set-source-port <index#> analog-input-linein`
or
`>>> set-source-port <index#> analog-input-mic`

**Output Ports**
`>>>list-sinks`
Note the sinks

Exit pacmd
`>>> exit`

Restart emulationstation
`ark@rg351v:/roms2/ports/M8$ sudo systemctl start emulationstation`

Edit the pulseaudio_pa file - set the sink and source corresponding to the correct device index from the listings in pacmd

