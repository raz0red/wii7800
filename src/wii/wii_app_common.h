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

#ifndef WII_APP_COMMON_H
#define WII_APP_COMMON_H

#define WII_FILES_DIR "/wii7800/"
#define WII_ROMS_DIR WII_FILES_DIR "roms/"
#define WII_SAVES_DIR WII_FILES_DIR "saves/"
#define WII_ROOT_BOOT_ROM_NTSC WII_FILES_DIR "7800.rom"
#define WII_ROOT_BOOT_ROM_PAL WII_FILES_DIR "7800pal.rom"
#define WII_CONFIG_FILE WII_FILES_DIR "wii7800.conf"
#define WII_PROSYSTEM_DB WII_FILES_DIR "ProSystem.dat"
#define WII_HIGH_SCORE_CART WII_FILES_DIR "highscore.rom"
#define WII_HIGH_SCORE_CART_SRAM WII_FILES_DIR "highscore.sram"

#define WII_BASE_APP_DIR "sd:/apps/wii7800/"

#define WII_SAVE_GAME_EXT "sav"

/**
 * The different types of nodes in the menu
 */
enum NODETYPE {
    NODETYPE_ROOT,
    NODETYPE_SCALE,
    NODETYPE_LOAD_ROM,
    NODETYPE_ROOT_DRIVE,
    NODETYPE_UPDIR,
    NODETYPE_DIR,
    NODETYPE_ROM,
    NODETYPE_SPACER,
    NODETYPE_RESUME,
    NODETYPE_ADVANCED,
    NODETYPE_CARTRIDGE_SETTINGS,
    NODETYPE_CARTRIDGE_SETTINGS_SPACER,
    NODETYPE_CARTRIDGE_SAVE_STATES,
    NODETYPE_CARTRIDGE_SAVE_STATES_SLOT,
    NODETYPE_DEBUG_MODE,
    NODETYPE_TOP_MENU_EXIT,
    NODETYPE_AUTO_LOAD_SNAPSHOT,
    NODETYPE_AUTO_SAVE_SNAPSHOT,
    NODETYPE_RESET,
    NODETYPE_SAVE_STATE,
    NODETYPE_LOAD_STATE,
    NODETYPE_DELETE_STATE,
    NODETYPE_VSYNC,
    NODETYPE_MAX_FRAME_RATE,
    NODETYPE_DIFF_SWITCH_DISPLAY,
    NODETYPE_DIFF_SWITCH_ENABLED,
    NODETYPE_DISPLAY_SETTINGS,
    NODETYPE_CONTROLS_SETTINGS,
    NODETYPE_LIGHTGUN_CROSSHAIR,
    NODETYPE_LIGHTGUN_FLASH,
    NODETYPE_RESIZE_SCREEN,
    NODETYPE_WIIMOTE_MENU_ORIENT,
    NODETYPE_16_9_CORRECTION,
    NODETYPE_FULL_WIDESCREEN,
    NODETYPE_FILTER,
    NODETYPE_GX_VI_SCALER,
    NODETYPE_DOUBLE_STRIKE,
    NODETYPE_TRAP_FILTER,
    NODETYPE_CART_SETTINGS_CART_TYPE,
    NODETYPE_CART_SETTINGS_REGION,
    NODETYPE_CART_SETTINGS_SAVE,
    NODETYPE_CART_SETTINGS_DELETE,
    NODETYPE_CART_SETTINGS_WSYNC,    
    NODETYPE_CART_SETTINGS_CYCLE_STEALING,
    NODETYPE_CART_SETTINGS_POKEY,
    NODETYPE_CART_SETTINGS_XM,
    NODETYPE_CART_SETTINGS_DISABLE_BIOS,
    NODETYPE_CART_SETTINGS_LEFT_SWITCH,
    NODETYPE_CART_SETTINGS_RIGHT_SWITCH,
    NODETYPE_CART_SETTINGS_CART_SETTINGS,
    NODETYPE_CART_SETTINGS_DIFF_SWITCH_SETTINGS,
    NODETYPE_CART_SETTINGS_CONTROLS_SETTINGS,
    NODETYPE_CART_SETTINGS_SWAP_BUTTONS,
    NODETYPE_CART_SETTINGS_CONTROLLER1,
    NODETYPE_CART_SETTINGS_CONTROLLER2,
    NODETYPE_CART_SETTINGS_DUAL_ANALOG,
    NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_X,
    NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_Y,
    NODETYPE_CART_SETTINGS_CONTROLS_SPACER,
    NODETYPE_CART_SETTINGS_HBLANK,
    NODETYPE_CART_SETTINGS_HSC,    
    NODETYPE_ADVANCED_DIFF_SWITCH_SETTINGS
};

#endif


