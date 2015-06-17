/*
Wii7800 : Port of the ProSystem Emulator for the Wii

Copyright (C) 2010
raz0red (www.twitchasylum.com)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#ifndef WII_APP_COMMON_H
#define WII_APP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define WII_SD_CARD "sd:"
#define WII_BASE_APP_DIR WII_SD_CARD "/apps/wii7800/"
#define WII_FILES_DIR WII_SD_CARD "/wii7800/"
#define WII_ROMS_DIR WII_FILES_DIR "roms/"
#define WII_SAVES_DIR WII_FILES_DIR "saves/"
#define WII_ROOT_BOOT_ROM_NTSC WII_FILES_DIR "7800.rom"
#define WII_ROOT_BOOT_ROM_PAL WII_FILES_DIR "7800pal.rom"
#define WII_CONFIG_FILE WII_FILES_DIR "wii7800.conf"
#define WII_PROSYSTEM_DB WII_FILES_DIR "ProSystem.dat"
#define WII_HIGH_SCORE_CART WII_FILES_DIR "highscore.rom"
#define WII_HIGH_SCORE_CART_SRAM WII_FILES_DIR "highscore.sram"
#define WII_SAVE_GAME_EXT "sav"

/*
 * The different types of nodes in the menu
 */
enum NODETYPE
{
    NODETYPE_ROOT,
    NODETYPE_SCALE,
    NODETYPE_LOAD_ROM,
    NODETYPE_ROM,
    NODETYPE_SPACER,
    NODETYPE_RESUME,
    NODETYPE_ADVANCED,
    NODETYPE_SNAPSHOT_MANAGEMENT,
    NODETYPE_DEBUG_MODE,
    NODETYPE_TOP_MENU_EXIT,
    NODETYPE_AUTO_LOAD_SNAPSHOT,
    NODETYPE_AUTO_SAVE_SNAPSHOT,
    NODETYPE_RESET,
    NODETYPE_SAVE_SNAPSHOT,
    NODETYPE_LOAD_SNAPSHOT,
    NODETYPE_DELETE_SNAPSHOT,
    NODETYPE_SNAPSHOT,
    NODETYPE_VSYNC,
    NODETYPE_MAX_FRAME_RATE,    
    NODETYPE_DIFF_SWITCH_DISPLAY,
    NODETYPE_DIFF_SWITCH_ENABLED,
    NODETYPE_SWAP_BUTTONS,
#if 0
    NODETYPE_DUAL_ANALOG,
#endif
    NODETYPE_HIGH_SCORE_MODE,
    NODETYPE_CARTRIDGE_SETTINGS,
    NODETYPE_CARTRIDGE_WSYNC,
    NODETYPE_CARTRIDGE_CYCLE_STEALING,
    NODETYPE_DISPLAY_SETTINGS,
    NODETYPE_CONTROLS_SETTINGS,
    NODETYPE_LIGHTGUN_CROSSHAIR,
    NODETYPE_LIGHTGUN_FLASH,
    NODETYPE_RESIZE_SCREEN
};

#ifdef __cplusplus
}
#endif

#endif
