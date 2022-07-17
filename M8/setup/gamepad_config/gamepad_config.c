/* gamepad_config.c

Console based gamepad configurator for SDL mappings - with m8c config helper

Based somewhat loosely on `jstest-sdl` https://github.com/meleu/jstest-sdl 
(which is in turn based on `sdl-jstest` http://github.com/Grumbel/sdl-jstest)

Changes: 06/25/2022 
    Added many configuration functions
    Changed the names and options of many functions
    Added loading gamecontrollerdb.txt file
    Removed ncurses dependency
    Added code to handle controlling the input from the joystick/gamepad
*/

// ORIGINAL DISCLAIMER and LICENSE
///////////////////////////////////////////////////////////////////////////////
// sdl-jstest - Joystick Test Program for SDL
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
///////////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
//#include <stdlib.h>

/* constants ***************************************************/
#define VERSION "0.0.1-smoot"

#define HELP_MESSAGE "\
List available joysticks or test a joystick.\n\
This programm uses SDL2 instead of using the raw interface\n\
\n\
Options:\n\
  -h, --help                Print this help\n\
  --version                 Print version number and exit\n\
  -l, --list                List all information for available\n\
                              joysticks\n\
  -ls, --listsimple         List index and name for available\n\
                              joysticks\n\
  -g, --gamepad PADNUM      Display ids from selected gamepad\n\
  -c, --config PADNUM       Configure the SDL gamepad mapping\n\
  -m, --m8c PADNUM          Modify config for m8c\n\
  -j, --joystick JOYNUM     Display ids from selected joystick\n\
  -r, --rumble JOYNUM       Test rumble on selected joystick\n\n"

/* functions **************************************************/

int wait_quit() {
    SDL_Event event;
    int quit = 0;
    int buttondown = 0;

    printf("\nPress two buttons at once on the selected joystick to continue.\n");

    while(!quit && SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_JOYBUTTONDOWN:
                buttondown += 1;
                break;

            case SDL_JOYBUTTONUP:
                buttondown = 0;
                break;

            case SDL_JOYHATMOTION:
                if (event.jhat.value >0) buttondown += 1;  else buttondown = 0; 
                break;

            case SDL_QUIT:
                quit = 1;
                break;

            default:
                break;
        }
        if (buttondown > 1) quit = 1;
    }
    printf("\n");
    return quit;
}


void simple_list_joysticks() {
    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks < 0)
        printf("No joysticks were found\n");
    else
        for(int i = 0; i < num_joysticks; i++)
            printf("%d: %s\n", i, SDL_JoystickNameForIndex(i));
}


int str2int(const char* str, int* val) {
    char* endptr;
    errno = 0;    /* To distinguish success/failure after call */

    *val = strtol(str, &endptr, 10);

    /* Check for various possible errors */
    if ((errno == ERANGE && (*val == LONG_MAX || *val == LONG_MIN))
            || (errno != 0 && *val == 0)) {
        return 0;
    }

    if (endptr == str) {
        return 0;
    }

    return 1;
}


void print_joystick_info(int joy_idx, SDL_Joystick* joy, SDL_GameController* gamepad) {
    int num_axes    = SDL_JoystickNumAxes(joy);
    int num_buttons = SDL_JoystickNumButtons(joy);
    int num_hats    = SDL_JoystickNumHats(joy);
    int num_balls   = SDL_JoystickNumBalls(joy);

    SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
    char guid_str[1024];
    SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
                
    // override for OpenSimHardware OSH PB Controller on rg351v
    if (strcmp(guid_str, "03000000091200000031000011010000") == 0) {
        num_axes    = 2;
        num_buttons = 12;
        num_hats    = 1;
        num_balls   = 0;
    }

    printf("Joystick Number:   %2d\n", joy_idx);
    printf("Joystick GUID:      %s\n", guid_str);
    printf("Joystick Name:      %s\n", SDL_JoystickName(joy));
    printf("Number of Axes:    %2d\n", num_axes);
    printf("Number of Buttons: %2d\n", num_buttons);
    printf("Number of Hats:    %2d\n", num_hats);
    printf("Number of Balls:   %2d\n", num_balls);
    printf("GameController:\n");
    if (!gamepad) {
        printf("  not a gamepad\n");
    } else {
        printf(" Name:    '%s'\n", SDL_GameControllerName(gamepad));
        printf(" Mapping: '%s'\n", SDL_GameControllerMappingForGUID(guid));
    }
    printf("\n");
}


