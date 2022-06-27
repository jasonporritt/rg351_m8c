#!/bin/bash
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


sudo chmod 666 /dev/tty1;
echo -e "\e[032m" > /dev/tty1;
echo "Running update ..." | tee /dev/tty1;
sudo apt-get update --assume-yes | tee /dev/tty1;
sudo apt-get --reinstall install --assume-yes libserialport-dev libsdl2-dev build-essential libc6-dev linux-libc-dev | tee /dev/tty1;
echo "Finished." | tee /dev/tty1;
sleep 2;
printf "\033c" >> /dev/tty1;
