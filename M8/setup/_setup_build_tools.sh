m8_path=ports/M8
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi

# wget https://github.com/christianhaitian/arkos/raw/main/Headers/install_headers.sh
# chmod 777 install_headers.sh
# ./install_headers.sh
#
# rm ./install_headers.sh

sudo apt-get update --assume-yes
sudo apt-get --reinstall install --assume-yes libserialport-dev libsdl2-dev build-essential libc6-dev linux-libc-dev