void print_help(const char* prg) {
    printf("Usage: %s [OPTION]\n", prg);
    printf(HELP_MESSAGE);
    printf("Examples:\n");
    printf("  %s --list\n", prg);
    printf("  %s --joystick 1\n", prg);
}


void list_joysticks() {
    // for debugging
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks == 0) {
        printf("No joysticks were found\n");
    } else {
        printf("Found %d joystick(s)\n\n", num_joysticks);
        for(int joy_idx = 0; joy_idx < num_joysticks; ++joy_idx) {
            SDL_Joystick* joy = SDL_JoystickOpen(joy_idx);
            if (!joy) {
                fprintf(stderr, "Unable to open joystick %d\n", joy_idx);
            } else {
                SDL_GameController* gamepad = SDL_GameControllerOpen(joy_idx);
                print_joystick_info(joy_idx, joy, gamepad);
                if (gamepad) {
                    SDL_GameControllerClose(gamepad);
                }

                wait_quit();
                SDL_JoystickClose(joy);
            }
        }
    }
}


void get_gamecontroller_state(SDL_GameController* gamepad) {
    assert(gamepad);

    printf("Entering test loop.\nTO EXIT: press any two buttons at once or Ctrl-c.\n\n");
    int quit = 0;
    int buttondown = 0;
    SDL_Event event;

    while(!quit && SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                quit = 1;
                printf("\nReceived interrupt, exiting\n");
                break;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERAXISMOTION) {
            for(int btn = 0; btn < SDL_CONTROLLER_BUTTON_MAX; ++btn) {
                if (SDL_GameControllerGetButton(gamepad, btn)!=0) {
                    printf("button: %s \n", SDL_GameControllerGetStringForButton(btn));
                    buttondown += 1;
                }
            }
            if (event.type == SDL_CONTROLLERAXISMOTION)
                for(int axis = 0; axis < SDL_CONTROLLER_AXIS_MAX; ++axis) {
                    if (SDL_GameControllerGetAxis(gamepad, axis)!=0 && SDL_GameControllerGetAxis(gamepad, axis)!=-1)
                    printf("axis: %s value:%d\n",SDL_GameControllerGetStringForAxis(axis), SDL_GameControllerGetAxis(gamepad, axis));
            }
        }
        if (event.type == SDL_CONTROLLERBUTTONUP) buttondown = 0;
        if (buttondown > 1) quit = 1;
    }
}


void test_gamecontroller(int gamecontroller_idx) {
    SDL_GameController* gamepad = SDL_GameControllerOpen(gamecontroller_idx);
    if (!gamepad) {
        printf("Joystick %d is not a configured gamecontroller.\n", gamecontroller_idx);
        printf("Please configure gampepad %d, then try again.\n", gamecontroller_idx);
        SDL_Delay(2000);
    } else {
        printf("GameController: %d:%s\n", gamecontroller_idx, SDL_GameControllerNameForIndex(gamecontroller_idx));
        SDL_Joystick* joy = SDL_JoystickOpen(gamecontroller_idx);
        SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
        char guid_str[1024];
        SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
        printf(" Mapping: '%s'\n", SDL_GameControllerMappingForGUID(guid));
        SDL_JoystickClose(joy);
        printf("\n");

        get_gamecontroller_state(gamepad);

        SDL_GameControllerClose(gamepad);
    }
}


