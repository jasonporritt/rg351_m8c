#!/bin/bash

# Uncomment the following lines to disable wi-fi
#sudo modprobe -r mt7601u
#sudo sed -i '$ablacklist mt7601u' /etc/modprobe.d/blacklist.conf
#printf "\n\n\n\e[32mWifi has been disabled.\n"

m8_path=ports/M8
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi

# set cpu governor to performance to minimize audio "crackles"
echo "performance" | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# alsaloop_wait will run in background and try to create the loopback 
# if it can't be created before m8c runs. Useful for wait_for_device=true. 
./alsaloop_wait &
#alsaloop -P hw:0,0 -C hw:1,0 -t 200000 -A 5 --rate 44100 --sync=0 -T -1 -d
./_m8c/m8c

pkill alsaloop

# Uncomment the following lines to enable wi-fi
#sudo modprobe -i mt7601u
#sudo sed -i '/blacklist mt7601u/d' /etc/modprobe.d/blacklist.conf
#printf "\n\n\n\e[32mWifi has been enabled.\n"
