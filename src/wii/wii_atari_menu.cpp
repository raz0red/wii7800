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

#include <stdio.h>
#include <stdlib.h>
#include <sys/iosupport.h>

#include "Cartridge.h"
#include "Region.h"

#include "wii_app_common.h"
#include "wii_main.h"
#include "wii_app.h"
#include "wii_gx.h"
#include "wii_resize_screen.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"
#include "wii_util.h"
#include "fileop.h"
#include "networkop.h"

#include "wii_app_common.h"
#include "wii_atari.h"
#include "wii_atari_emulation.h"
#include "wii_atari_snapshot.h"
#include "wii_atari_db.h"

#include "gettext.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

/** SDL Video external references */
extern "C" {
void WII_VideoStart();
void WII_VideoStop();
}

extern int PauseAudio(int Switch);
extern void ResetAudio();

/** Whether we are loading a game */
BOOL loading_game = FALSE;
/** Have we read the games list yet? */
static BOOL games_read = FALSE;
/** Whether we are pending a drive mount */
static BOOL mount_pending = TRUE;
/** The index of the last rom that was run */
static s16 last_rom_index = 1;
/** The roms node */
static TREENODE* roms_menu;

// Forward refs
static void wii_read_game_list(TREENODE* menu);

/**
 * Returns the space node type
 *
 * @return  The spacer node type
 */
int wii_get_nodetype_spacer() {
    return NODETYPE_SPACER;
}

/**
 * Returns the rom node type
 *
 * @return  The rom node type
 */
int wii_get_nodetype_rom() {
    return NODETYPE_ROM;
}

/**
 * Initializes the Atari menu
 */