void test_joystick(int joy_idx) {
    SDL_Joystick* joy = SDL_JoystickOpen(joy_idx);
    if (!joy) {
        printf("Unable to open joystick %d\n", joy_idx);
        SDL_Delay(2000);
    } else {
        SDL_GameController* gamepad = SDL_GameControllerOpen(joy_idx);
        printf("Joystick: %d:%s\n", joy_idx, SDL_JoystickNameForIndex(joy_idx));
        if (gamepad) {
            SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
            char guid_str[1024];
            SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
            printf(" Mapping: '%s'\n\n", SDL_GameControllerMappingForGUID(guid));
            SDL_GameControllerClose(gamepad);
        } else {
            printf(" is not a configured gamepad.\n\n");
        }

        printf("Entering test loop.\nTO EXIT: press any two buttons at once or Ctrl-c.\n\n");
        int quit = 0;
        int buttondown = 0;
        SDL_Event event;

        while(!quit && SDL_WaitEvent(&event)) {
            switch(event.type) {
                case SDL_JOYAXISMOTION:
                    printf("axis id: a%d~ value: %d\n",
                            event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                    printf("button id: b%d\n",
                            event.jbutton.button);
                    buttondown += 1;
                    break;

                case SDL_JOYBUTTONUP:
                    //printf("SDL_JOYBUTTONUP: joystick: %d button: %d state: %d\n",
                    //        event.jbutton.which, event.jbutton.button, event.jbutton.state);
                    buttondown = 0;
                    break;

                case SDL_JOYHATMOTION:
                    if (event.jhat.value >0) {
                        printf("hat id: h%d.%d\n",event.jhat.hat, event.jhat.value );
                    }
                    break;

                case SDL_JOYBALLMOTION:
                    printf("ball: %d x: %d y: %d\n",
                            event.jball.ball, event.jball.xrel, event.jball.yrel);
                    break;

                case SDL_QUIT:
                    quit = 1;
                    printf("\nReceived interrupt, exiting\n");
                    break;

                default:
                    // fprintf(stderr, "Error: Unhandled event type: %d\n", event.type);
                    break;
            }
            if (buttondown > 1) quit = 1;
        }
        SDL_JoystickClose(joy);
    }
}


void test_rumble(int joy_idx) {
    SDL_Joystick* joy = SDL_JoystickOpen(joy_idx);
    if (!joy) {
        fprintf(stderr, "Unable to open joystick %d\n", joy_idx);
    } else {
        SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joy);
        if (!haptic) {
            fprintf(stderr, "Unable to open haptic on joystick %d\n", joy_idx);
            fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
        } else {
            if (!SDL_HapticRumbleSupported(haptic)) {
                fprintf(stderr, "rumble not supported on joystick %d\n", joy_idx);
            } else {
                if (SDL_HapticRumbleInit(haptic) != 0) {
                    fprintf(stderr, "failed to init rumble\n");
                } else {
                    SDL_HapticRumblePlay(haptic, 1.0, 3000);
                    SDL_Delay(3000);
                }
            }
            SDL_HapticClose(haptic);
        }
        SDL_JoystickClose(joy);
    }
}


