#!/bin/bash
sudo chmod 666 /dev/tty1;
echo -e "\e[032m" > /dev/tty1;
echo "Running update ..." | tee /dev/tty1;
sudo apt-get update --assume-yes | tee /dev/tty1;
sudo apt-get --assume-yes install libserialport0 pulseaudio | tee /dev/tty1;
sudo adduser $USER dialout  | tee /dev/tty1;
echo "Finished." | tee /dev/tty1;
sleep 2;
printf "\033c" >> /dev/tty1;
