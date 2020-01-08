/*--------------------------------------------------------------------------*\
|                                                                            |
|     __      __.__.___________  ______ _______  _______                     |
|    /  \    /  \__|__\______  \/  __  \\   _  \ \   _  \                    |
|    \   \/\/   /  |  |   /    />      </  /_\  \/  /_\  \                   |
|     \        /|  |  |  /    //   --   \  \_/   \  \_/   \                  |
|      \__/\  / |__|__| /____/ \______  /\_____  /\_____  /                  |
|           \/                        \/       \/       \/                   |
|                                                                            |
|    Wii7800 by raz0red                                                      |
|    Wii port of the ProSystem emulator developed by Greg Stanton            |
|                                                                            |
|    [github.com/raz0red/wii7800]                                            |
|                                                                            |
+----------------------------------------------------------------------------+
|                                                                            |
|    This program is free software; you can redistribute it and/or           |
|    modify it under the terms of the GNU General Public License             |
|    as published by the Free Software Foundation; either version 2          |
|    of the License, or (at your option) any later version.                  |
|                                                                            |
|    This program is distributed in the hope that it will be useful,         |
|    but WITHOUT ANY WARRANTY; without even the implied warranty of          |
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
|    GNU General Public License for more details.                            |
|                                                                            |
|    You should have received a copy of the GNU General Public License       |
|    along with this program; if not, write to the Free Software             |
|    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           |
|    02110-1301, USA.                                                        |
|                                                                            |
\*--------------------------------------------------------------------------*/

#include <malloc.h>
#include <string.h>

#include "ProSystem.h"
#include "Sound.h"

#include "wii_app.h"
#include "wii_config.h"
#include "wii_input.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"

#include "wii_atari.h"
#include "wii_atari_snapshot.h"

extern "C" {
void WII_VideoStart();
void WII_VideoStop();
}

#if 0
extern u32 sound_max;
#endif

/**
 * Starts the emulator for the specified rom file.
 *
 * @param   romfile The rom file to run in the emulator
 * @param   savefile The name of the save file to load. If this value is NULL,
 *          no save is explicitly loaded
 * @param   reset    Whether we are resetting the current game
 * @param   resume   Whether we are resuming the current game
 * @return  Whether emulation was successfully started
 */
BOOL wii_start_emulation(char* romfile,
                         const char* savefile,
                         BOOL reset,
                         BOOL resume) {
    // Disabled the high score cartridge
    wii_hs_enabled = (wii_hs_mode != HSMODE_DISABLED);

    // Write out the current config
    wii_write_config();

    // Whether emulation started successfully
    BOOL succeeded = TRUE;
    BOOL loadsave = FALSE;

    // Start emulation
    if (!reset && !resume) {
        // Whether to load a save file
        loadsave = (savefile != NULL && strlen(savefile) > 0);

        // Disable the high score cart for saves if applicable
        if (loadsave && (wii_hs_mode != HSMODE_ENABLED_SNAPSHOTS)) {
            wii_hs_enabled = false;
        }

        // If we are not loading a save, reset snapshot related state
        // information (current index, etc.)
        if (!loadsave) {
            wii_snapshot_reset();  // Reset snapshot related state
        }

        wii_reset_keyboard_data();
        succeeded = wii_atari_load_rom(romfile, !loadsave);

        if (succeeded) {
            if (loadsave) {
                // Ensure the save is valid
                int sscheck = wii_check_snapshot(savefile);
                if (sscheck < 0) {
                    if (sscheck == -2) {
                        wii_set_status_message(
                            "The save specified is not valid.");
                    } else {
                        wii_set_status_message(
                            "Unable to find the specified save state file.");
                    }
                    succeeded = false;
                } else {
                    // Run some test frames
                    //
                    // TODO:
                    //
                    // This is definitely a ProSystem emu bug. Basically, if
                    // you don't load the ROM and run some frames prior to
                    // loading save state the game may not work. This is due to
                    // the fact that it relies on state set when the ROM is
                    // initially loaded (I believe it is Maria state). This is
                    // something that should be resolved by adding Maria state
                    // to the save file. For now, we just load the ROM and let
                    // it execute for a second.
                    wii_atari_main_loop(prosystem_frequency);

                    succeeded = prosystem_Load(savefile);
                    if (succeeded) {
                        wii_atari_pause(false);
                    } else {
                        wii_set_status_message(
                            "An error occurred attempting to load the save "
                            "state file.");
                    }
                }
            }
        } else {
            wii_set_status_message(
                "An error occurred loading the specified cartridge.");
        }
    } else if (reset) {
        wii_reset_keyboard_data();
        prosystem_Reset();
    } else {
        prosystem_Pause(false);
    }

    if (succeeded) {
        // Store the name of the last rom (for resuming later)
        // Do it in this order in case they passed in the pointer
        // to the last rom variable
        char* last = strdup(romfile);
        char* old_last = wii_last_rom;
        wii_last_rom = last;
        if (old_last != NULL) {
            free(old_last);
        }

        if (wii_max_frame_rate != 0) {
            prosystem_frequency = wii_max_frame_rate;
        }

        // Clear the screen
        wii_sdl_black_screen();
        VIDEO_WaitVSync();

        // Wait until no buttons are pressed
        wii_wait_until_no_buttons(2);

#if 0
        sound_max = 0;
#endif        

        wii_atari_main_loop();

        if (wii_top_menu_exit) {
            // Pop to the top
            while (wii_menu_pop() != NULL)
                ;
        }
    } else {
        // Reset the last rom that was loaded
        char *old_last = wii_last_rom;
        wii_last_rom = NULL;
        if (old_last != NULL) {
            free(old_last);            
        }
    }

    return succeeded;
}

/**
 * Resumes emulation of the current game
 */
void wii_resume_emulation() {
    wii_start_emulation(wii_last_rom, "", FALSE, TRUE);
}

/**
 * Resets the current game
 */
void wii_reset_emulation() {
    wii_start_emulation(wii_last_rom, "", TRUE, FALSE);
}