void get_config_event(SDL_Event event, char* button, char* aButton, int ignoreAxis) {
    int quit = 0;
    int axis = 0;

    while(!quit && SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_JOYAXISMOTION:
                //printf("SDL_JOYAXISMOTION: joystick: %d axis: %d value: %d\n",
                //        event.jaxis.which, event.jaxis.axis, event.jaxis.value);
                if (!ignoreAxis && event.jaxis.value !=-1) {
                     axis = 1;
                    snprintf (button, 12, "a%d~", event.jaxis.axis);
                    //printf("axis val %d",event.jaxis.value);
                }
                break;

            case SDL_JOYBUTTONDOWN:
                //printf("SDL_JOYBUTTONDOWN: joystick: %d button: %d state: %d  id: b%d\n",
                //        event.jbutton.which, event.jbutton.button, event.jbutton.state, event.jbutton.button);
                quit = 1;
                //printf("about to assign\n");
                char tmp[12];
                snprintf(tmp,12,"b%d",event.jbutton.button);
                if (strcmp(tmp,aButton) ==0) {
                    snprintf (button, 12, "%s", "");
                    //printf("here %s", button);
                    break;
                }
                //printf("axis:%d, tmp:%s, abutton:%s",axis,tmp,aButton);
                if (!axis) snprintf (button, 12, "b%d", event.jbutton.button);
                break;

            case SDL_JOYBUTTONUP:
                //printf("SDL_JOYBUTTONUP: joystick: %d button: %d state: %d\n",
                //        event.jbutton.which, event.jbutton.button, event.jbutton.state);
                break;

            case SDL_JOYHATMOTION:
                if (event.jhat.value >0) {
                    //printf("SDL_JOYHATMOTION: joystick: %d hat: %d value: %d",
                    //    event.jhat.which, event.jhat.hat, event.jhat.value);
                    //printf("  id: h%d.%d\n",event.jhat.hat, event.jhat.value );
                quit = 1;
                snprintf (button, 6, "h%d.%d", event.jhat.hat, event.jhat.value);
                }
                break;

            case SDL_JOYBALLMOTION:
                //printf("SDL_JOYBALLMOTION: joystick: %d ball: %d x: %d y: %d\n",
                //        event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
                quit = 1;
                break;

            case SDL_JOYDEVICEADDED:
                //printf("SDL_JOYDEVICEADDED which:%d\n", event.jdevice.which);
                break;

            case SDL_JOYDEVICEREMOVED:
                //printf("SDL_JOYDEVICEREMOVED which:%d\n", event.jdevice.which);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                //printf("SDL_CONTROLLERBUTTONDOWN\n");
                break;

            case SDL_CONTROLLERBUTTONUP:
                //printf("SDL_CONTROLLERBUTTONUP\n");
                break;

            case SDL_CONTROLLERDEVICEADDED:
                //printf("SDL_CONTROLLERDEVICEADDED which:%d\n", event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                //printf("SDL_CONTROLLERDEVICEREMOVED which:%d\n",  event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMAPPED:
                printf("SDL_CONTROLLERDEVICEREMAPPED which:%d\n", event.cdevice.which);
                break;

            case SDL_QUIT:
                quit = 1;
                printf("\nReceived interrupt, exiting\n");
                break;

            default:
                fprintf(stderr, "Error: Unhandled event type: %d\n", event.type);
                break;
        }
    }
    //printf("button was %s\n",button);
}


