m8_path=ports/M8
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi

alsaloop -P hw:0,0 -C hw:1,0 -t 50000 -B 5000 --rate 44100 --sync=0 -d
./_m8c/m8c

pkill alsaloop
