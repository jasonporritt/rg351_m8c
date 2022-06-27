#!/bin/bash
# modified from original at https://github.com/stevelittlefish/auto_midi_connect
# see also midi-connect-LICENSE

SCRIPT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $SCRIPT_PATH
python3 midi_connect.py

# show connections on main screen
sudo chmod 666 /dev/tty1;
echo -e "\e[32mMidi Connections:" > /dev/tty1;
/bin/aconnect -ol > /dev/tty1;
sleep 5;
printf "\033c" >> /dev/tty1;