void config(int joy_idx) {
    SDL_Joystick* joy = SDL_JoystickOpen(joy_idx);
    if (!joy) {
        printf("Unable to open joystick %d\n", joy_idx);
        SDL_Delay(2000);
    } else {
        SDL_GameController* gamepad = SDL_GameControllerOpen(joy_idx);
        //print_joystick_info(joy_idx, joy, gamepad);
        printf("Joystick: %d:%s\n\n", joy_idx, SDL_JoystickNameForIndex(joy_idx));
       if (gamepad) {
            SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
            char guid_str[1024];
            SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
            printf(" Mapping: '%s'\n\n", SDL_GameControllerMappingForGUID(guid));
            SDL_GameControllerClose(gamepad);
        } else {
            printf(" no current gamepad mapping.\n\n");
        }
        SDL_Event event;

        printf("Starting configuration:\n\n");
        char button[SDL_CONTROLLER_BUTTON_MAX][12] = {};
        char tmp[12] = "";
        for (int i = 0; i< SDL_CONTROLLER_BUTTON_MAX; i++) {
            // printf("b14:%s",button[14]);
            //printf("i:%d button:%s tmp:%s\n",i,button[i],tmp);
            // printf("button: %s\n",SDL_GameControllerGetStringForButton(i));
            printf ("Press the %s button",SDL_GameControllerGetStringForButton(i));
            if (i>3) printf (". Press the A button to skip.");
            if (i==3) snprintf(tmp, 12, "%s", button[0]);
            printf("\n");
            //printf("i:%d button:%s tmp:%s\n",i,button[i],tmp);
            get_config_event(event, button[i], tmp, 1);
            if (strcmp(button[i],"")!=0) printf("          button: %s\n",button[i]);
            //printf("i:%d button:%s tmp:%s\n",i,button[i],tmp);
        } 
        //snprintf(tmp,12, "%s", button[0]);
        char axis[SDL_CONTROLLER_AXIS_MAX][12] = {};
        for (int i = 0; i< SDL_CONTROLLER_AXIS_MAX; i++) {
            // printf("axis: %s\n",SDL_GameControllerGetStringForAxis(i));
            printf ("Push the %s ",SDL_GameControllerGetStringForAxis(i));
            if ((strcmp(SDL_GameControllerGetStringForAxis(i), "lefttrigger") !=0) && 
                (strcmp(SDL_GameControllerGetStringForAxis(i), "righttrigger") !=0))
                printf ("axis then press B or");
            printf (" Press the A button to skip.");
            printf("\n");
            //printf("%s\n",tmp);
            get_config_event(event, axis[i], tmp, 0);
            if (strcmp(axis[i],"")!=0) printf("          axis: %s\n",axis[i]);
        }

        SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
        char guid_str[1024];
        SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
        printf("\nNew mapping is:\n");
        printf("%s,%s,",guid_str,SDL_JoystickName(joy));
        for (int i = 0; i< SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (strcmp(button[i],"")!=0) printf("%s:%s,",SDL_GameControllerGetStringForButton(i),button[i]);
        }
        for (int i = 0; i< SDL_CONTROLLER_AXIS_MAX; i++) {
            if (strcmp(axis[i],"")!=0) printf("%s:%s,",SDL_GameControllerGetStringForAxis(i),axis[i]);
        }
        const char* sdlPlatform = SDL_GetPlatform();
        printf("platform:%s,\n",sdlPlatform);

        // write to file
        char config_path[1024] = {0};
        snprintf(config_path, sizeof(config_path), "gamepad-%d-mapping.txt", joy_idx);
        printf("\nWriting to file %s\n",config_path);
        FILE *f = fopen(config_path, "w");
        if (f == NULL)
        {
            printf("Error opening file!\n");
        } else {
            fprintf(f,"%s,%s,",guid_str,SDL_JoystickName(joy));
            for (int i = 0; i< SDL_CONTROLLER_BUTTON_MAX; i++) {
                if (strcmp(button[i],"")!=0) fprintf(f,"%s:%s,",SDL_GameControllerGetStringForButton(i),button[i]);
            }
            for (int i = 0; i< SDL_CONTROLLER_AXIS_MAX; i++) {
                if (strcmp(axis[i],"")!=0) fprintf(f,"%s:%s,",SDL_GameControllerGetStringForAxis(i),axis[i]);
            }
            fprintf(f,"platform:%s,\n",sdlPlatform);
            fclose(f);
        }

        // file to concatenate to
        snprintf(config_path, sizeof(config_path), "gamepad-mapping.txt");
        printf("Compiling all defined gamepads to %s\n\n",config_path);
        char input_path[1024] = {0};
        f = fopen(config_path, "w");
        if (f == NULL)
        {
            printf("Error opening file!\n\n");
        } else {
            // concatenate all existing configs into gamepad-mapping.txt
            for (int j = 0; j < 10; j++) {
                // open FILE
                snprintf(input_path, sizeof(input_path), "gamepad-%d-mapping.txt", j);
                FILE *f1 = fopen(input_path, "r");
                if (f1 != NULL) {
                    int c;
                    int failover = 0;
                    while ((c = fgetc(f1)) != EOF && failover < 2048) {
                        fputc(c, f);
                        //printf("%c",c);
                        failover++;
                    }
                    fclose(f1);
                }
            }
            fclose(f);
        }

        if (!gamepad) printf("Copy the mapping in this file to your gamecontrollerdb.txt file.\n");
        else printf("Copy and overwrite the mapping in gamecontrollerdb.txt with the contents of this file.\n");

        printf("\nIf your application is still not detecting the gamepad,\nyou may need to add the line\n");
        printf("    export SDL_GAMECONTROLLERCONFIG=\"<your mapping>\"\nbefore running your application.\n");

        wait_quit();

        SDL_JoystickClose(joy);
    }
}


