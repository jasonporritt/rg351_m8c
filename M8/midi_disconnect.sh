#!/bin/bash

aconnect -x

# show connections on main screen
sudo chmod 666 /dev/tty1;
echo -e "\e[32mMidi Connections:" > /dev/tty1;
/bin/aconnect -ol > /dev/tty1;
sleep 5;
printf "\033c" >> /dev/tty1;
