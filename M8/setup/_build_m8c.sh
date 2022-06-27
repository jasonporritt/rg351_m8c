#!/bin/bash
m8_path=ports/M8/_m8c
roms2_path=/roms2/$m8_path
roms_path=/roms/$m8_path

if [ -e "$roms2_path" ]; then
  cd $roms2_path
else
  cd $roms_path
fi


sudo chmod 666 /dev/tty1;
echo -e "\e[032m" > /dev/tty1;
echo "Running update ..." | tee /dev/tty1;
make | tee /dev/tty1;
echo "Finished." | tee /dev/tty1;
sleep 2;
printf "\033c" >> /dev/tty1;