void get_gamepad_button(SDL_GameController* gamepad, int* button) {
    SDL_Event event;
    int quit = 0;
    while(!quit && SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                quit = 1;
                *button = -1;
                printf("\n");
                break;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN) {
            for(int btn = 0; btn < SDL_CONTROLLER_BUTTON_MAX; ++btn) {
                if (SDL_GameControllerGetButton(gamepad, btn)!=0) {
                    *button = btn;
                    quit = 1;
                }
            }
        }
    }
}


void get_gamepad_axis(SDL_GameController* gamepad, int* button) {
    SDL_Event event;
    int buttondown = 0;
    int quit = 0;
    while(!quit && SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                quit = 1;
                *button = -1;
                printf("\n");
                break;
        }
        if (event.type == SDL_CONTROLLERBUTTONDOWN) {
            for(int btn = 0; btn < SDL_CONTROLLER_BUTTON_MAX; ++btn) {
                if (SDL_GameControllerGetButton(gamepad, btn)!=0) {
                    if (strcmp("a",SDL_GameControllerGetStringForButton(btn))==0) *button = -1;
                    quit = 1;
                }
            }
            buttondown += 1;
        }
        if (event.type == SDL_CONTROLLERAXISMOTION)
            for(int axis = 0; axis < SDL_CONTROLLER_AXIS_MAX; ++axis) {
                if (SDL_GameControllerGetAxis(gamepad, axis)!=0 && SDL_GameControllerGetAxis(gamepad, axis)!=-1) {
                    *button = axis;
                }
            }
        if (buttondown > 0) {
            quit = 1;
        }
    }
}