void wii_atari_menu_init() {
    //
    // The root menu
    //

    wii_menu_root = wii_create_tree_node(NODETYPE_ROOT, "root");

    TREENODE* child = NULL;
    child = wii_create_tree_node(NODETYPE_RESUME, "Resume");
    wii_add_child(wii_menu_root, child);

    child = NULL;
    child = wii_create_tree_node(NODETYPE_RESET, "Reset");
    wii_add_child(wii_menu_root, child);

    child = wii_create_tree_node(NODETYPE_LOAD_ROM, "Load cartridge");
    roms_menu = child;
    wii_add_child(wii_menu_root, child);

    child =
        wii_create_tree_node(NODETYPE_CARTRIDGE_SETTINGS_SPACER, "");
    wii_add_child(wii_menu_root, child);

    //
    // Save state management
    //

    TREENODE* states =
        wii_create_tree_node(NODETYPE_CARTRIDGE_SAVE_STATES, "Save states");
    wii_add_child(wii_menu_root, states);

    child = wii_create_tree_node(NODETYPE_CARTRIDGE_SAVE_STATES_SLOT, "Slot");
    wii_add_child(states, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(states, child);

    child = wii_create_tree_node(NODETYPE_SAVE_STATE, "Save state");
    wii_add_child(states, child);

    child = wii_create_tree_node(NODETYPE_LOAD_STATE, "Load state");
    wii_add_child(states, child);

    child = wii_create_tree_node(NODETYPE_DELETE_STATE, "Delete state");
    wii_add_child(states, child);    

    //
    // Cartridge settings
    //

    TREENODE* cart_settings =
        wii_create_tree_node(NODETYPE_CARTRIDGE_SETTINGS, "Cartridge-specific settings");
    wii_add_child(wii_menu_root, cart_settings);

    // Add items to the cart settings menu
    wii_atari_db_create_menu(cart_settings);

    //
    // The advanced menu
    //

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(wii_menu_root, child);

    TREENODE* advanced = wii_create_tree_node(NODETYPE_ADVANCED, "Advanced");
    wii_add_child(wii_menu_root, advanced);

    //
    // The display settings menu
    //

    TREENODE* display =
        wii_create_tree_node(NODETYPE_DISPLAY_SETTINGS, "Video settings");
    wii_add_child(advanced, display);

    child = wii_create_tree_node(NODETYPE_RESIZE_SCREEN, "Screen size");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_VSYNC, "Vertical sync");
    wii_add_child(display, child);

    child =
        wii_create_tree_node(NODETYPE_MAX_FRAME_RATE, "Maximum frame rate");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_FULL_WIDESCREEN, "Full widescreen");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_16_9_CORRECTION, "16:9 correction");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_TRAP_FILTER, "Color trap filter");
    wii_add_child(display, child);

    child =
        wii_create_tree_node(NODETYPE_DOUBLE_STRIKE, "Double strike (240p)");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_GX_VI_SCALER, "Scaler");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_FILTER, "Bilinear filter");
    wii_add_child(display, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(display, child);

    //
    // The controls settings menu
    //

    TREENODE* controls =
        wii_create_tree_node(NODETYPE_CONTROLS_SETTINGS, "Control settings");
    wii_add_child(advanced, controls);

    child =
        wii_create_tree_node(NODETYPE_DIFF_SWITCH_ENABLED, "Diff. switches");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_DIFF_SWITCH_DISPLAY,
                                 "Diff. switches display");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_LIGHTGUN_CROSSHAIR,
                                 "Lightgun crosshair");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_LIGHTGUN_FLASH, "Lightgun flash");
    wii_add_child(controls, child);

    //
    // The cartridge settings menu
    //

    TREENODE* cartridge =
        wii_create_tree_node(NODETYPE_ADVANCED_CART_SETTINGS, "Cartridge settings");
    wii_add_child(advanced, cartridge);

    child = wii_create_tree_node(NODETYPE_HIGH_SCORE_MODE, "High score cart.");
    wii_add_child(cartridge, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(cartridge, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(advanced, child);

    child = wii_create_tree_node(NODETYPE_TOP_MENU_EXIT, "Top menu exit");
    wii_add_child(advanced, child);

    child =
        wii_create_tree_node(NODETYPE_WIIMOTE_MENU_ORIENT, "Wiimote (menu)");
    wii_add_child(advanced, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(advanced, child);

    child = wii_create_tree_node(NODETYPE_DEBUG_MODE, "Debug mode");
    wii_add_child(advanced, child);

    wii_menu_push(wii_menu_root);
}

/**
 * Updates the buffer with the header message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the header message for the current
 *          menu.
 */
void wii_menu_handle_get_header(TREENODE* menu, char* buffer) {
    if (loading_game) {
        snprintf(buffer, WII_MENU_BUFF_SIZE, gettextmsg("Loading game..."));
    } else {
        switch (menu->node_type) {
            case NODETYPE_LOAD_ROM:
                if (!games_read) {
                    snprintf(buffer, WII_MENU_BUFF_SIZE,
                             mount_pending
                                 ? gettextmsg("Attempting to mount drive...")
                                 : gettextmsg("Reading game list..."));
                }
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

/**
 * Updates the buffer with the footer message for the current menu
 *
 * @param   menu The menu
 * @param   buffer The buffer to update with the footer message for the
 *          current menu.
 */
void wii_menu_handle_get_footer(TREENODE* menu, char* buffer) {
    if (loading_game) {
        snprintf(buffer, WII_MENU_BUFF_SIZE, " ");
    } else {
        switch (menu->node_type) {
            case NODETYPE_LOAD_ROM:
                if (games_read) {
                    wii_get_list_footer(roms_menu, "item", "items", buffer);
                }
                break;
            default:
                break;
        }
    }
}

/**
 * Updates the buffer with the name of the specified node
 *
 * @param   node The node
 * @param   buffer The name of the specified node
 * @param   value The value of the specified node
 */
void wii_menu_handle_get_node_name(TREENODE* node, char* buffer, char* value) {
    const char* strmode = NULL;
    snprintf(buffer, WII_MENU_BUFF_SIZE, "%s", node->name);

    switch (node->node_type) {
        case NODETYPE_ROOT_DRIVE: {
            int device;
            FindDevice(node->name, &device);
            switch (device) {
                case DEVICE_SD:
                    snprintf(buffer, WII_MENU_BUFF_SIZE, "[%s]", "SD Card");
                    break;
                case DEVICE_USB:
                    snprintf(buffer, WII_MENU_BUFF_SIZE, "[%s]", "USB Device");
                    break;
                case DEVICE_SMB:
                    snprintf(buffer, WII_MENU_BUFF_SIZE, "[%s]",
                             "Network Share");
                    break;
            }
        } break;
        case NODETYPE_DIR:
            snprintf(buffer, WII_MENU_BUFF_SIZE, "[%s]", node->name);
            break;
        case NODETYPE_CARTRIDGE_SAVE_STATES_SLOT: {
            BOOL isLatest;
            int current = wii_snapshot_current_index(&isLatest);
            current++;
            if (!isLatest) {
                snprintf(value, WII_MENU_BUFF_SIZE, "%d", current);
            } else {
                snprintf(value, WII_MENU_BUFF_SIZE, "%d (%s)", current,
                         gettextmsg("Latest"));
            }
        } break;
        case NODETYPE_RESIZE_SCREEN: {
            const screen_size* sizes = NULL;
            int size_count = 0;
            wii_get_default_screen_sizes(&sizes, &size_count);
            resize_info rinfo = {sizes, size_count, (float)wii_screen_x,
                                 (float)wii_screen_y};
            int idx =
                wii_resize_screen_find_size(&rinfo, wii_screen_x, wii_screen_y);
            snprintf(value, WII_MENU_BUFF_SIZE, "%s",
                     (idx == -1 ? "Custom" : sizes[idx].name));
        } break;
        case NODETYPE_GX_VI_SCALER:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s",
                     (wii_gx_vi_scaler ? "GX + VI" : "GX"));
            break;
        case NODETYPE_16_9_CORRECTION:
        case NODETYPE_FULL_WIDESCREEN: {
            int val = node->node_type == NODETYPE_16_9_CORRECTION
                          ? wii_16_9_correction
                          : wii_full_widescreen;
            snprintf(
                value, WII_MENU_BUFF_SIZE, "%s",
                (val == WS_AUTO ? "(auto)" : (val ? "Enabled" : "Disabled")));
        } break;
        case NODETYPE_DIFF_SWITCH_DISPLAY:
            switch (wii_diff_switch_display) {
                case DIFF_SWITCH_DISPLAY_DISABLED:
                    strmode = "Disabled";
                    break;
                case DIFF_SWITCH_DISPLAY_WHEN_CHANGED:
                    strmode = "When changed (default)";
                    break;
                case DIFF_SWITCH_DISPLAY_ALWAYS:
                    strmode = "Always";
                    break;
                default:
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_WIIMOTE_MENU_ORIENT:
            if (wii_mote_menu_vertical) {
                strmode = "Upright";
            } else {
                strmode = "Sideways";
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_VSYNC:
            switch (wii_vsync) {
                case VSYNC_DISABLED:
                    strmode = "Disabled";
                    break;
                case VSYNC_ENABLED:
                    strmode = "Enabled";
                    break;
                default:
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_HIGH_SCORE_MODE:
            switch (wii_hs_mode) {
                case HSMODE_ENABLED_NORMAL:
                    strmode = "Enabled (excludes saved state)";
                    break;
                case HSMODE_ENABLED_SNAPSHOTS:
                    strmode = "Enabled (includes saved state)";
                    break;
                case HSMODE_DISABLED:
                    strmode = "Disabled";
                    break;
                default:
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_MAX_FRAME_RATE:
            if (wii_max_frame_rate == 0) {
                snprintf(value, WII_MENU_BUFF_SIZE, "(auto)");
            } else {
                snprintf(value, WII_MENU_BUFF_SIZE, "%d", wii_max_frame_rate);
            }
            break;
        case NODETYPE_DEBUG_MODE:
        case NODETYPE_TOP_MENU_EXIT:
        case NODETYPE_DIFF_SWITCH_ENABLED:
        case NODETYPE_LIGHTGUN_CROSSHAIR:
        case NODETYPE_LIGHTGUN_FLASH: 
        case NODETYPE_DOUBLE_STRIKE:
        case NODETYPE_TRAP_FILTER:        
        case NODETYPE_FILTER: {
            BOOL enabled = FALSE;
            switch (node->node_type) {
                case NODETYPE_FILTER:
                    enabled = wii_filter;
                    break;
                case NODETYPE_TRAP_FILTER:                    
                    enabled = wii_trap_filter;
                    break;                
                case NODETYPE_DOUBLE_STRIKE:
                    enabled = wii_double_strike_mode;
                    break;
                case NODETYPE_DEBUG_MODE:
                    enabled = wii_debug;
                    break;
                case NODETYPE_TOP_MENU_EXIT:
                    enabled = wii_top_menu_exit;
                    break;
                case NODETYPE_DIFF_SWITCH_ENABLED:
                    enabled = wii_diff_switch_enabled;
                    break;
                case NODETYPE_LIGHTGUN_CROSSHAIR:
                    enabled = wii_lightgun_crosshair;
                    break;
                case NODETYPE_LIGHTGUN_FLASH:
                    enabled = wii_lightgun_flash;
                    break;
                default:
                    break;
            }

            snprintf(value, WII_MENU_BUFF_SIZE, "%s",
                     enabled ? "Enabled" : "Disabled");
            break;
        }
        default:
            break;
    }

    wii_atari_db_get_node_name(node, buffer, value);
}

/**
 * React to the "select" event for the specified node
 *
 * @param   node The node that was selected
 */
void wii_menu_handle_select_node(TREENODE* node) {
    if (node->node_type == NODETYPE_ROM || 
        node->node_type == NODETYPE_RESUME ||
        node->node_type == NODETYPE_LOAD_STATE ||
        node->node_type == NODETYPE_RESET) {
        char buff[WII_MAX_PATH];

        wii_gx_push_callback( NULL, FALSE, NULL ); // Blank screen   
        VIDEO_WaitVSync();

        switch (node->node_type) {
            case NODETYPE_ROM:                
                snprintf(buff, sizeof(buff), "%s%s",
                         wii_get_roms_dir(), node->name);
                // Default the cartridge title
                cartridge_title = node->name;
                last_rom_index = wii_menu_get_current_index();
                loading_game = TRUE;
                wii_start_emulation(buff);
                loading_game = FALSE;
                break;
            case NODETYPE_RESUME:
                wii_resume_emulation();
                break;
            case NODETYPE_RESET:
                wii_reset_emulation();
                break;
            case NODETYPE_LOAD_STATE:
                loading_game = TRUE;
                if (!wii_start_snapshot()) {
                    // Exit the save states (rom is no longer valid)
                    wii_menu_pop();
                }
                loading_game = FALSE;
                break;
        }

        wii_gx_pop_callback();
        VIDEO_WaitVSync();

    } else {
        LOCK_RENDER_MUTEX();

        // Allow DB to select node
        wii_atari_db_select_node(node);

        switch (node->node_type) {
            case NODETYPE_ROOT_DRIVE:
            case NODETYPE_UPDIR:
            case NODETYPE_DIR:
                if (node->node_type == NODETYPE_ROOT_DRIVE) {
                    char path[WII_MAX_PATH];
                    snprintf(path, sizeof(path), "%s/", node->name);
                    wii_set_roms_dir(path);
                    mount_pending = TRUE;
                } else if (node->node_type == NODETYPE_UPDIR) {
                    const char* romsDir = wii_get_roms_dir();
                    int len = strlen(romsDir);
                    if (len > 1 && romsDir[len - 1] == '/') {
                        char dirpart[WII_MAX_PATH] = "";
                        char filepart[WII_MAX_PATH] = "";
                        Util_splitpath(romsDir, dirpart, filepart);
                        len = strlen(dirpart);
                        if (len > 0) {
                            dirpart[len] = '/';
                            dirpart[len + 1] = '\0';
                        }
                        wii_set_roms_dir(dirpart);
                    }
                } else {
                    char newDir[WII_MAX_PATH];
                    snprintf(newDir, sizeof(newDir), "%s%s/",
                             wii_get_roms_dir(), node->name);
                    wii_set_roms_dir(newDir);
                }
                games_read = FALSE;
                last_rom_index = 1;
                break;
            case NODETYPE_RESIZE_SCREEN: {
                int blity =
                    (cartridge_region == REGION_NTSC ? NTSC_ATARI_BLIT_TOP_Y
                                                     : PAL_ATARI_BLIT_TOP_Y);
                int height =
                    (cartridge_region == REGION_NTSC ? NTSC_ATARI_HEIGHT
                                                     : PAL_ATARI_HEIGHT);
                wii_resize_screen_draw_border(blit_surface, blity, height);
                wii_atari_put_image_gu_normal();
                wii_sdl_flip();

                const screen_size* sizes = NULL;
                int size_count = 0;
                wii_get_default_screen_sizes(&sizes, &size_count);
                resize_info rinfo = {sizes, size_count, (float)wii_screen_x,
                                     (float)wii_screen_y};
                wii_resize_screen_gui(&rinfo);
                wii_screen_x = rinfo.currentX;
                wii_screen_y = rinfo.currentY;
            } break;
            case NODETYPE_FULL_WIDESCREEN:
                wii_full_widescreen++;
                if (wii_full_widescreen > WS_AUTO) {
                    wii_full_widescreen = 0;
                }
                break;
            case NODETYPE_16_9_CORRECTION:
                wii_16_9_correction++;
                if (wii_16_9_correction > WS_AUTO) {
                    wii_16_9_correction = 0;
                }
                break;
            case NODETYPE_DIFF_SWITCH_DISPLAY:
                wii_diff_switch_display++;
                if (wii_diff_switch_display > 2) {
                    wii_diff_switch_display = 0;
                }
                break;
            case NODETYPE_HIGH_SCORE_MODE:
                wii_hs_mode++;
                if (wii_hs_mode > 2) {
                    wii_hs_mode = 0;
                }
                break;
            case NODETYPE_VSYNC:
                wii_set_vsync(wii_vsync ^ 1);
                break;
            case NODETYPE_LIGHTGUN_CROSSHAIR:
                wii_lightgun_crosshair ^= 1;
                break;
            case NODETYPE_LIGHTGUN_FLASH:
                wii_lightgun_flash ^= 1;
                break;
            case NODETYPE_MAX_FRAME_RATE:
                wii_max_frame_rate += 1;
                if (wii_max_frame_rate > 70) {
                    wii_max_frame_rate = 0;
                } else if (wii_max_frame_rate == 1) {
                    wii_max_frame_rate = 30;
                }
                break;
            case NODETYPE_DOUBLE_STRIKE:
                wii_double_strike_mode ^= 1;
                break;
            case NODETYPE_FILTER:
                wii_filter ^= 1;
                break;
            case NODETYPE_GX_VI_SCALER:
                wii_gx_vi_scaler ^= 1;
                break;
            case NODETYPE_TRAP_FILTER:
                wii_trap_filter ^= 1;
                break;
            case NODETYPE_TOP_MENU_EXIT:
                wii_top_menu_exit ^= 1;
                break;
            case NODETYPE_DEBUG_MODE:
                wii_debug ^= 1;
                break;
            case NODETYPE_WIIMOTE_MENU_ORIENT:
                wii_mote_menu_vertical ^= 1;
                break;
            case NODETYPE_DIFF_SWITCH_ENABLED:
                wii_diff_switch_enabled ^= 1;
                break;
            case NODETYPE_CARTRIDGE_SETTINGS:                
                wii_atari_db_check_exists();
                wii_menu_push(node);
                break;
            case NODETYPE_ADVANCED:
            case NODETYPE_LOAD_ROM:            
            case NODETYPE_DISPLAY_SETTINGS:
            case NODETYPE_CONTROLS_SETTINGS:
                wii_menu_push(node);
                if (node->node_type == NODETYPE_LOAD_ROM) {
                    wii_menu_move(node, last_rom_index);
                }
                break;
            case NODETYPE_SAVE_STATE:
                wii_save_snapshot(NULL, TRUE);
                break;
            case NODETYPE_DELETE_STATE:
                wii_delete_snapshot();
                wii_snapshot_refresh();
                break;
            case NODETYPE_CARTRIDGE_SAVE_STATES_SLOT:
                wii_snapshot_next();
                break;
            case NODETYPE_CARTRIDGE_SAVE_STATES:
                wii_menu_push(node);
                if (node->node_type == NODETYPE_LOAD_ROM) {
                    if (games_read) {
                        wii_menu_move(node, last_rom_index);
                    }
                }
                break;
            case NODETYPE_ADVANCED_CART_SETTINGS:
                wii_menu_push(node);
                break;
            default:
                break;
        }

        UNLOCK_RENDER_MUTEX();
    }
}

/**
 * Determines whether the node is currently visible
 *
 * @param   node The node
 * @return  Whether the node is visible
 */
BOOL wii_menu_handle_is_node_visible(TREENODE* node) {
    switch (node->node_type) {
        case NODETYPE_DELETE_STATE:
        case NODETYPE_LOAD_STATE:
            return wii_snapshot_current_exists();
        case NODETYPE_GX_VI_SCALER:
            return !wii_double_strike_mode;
        case NODETYPE_FILTER:
            return !wii_gx_vi_scaler && !wii_double_strike_mode;
        case NODETYPE_RESET:
        case NODETYPE_RESUME:
        case NODETYPE_CARTRIDGE_SETTINGS_SPACER:
        case NODETYPE_CARTRIDGE_SETTINGS:
        case NODETYPE_CARTRIDGE_SAVE_STATES:
            return wii_last_rom != NULL;
            break;
        default:
            break;
    }

    if (!wii_atari_db_is_node_visible(node)) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Determines whether the node is selectable
 *
 * @param   node The node
 * @return  Whether the node is selectable
 */
BOOL wii_menu_handle_is_node_selectable(TREENODE* node) {
    if (node->node_type == NODETYPE_CARTRIDGE_SETTINGS_SPACER) {
        return FALSE;
    }

    if (!wii_atari_db_is_node_selectable(node)) {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * Provides an opportunity for the specified menu to do something during
 * a display cycle.
 *
 * @param   menu The menu
 */
void wii_menu_handle_update(TREENODE* menu) {
    switch (menu->node_type) {
        case NODETYPE_LOAD_ROM:
            if (!games_read) {
                LOCK_RENDER_MUTEX();

                if (mount_pending) {
                    const char* roms = wii_get_roms_dir();
                    if (strlen(roms) > 0) {
                        char mount[WII_MAX_PATH];
                        Util_strlcpy(mount, roms, sizeof(mount));

                        resetSmbErrorMessage();  // Reset the SMB error message
                        if (!ChangeInterface(mount, FS_RETRY_COUNT)) {
                            wii_set_roms_dir("");
                            const char* netMsg = getSmbErrorMessage();
                            if (netMsg != NULL) {
                                wii_set_status_message(netMsg);
                            } else {
                                char msg[256];
                                snprintf(msg, sizeof(msg), "%s: %s",
                                         "Unable to mount", mount);
                                wii_set_status_message(msg);
                            }
                        }
                    }
                    mount_pending = FALSE;
                }
                wii_read_game_list(roms_menu);
                wii_menu_reset_indexes();
                wii_menu_move(roms_menu, 1);

                UNLOCK_RENDER_MUTEX();
            }
            break;
        default:
            /* do nothing */
            break;
    }
}

/**
 * Used for comparing menu names when sorting (qsort)
 *
 * @param   a The first tree node to compare
 * @param   b The second tree node to compare
 * @return  The result of the comparison
 */
static int game_name_compare(const void* a, const void* b) {
    TREENODE** aptr = (TREENODE**)a;
    TREENODE** bptr = (TREENODE**)b;
    int type = (*aptr)->node_type - (*bptr)->node_type;
    return type != 0 ? type : strcasecmp((*aptr)->name, (*bptr)->name);
}

/**
 * Reads the list of games into the specified menu
 *
 * @param   menu The menu to read the games into
 */
static void wii_read_game_list(TREENODE* menu) {
    const char* roms = wii_get_roms_dir();

    wii_menu_clear_children(menu);  // Clear the children

#ifdef WII_NETTRACE
    net_print_string(NULL, 0, "ReadGameList: %s\n", roms, strlen(roms));
#endif

    BOOL success = FALSE;
    if (strlen(roms) > 0) {
        DIR* romdir = opendir(roms);

#ifdef WII_NETTRACE
        net_print_string(NULL, 0, "OpenDir: %d\n", roms, romdir);
#endif

        if (romdir != NULL) {
            wii_add_child(menu, wii_create_tree_node(NODETYPE_UPDIR, "[..]"));

            struct dirent* entry = NULL;
            while ((entry = readdir(romdir)) != NULL) {
                if ((strcmp(".", entry->d_name) &&
                     strcmp("..", entry->d_name))) {
                    TREENODE* child = wii_create_tree_node(
                        (entry->d_type == DT_DIR ? NODETYPE_DIR : NODETYPE_ROM),
                        entry->d_name);

                    wii_add_child(menu, child);
                }
            }
            closedir(romdir);

            // Sort the games list
            qsort(menu->children, menu->child_count, sizeof(*(menu->children)),
                  game_name_compare);

            success = TRUE;
        } else {
            char msg[256];
            snprintf(msg, sizeof(msg), "%s: %s", "Error opening", roms);
            wii_set_status_message(msg);
        }
    }

    if (!success) {
        wii_set_roms_dir("");
        wii_add_child(menu, wii_create_tree_node(NODETYPE_ROOT_DRIVE, "sd:"));
        wii_add_child(menu, wii_create_tree_node(NODETYPE_ROOT_DRIVE, "usb:"));
        wii_add_child(menu, wii_create_tree_node(NODETYPE_ROOT_DRIVE, "smb:"));
    }

    games_read = TRUE;
}

/**
 * Invoked after exiting the menu loop
 */
void wii_menu_handle_post_loop() {}

/**
 * Invoked prior to entering the menu loop
 */
void wii_menu_handle_pre_loop() {}

/**
 * Invoked when the home button is pressed when the menu is being displayed
 */
void wii_menu_handle_home_button() {}