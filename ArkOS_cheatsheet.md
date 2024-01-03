### **Installing ArkOS** (cheat sheet for Windows)
full instructions at https://github.com/christianhaitian/arkos/wiki#instructions-for-loading

**ArkOS downloads**
https://github.com/christianhaitian/arkos/wiki#download-links

**Imager** (win)
https://www.alexpage.de/usb-image-tool/

Back up both original cards from the RG351v if you want to play games

Uncompress the ArkOS image to hard drive

Use a new SD card (16G) - OR -
wipe card 1 (16G) 
You can use the Administrative tools/computer management/Disk Management in Windows to delete the partitions on the SDHC card 

**Install ArkOS image**
Use the imager to restore the arkos image file - this takes a few minutes.
eject and put the card in the first (top) slot of the RG351v
Start the RG351 and follow the on-screen instructions. This takes several minutes. If you see only a white screen with a black bar on the left side, see [this solution](https://github.com/christianhaitian/arkos/issues/662#issuecomment-1510064072).

Options/Wifi - Setup wifi (shoulder buttons advance cursor). You have to be connected to the internet to run the setup scripts.
Options/Network_info - Verify Network_info
Options/Update - follow on-screen instructions - this takes several minutes

If you are using the second card do "Options/Advanced/Switch to SD2" for ROMS
You can select visible systems under <Start button menu> UI Settings.

**To enable USB as a drive** - Options/ USB Drive Mount

**To FTP or SSH** 
Options/Network Info - note network info IP address - mine was 192.168.86.42
Options/Enable Remote Services
Then you can ssh or sftp to the device using login ark, pw ark

Put the M8 folder in roms2/ports/M8 (or roms/ports/M8 if not using the second card)
attach the Teensy that has already been set up with the latest Headless firmware 

Run M8 Setup and/or Setup_Pulse
**Restart system**