void config_m8c(int idx) {
    SDL_GameController* gamepad = SDL_GameControllerOpen(idx);
    if (!gamepad) {
        printf("Joystick %d is not a configured gamecontroller.\n", idx);
        printf("Please configure gampepad %d, then try again.\n", idx);
        SDL_Delay(2000);
    } else {
        printf("GameController: %d:%s\n", idx, SDL_GameControllerNameForIndex(idx));
        SDL_Joystick* joy = SDL_JoystickOpen(idx);
        SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
        char guid_str[1024];
        SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
        printf(" Mapping: '%s'\n", SDL_GameControllerMappingForGUID(guid));
        SDL_JoystickClose(joy);
        printf("\n\n");

        printf("M8 keys reference:\n");
        printf("|            _   _   | \n");
        printf("|           |_| |_|  | \n");
        printf("|      _ option edit | \n");
        printf("|     |_|            |\n");
        printf("|  _   _   _         |\n");
        printf("| |_| |_| |_|        |\n");
        printf("|       _   _        |\n");
        printf("|      |_| |_|       |\n");
        printf("|    shift play     |\n");
        printf("|                    |\n");
        printf("\n");
        
        char config_btn[10][20] = {"Option", "Edit", "Shift (select)", "Play (start)", "Up", "Down", "Left", 
                            "Right", "Quit", "Display Reset"};
        int button[10] = {};
        for (int i=0;i<10;i++) {
            printf("Choose %s button:\n",config_btn[i]);
            get_gamepad_button(gamepad, &button[i]);
            printf("          button: %s number:%d\n", SDL_GameControllerGetStringForButton(button[i]), button[i]);
        }
        printf("\n");
        char config_axis[6][20] = {"Option", "Edit", "Shift (select)", "Play (start)", "Up/Down", "Left/Right"};
        int axis[6] = {};
        for (int i=0;i<6;i++) {
            printf("Choose axis %s and press B to confirm, or A to skip:\n",config_axis[i]);
            get_gamepad_axis(gamepad, &axis[i]);
            printf("          axis: %s number:%d\n",SDL_GameControllerGetStringForAxis(axis[i]), axis[i]);
        }

        printf("\nNew M8 config snippet:\n\n");
        printf("gamepad_up=%d\n",button[4]);
        printf("gamepad_left=%d\n",button[6]);
        printf("gamepad_down=%d\n",button[5]);
        printf("gamepad_right=%d\n",button[7]);
        printf("gamepad_select=%d\n",button[2]);
        printf("gamepad_start=%d\n",button[3]);
        printf("gamepad_opt=%d\n",button[0]);
        printf("gamepad_edit=%d\n",button[1]);
        printf("gamepad_quit=%d\n",button[8]);
        printf("gamepad_reset=%d\n",button[9]);
        printf("gamepad_analog_threshold=32766\n");
        printf("gamepad_analog_invert=false\n");
        printf("gamepad_analog_axis_updown=%d\n",axis[4]);
        printf("gamepad_analog_axis_leftright=%d\n",axis[5]);
        printf("gamepad_analog_axis_select=%d\n",axis[2]);
        printf("gamepad_analog_axis_start=%d\n",axis[3]);
        printf("gamepad_analog_axis_opt=%d\n",axis[0]);
        printf("gamepad_analog_axis_edit=%d\n",axis[1]);
        printf("\n");

        printf("Do you want to save this config? Press B for yes, or A to skip:\n");
        int confirm;
        get_gamepad_button(gamepad, &confirm);
        if (-1 != confirm && strcmp("b", SDL_GameControllerGetStringForButton(confirm))==0) {
            // write to file
            char config_path[1024] = {0};
            snprintf(config_path, sizeof(config_path), "m8c_config_snippet.txt");
            printf("\nWriting to file %s\n",config_path);
            FILE *f = fopen(config_path, "w");
            if (f == NULL)
            {
                printf("Error opening file!\n");
            } else {
                fprintf(f,"gamepad_up=%d\n",button[4]);
                fprintf(f,"gamepad_left=%d\n",button[6]);
                fprintf(f,"gamepad_down=%d\n",button[5]);
                fprintf(f,"gamepad_right=%d\n",button[7]);
                fprintf(f,"gamepad_select=%d\n",button[2]);
                fprintf(f,"gamepad_start=%d\n",button[3]);
                fprintf(f,"gamepad_opt=%d\n",button[0]);
                fprintf(f,"gamepad_edit=%d\n",button[1]);
                fprintf(f,"gamepad_quit=%d\n",button[8]);
                fprintf(f,"gamepad_reset=%d\n",button[9]);
                fprintf(f,"gamepad_analog_threshold=32766\n");
                fprintf(f,"gamepad_analog_invert=false\n");
                fprintf(f,"gamepad_analog_axis_updown=%d\n",axis[4]);
                fprintf(f,"gamepad_analog_axis_leftright=%d\n",axis[5]);
                fprintf(f,"gamepad_analog_axis_select=%d\n",axis[2]);
                fprintf(f,"gamepad_analog_axis_start=%d\n",axis[3]);
                fprintf(f,"gamepad_analog_axis_opt=%d\n",axis[0]);
                fprintf(f,"gamepad_analog_axis_edit=%d\n",axis[1]);
                fclose(f);
            }
            printf("\nYou'll need to manually update these values in the config.ini\n file in: %s\n", SDL_GetPrefPath("", "m8c"));
            printf("\nPLEASE NOTE: to take advantage of \"Quit\" and \"Display Reset\"\n");
            printf("on the gamepad, you must press SHIFT+quit or SHIFT+reset\n");
            printf("to trigger those actions.\n");
        }

        wait_quit();
    }
}


