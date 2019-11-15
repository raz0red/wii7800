/*
Wii7800 : Port of the ProSystem Emulator for the Wii

Copyright (C) 2010
raz0red (www.twitchasylum.com)
*/


#ifndef WII_ATARI_H
#define WII_ATARI_H

#include <gctypes.h>

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
#define DEFAULT_SCREEN_X 640
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

// The scanline that the lightgun shot occurred at
extern int lightgun_scanline;
// The cycle that the lightgun shot occurred at
extern float lightgun_cycle;
// Whether the lightgun is enabled for the current cartridge
extern bool lightgun_enabled;
// Whether this is a test frame
extern bool wii_testframe;

// Whether to flash the screen 
extern BOOL wii_lightgun_flash;
// Whether to display a crosshair for the lightgun
extern BOOL wii_lightgun_crosshair;
// Whether wsync is enabled/disabled
extern u8 wii_cart_wsync;
// Whether cycle stealing is enabled/disabled
extern u8 wii_cart_cycle_stealing;
// Whether high score cart is enabled
extern BOOL wii_hs_enabled;
// What mode the high score cart is in
extern BOOL wii_hs_mode;
// Whether to swap buttons
extern BOOL wii_swap_buttons;
// Whether the difficulty switches are enabled
extern BOOL wii_diff_switch_enabled;
// When to display the difficulty switches
extern BOOL wii_diff_switch_display;
// Whether to display debug info (FPS, etc.)
extern short wii_debug;
// The maximum frame rate
extern int wii_max_frame_rate;
// What is the display size?
extern u8 wii_scale;
// The screen X size
extern int wii_screen_x;
// The screen Y size
extern int wii_screen_y;
// Auto load snapshot?
extern BOOL wii_auto_load_snapshot;
// Auto save snapshot?
extern BOOL wii_auto_save_snapshot;

/*
 * Loads the specified ROM
 *
 * filename     The filename of the ROM
 * loadbios     Whether or not to load the Atari BIOS
 *
 * return   Whether the load was successful
 */
extern bool wii_atari_load_rom( char *filename, bool loadbios = true ) ;

/*
 * Resets the keyboard (control) information
 */
extern void wii_reset_keyboard_data();

/*
 * Runs the main Atari emulator loop
 *
 * testframes   The number of testframes to run (for loading saves)
 */
extern void wii_atari_main_loop( int testframes = -1 );

/*
 * Pauses the emulator
 *
 * pause    Whether to pause or resume
 */
extern void wii_atari_pause( bool pause );
#endif
