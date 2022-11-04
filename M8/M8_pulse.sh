#!/bin/bash

# Uncomment the following lines to disable wi-fi
#sudo modprobe -r mt7601u
#sudo sed -i '$ablacklist mt7601u' /etc/modprobe.d/blacklist.conf
#printf "\n\n\n\e[32mWifi has been disabled.\n"

sed -i "/^idle_ms=/s/=.*/=25/" ~/.local/share/m8c/config.ini

m8_path=ports/M8
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi

# set cpu governor to powersave to minimize audio "crackles"
echo "powersave" | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# pulse_wait will run in background and try to create the loopback 
# if it can't be created before m8c runs. Useful for wait_for_device=true. 
./pulse_wait &
#sudo pulseaudio --start --file=pulseaudio_config.pa
./_m8c/m8c

sudo pkill -f pulse_wait
sudo pkill -f 'pulseaudio_config.pa'

# Uncomment the following lines to enable wi-fi
#sudo modprobe -i mt7601u
#sudo sed -i '/blacklist mt7601u/d' /etc/modprobe.d/blacklist.conf
#printf "\n\n\n\e[32mWifi has been enabled.\n"