void load_gamecontrollers() {
    char db_filename[1024] = {0};
    // Try to load the game controller database file in the m8c prefs dir
    snprintf(db_filename, sizeof(db_filename), "%sgamecontrollerdb.txt",
            SDL_GetPrefPath("", "m8c"));
    SDL_Log("Trying to open game controller database from %s", db_filename);
    SDL_RWops* db_rw = SDL_RWFromFile(db_filename, "rb");
    if (db_rw == NULL) {
        // Try to load the game controller database file in the gamepad_config dir
        snprintf(db_filename, sizeof(db_filename), "%sgamecontrollerdb.txt",
        SDL_GetBasePath());
        SDL_Log("Trying to open game controller database from %s", db_filename);
        db_rw = SDL_RWFromFile(db_filename, "rb");
    }

    if (db_rw != NULL) {
        int mappings = SDL_GameControllerAddMappingsFromRW(db_rw, 1);
        if (mappings != -1) {
            SDL_Log("Found %d game controller mappings", mappings);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT,
                        "Error loading game controller mappings.");
            SDL_Log("Error loading game controller mappings.");
        }
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_INPUT,
                    "Unable to open game controller database file.");
        SDL_Log("Unable to open game controller database file.");
    }

    // read from the generated gamepad-mappings, if they exist
    snprintf(db_filename, sizeof(db_filename), "%sgamepad-mapping.txt",
    SDL_GetBasePath());
    SDL_Log("Trying to open game controller database from %s", db_filename);
    db_rw = SDL_RWFromFile(db_filename, "rb");

    if (db_rw != NULL) {
        int mappings = SDL_GameControllerAddMappingsFromRW(db_rw, 1);
        if (mappings != -1) {
            SDL_Log("Found %d game controller mappings", mappings);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT,
                        "Error loading game controller mappings.");
            SDL_Log("Error loading game controller mappings.");
        }
    } else {
        SDL_Log("Unable to open game controller database file.");
    }
}


/* main() ********************************************************************/
int main(int argc, char** argv) {

char test[12];
    if (argc == 1) {
        print_help(argv[0]);
        exit(1);
    }

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);

    load_gamecontrollers();

    // SDL2 will only report events when the window has focus, so set
    // this hint as we don't have a window
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    // FIXME: We don't need video, but without it SDL will fail to work in SDL_WaitEvent()
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    } else {
        atexit(SDL_Quit);

        if (argc == 2 && (strcmp(argv[1], "--help") == 0 ||
                          strcmp(argv[1], "-h") == 0)) {
            print_help(argv[0]);
        }
        else if (argc == 2 && (strcmp(argv[1], "--version") == 0)) {
            puts(VERSION);
            exit(EXIT_SUCCESS);
        }
        else if (argc == 2 && (strcmp(argv[1], "--list") == 0 ||
                              (strcmp(argv[1], "-l") == 0))) {
            list_joysticks();
        }
        else if (argc == 2 && (strcmp(argv[1], "--listsimple") == 0 ||
                              (strcmp(argv[1], "-ls") == 0))) {
            simple_list_joysticks();
        }
        else if (argc == 3 && (strcmp(argv[1], "--gamecontroller") == 0 ||
                               strcmp(argv[1], "-g") == 0)) {
            int idx;
            if (!str2int(argv[2], &idx)) {
                fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
                exit(1);
            } else {
                test_gamecontroller(idx);
            }
        }
        else if (argc == 3 && (strcmp(argv[1], "--joystick") == 0 ||
                               strcmp(argv[1], "-j") == 0)) {
            int joy_idx;
            if (!str2int(argv[2], &joy_idx)) {
                fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
                exit(1);
            }
            test_joystick(joy_idx);
        }
        else if (argc == 3 && (strcmp(argv[1], "--m8c") == 0 ||
                               strcmp(argv[1], "-m") == 0)) {
            int idx;
            if (!str2int(argv[2], &idx)) {
                fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
                exit(1);
            } else {
                config_m8c(idx);
            }
        }
        else if (argc == 3 && (strcmp(argv[1], "--rumble") == 0 ||
                               strcmp(argv[1], "-r") == 0)) {
            int idx;
            if (!str2int(argv[2], &idx)) {
                fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
                exit(1);
            } else {
                test_rumble(idx);
            }
        }
        else if (argc == 3 && (strcmp(argv[1], "--config") == 0 ||
                               strcmp(argv[1], "-c") == 0)) {
            int idx;
            if (!str2int(argv[2], &idx)) {
                fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
                exit(1);
            } else {
                config(idx);
            }
        } else {
            fprintf(stderr, "%s: unknown arguments\n", argv[0]);
            fprintf(stderr, "Try '%s --help' for more informations\n", argv[0]);
        }
    }
}

/* EOF */