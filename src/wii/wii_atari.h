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

#ifndef WII_ATARI_H
#define WII_ATARI_H

#include <gctypes.h>
#include "wii_resize_screen.h"

// Dimensions of the surface that is being written to
// by the emulator
#define ATARI_WIDTH 320
#define ATARI_BLIT_HEIGHT 300

// NTSC
#define NTSC_ATARI_BLIT_TOP_Y 2
#define NTSC_ATARI_HEIGHT 240

// PAL
#define PAL_ATARI_BLIT_TOP_Y 26
#define PAL_ATARI_HEIGHT 240

// cart modes
#define CART_MODE_AUTO 0
#define CART_MODE_ENABLED 1
#define CART_MODE_DISABLED 2

// high score modes
#define HSMODE_DISABLED 0
#define HSMODE_ENABLED_NORMAL 1
#define HSMODE_ENABLED_SNAPSHOTS 2

// Default screen size
#define DEFAULT_SCREEN_X 548 // (6:7)
#define DEFAULT_SCREEN_Y 480

// vsync modes
#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1

// diff switches
#define DIFF_SWITCH_DISPLAY_DISABLED 0
#define DIFF_SWITCH_DISPLAY_ALWAYS 1
#define DIFF_SWITCH_DISPLAY_WHEN_CHANGED 2

// Typedefs
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long ullong;

/** The scanline that the lightgun shot occurred at */
extern int lightgun_scanline;
/** The cycle that the lightgun shot occurred at */
extern float lightgun_cycle;
/** Whether the lightgun is enabled for the current cartridge */
extern bool lightgun_enabled;
/** Whether this is a test frame */
extern bool wii_testframe;

/** Whether to flash the screen */
extern BOOL wii_lightgun_flash;
/** Whether to display a crosshair for the lightgun */
extern BOOL wii_lightgun_crosshair;
/** Whether wsync is enabled/disabled */
extern u8 wii_cart_wsync;
/** Whether cycle stealing is enabled/disabled */
extern u8 wii_cart_cycle_stealing;
/** Whether high score cart is enabled */
extern BOOL wii_hs_enabled;
/** What mode the high score cart is in */
extern BOOL wii_hs_mode;
/** Whether to swap buttons */
extern BOOL wii_swap_buttons;
/** Whether the difficulty switches are enabled */
extern BOOL wii_diff_switch_enabled;
/** When to display the difficulty switches */
extern BOOL wii_diff_switch_display;
/** Whether to display debug info (FPS, etc.) */
extern short wii_debug;
/** The maximum frame rate */
extern int wii_max_frame_rate;
/** The screen X size */
extern int wii_screen_x;
/** The screen Y size */
extern int wii_screen_y;
/** Whether to filter the display */
extern BOOL wii_filter;
/** Whether to use the GX/VI scaler */
extern BOOL wii_gx_vi_scaler;

/**
 * Returns the current roms directory
 *
 * @return  The current roms directory
 */
char* wii_get_roms_dir();

/**
 * Returns the saves directory
 *
 * @return  The saves directory
 */
char* wii_get_saves_dir();

/**
 * Sets the current roms directory
 *
 * @param   newDir The new roms directory
 */
void wii_set_roms_dir(const char* newDir);

/**
 * Updates whether the Wii is in widescreen mode
 */
void wii_update_widescreen();

/**
 * Loads the specified ROM
 *
 * @param   filename The filename of the ROM
 * @param   loadbios Whether or not to load the Atari BIOS
 * @return  Whether the load was successful
 */
bool wii_atari_load_rom(char* filename, bool loadbios = true);

/**
 * Resets the keyboard (control) information
 */
void wii_reset_keyboard_data();

/**
 * Runs the main Atari emulator loop
 *
 * @param   testframes The number of testframes to run (for loading saves)
 */
void wii_atari_main_loop(int testframes = -1);

/**
 * Pauses the emulator
 *
 * @param   pause Whether to pause or resume
 */
void wii_atari_pause(bool pause);

/**
 * Renders the current frame to the Wii
 */
void wii_atari_put_image_gu_normal();

/**
 * Returns the default screen sizes
 * 
 * @param   sizes (out) The array of screen sizes
 * @param   size_count (out) The count of screen sizes
 */
void wii_get_default_screen_sizes(const screen_size** sizes, int* size_count);
#endif
