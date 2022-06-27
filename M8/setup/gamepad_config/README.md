# gamepad_config
Console based gamepad configurator for SDL mappings - with m8c config helper


###### Based somewhat loosely on `jstest-sdl` https://github.com/meleu/jstest-sdl (which is in turn based on `sdl-jstest` http://github.com/Grumbel/sdl-jstest)


`gamepad_config` is a console based program that lets you see what joysticks 
and game controllers are detected by SDL and configure SDL mappings. 
In most cases, once running, the program can be controlled exclusively by the
connected joystick/gamepad.

`m8c` - includes a config helper for m8c that will translate 
button inputs to the proper values and output a snippet that can be dropped 
in to the m8c config.ini file found in `/home/<user>/.local/share/m8c/`

`EmulationStation` - includes scripts to run it directly from EmulationStation. 
Currently tested in ArkOS, but should work with 351ELEC or TheRA and 
possibly other rpi-based distributions.


## Running from the console

Make sure any other programs that might be capturing joystick input are closed.


### Commands
```  -h, --help                Print this help
  --version                 Print version number and exit
  -l, --list                List all information for available
                              joysticks
  -ls, --listsimple         List index and name for available
                              joysticks
  -g, --gamepad PADNUM      Display ids from selected gamepad
  -c, --config PADNUM       Configure the SDL gamepad mapping
  -m, --m8c PADNUM          Modify config for m8c
  -j, --joystick JOYNUM     Display ids from selected joystick
  -r, --rumble JOYNUM       Test rumble on selected joystick
```

I suggest running in this order
1. List the joysticks.
2. Test the joystick.
3. Configure and test the gamepad.
   - Configuring the gamepad will give you the option to update the game controller
database.
#### Examples
Test the joystick or gamepad
```
./gamepad_config -j 0
./gamepad_config -g 0
```
Configure the gamepad mapping
```
./gamepad_config -c 0
```
 

## Building

To build, you will need to install the dependency - libsdl2-dev

On a Debian-based distribution this can be done like so:

```sh
sudo apt-get install libsdl2-dev 
```

To compile simply cd to the directory and run make

```sh
cd gamepad_config
make
```


## Shell scripts for systems, consoles and handhelds running EmulationStation 

A number of scripts have been included that will make it easy to run directly 
from EmulationStation.

These scripts direct the output of `gamepad_config` to the main display.

 - Drop the entire `gamepad_config` folder in the "ports" directory.
 - Restart EmulationStation.

The scripts should be available in the "PORTS" menu.


## Troubleshooting
- Make sure your mapping is in your application's gamecontrollerdb.txt  
  (for m8c - `/home/<user>/.local/share/m8c/` - you may have to create it)

- Some applications may not have or read the gamecontrollerdb.txt file

- If your application is still not detecting the gamepad, you may need to add the line  
  ```export SDL_GAMECONTROLLERCONFIG=<your mapping>```  
  before running your application 
  (or put in a shell script that calls your application)
