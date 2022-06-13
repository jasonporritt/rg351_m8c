m8_path=ports/M8
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi

sudo pulseaudio --start --file=_other_files/pulseaudio_config.pa

./_m8c/m8c

sudo pkill -f '_other_files/pulseaudio_config.pa'
