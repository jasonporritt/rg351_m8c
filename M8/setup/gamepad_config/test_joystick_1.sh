#!/bin/bash
CURRENT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $CURRENT_PATH

sudo chmod 666 /dev/tty1; ./gamepad_config -j 1 > /dev/tty1; printf "\033c" >> /dev/tty1;
