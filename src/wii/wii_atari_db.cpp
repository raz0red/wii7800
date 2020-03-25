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
#include <string.h>

#include "Cartridge.h"
#include "Region.h"

#include "wii_app_common.h"
#include "wii_app.h"
#include "wii_main.h"
#include "wii_atari_db.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

#define DB_FILE_PATH WII_PROSYSTEM_DB
#define DB_TMP_FILE_PATH WII_FILES_DIR "ProSystem.dat.tmp"
#define DB_OLD_FILE_PATH WII_FILES_DIR "ProSystem.dat.old"

/**
 * Cartridge settings.
 *
 * For these settings to be applied, the settings must be saved, and the
 * cartridge must be re-loaded.
 */
typedef struct CartSettings {
    /** The type of cartridge */
    byte type;
    /** Whether pokey is enabled */
    bool pokey;
    /** Whether pokey at $450 is enabled */
    bool pokey450;
    /** The region of the cartridge */
    byte region;
    /** Flags for the cartridge */
    uint flags;
    /** Whether the expansion module is enabled */
    bool xm;
    /** Whether to disable the bios loadig */
    bool disable_bios;
    /** The hblank */
    int hblank;
    /** High score cartridge */
    bool hsc_enabled;
} CartSettings;

extern unsigned char keyboard_data[19];

/** The database file */
static char db_file[WII_MAX_PATH] = "";
/** The database temp file */
static char db_tmp_file[WII_MAX_PATH] = "";
/** The database old file */
static char db_old_file[WII_MAX_PATH] = "";
/** The cartridge settings */
static CartSettings cart_settings = {0};
/** Whether the cart exists in the DB */
static bool cart_exists_in_db = false;

/**
 * This method is invoked after a cartridge has been loaded. It provides an
 * opportunity for the current cartridge settings to be captured, so they can be
 * viewed and modified later.
 */
void wii_atari_db_after_load() 
{
    cart_settings.type = cartridge_type;
    cart_settings.pokey = cartridge_pokey;
    cart_settings.pokey450 = cartridge_pokey450;
    cart_settings.region = cartridge_region;
    cart_settings.flags = cartridge_flags;
    cart_settings.xm = cartridge_xm;
    cart_settings.disable_bios = cartridge_disable_bios;
    cart_settings.hblank = cartridge_hblank;
    cart_settings.hsc_enabled = cartridge_hsc_enabled;
    cart_exists_in_db = false;
}

/**
 * Returns the path to the database file
 *
 * @return  The path to the database file
 */
static char* get_db_path() {
    if (db_file[0] == '\0') {
        snprintf(db_file, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 DB_FILE_PATH);
    }
    return db_file;
}

/**
 * Returns the path to the database temporary file
 *
 * @return  The path to the database temporary file
 */
static char* get_db_tmp_path() {
    if (db_tmp_file[0] == '\0') {
        snprintf(db_tmp_file, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 DB_TMP_FILE_PATH);
    }
    return db_tmp_file;
}

/**
 * Returns the path to the database old file
 *
 * @return  The path to the database old file
 */
static char* get_db_old_path() {
    if (db_old_file[0] == '\0') {
        snprintf(db_old_file, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 DB_OLD_FILE_PATH);
    }
    return db_old_file;
}

/**
 * Writes the database entry to the specified file
 *
 * @param   file The file to write the entry to
 * @param   hash The hash for the entry
 */
static void write_entry(FILE* file, const char* hash) {
#ifdef WII_NETTRACE
    net_print_string(NULL, 0, "writing db entry with hash: [%s]\n", hash);
#endif
    bool lightgun =
        cartridge_controller[0] == CARTRIDGE_CONTROLLER_LIGHTGUN ||
        cartridge_controller[1] == CARTRIDGE_CONTROLLER_LIGHTGUN;
    bool bothJoystick =
        cartridge_controller[0] == CARTRIDGE_CONTROLLER_JOYSTICK &&
        cartridge_controller[1] == CARTRIDGE_CONTROLLER_JOYSTICK;
    fprintf(file, "[%s]\n", hash);
    fprintf(file, "title=%s\n", cartridge_title.c_str());
    fprintf(file, "type=%d\n", cart_settings.type);
    fprintf(file, "pokey=%s\n", cart_settings.pokey ? "true" : "false");
    fprintf(file, "controller1=%d\n", cartridge_controller[0]);
    fprintf(file, "controller2=%d\n", cartridge_controller[1]);
    fprintf(file, "region=%d\n", cart_settings.region);
    fprintf(file, "flags=%d\n", cart_settings.flags);
    fprintf(file, "pokey450=%s\n", cart_settings.pokey450 ? "true" : "false");
    fprintf(file, "xm=%s\n", cart_settings.xm ? "true" : "false");
    fprintf(file, "hsc=%s\n", cart_settings.hsc_enabled ? "true" : "false");    
    if (lightgun) {
        if (cartridge_crosshair_x) {
            fprintf(file, "crossx=%d\n", cartridge_crosshair_x);
        }
        if (cartridge_crosshair_y) {
            fprintf(file, "crossy=%d\n", cartridge_crosshair_y);
        }
    }
#ifdef ENABLE_BIOS_SUPPORT    
    if (cart_settings.disable_bios) {
        fprintf(file, "disablebios=%s\n", cart_settings.disable_bios ? "true" : "false");
    }    
#endif    
    /*byte cartridge_left_switch = 1;*/ // Default
    if (cartridge_left_switch != 1) {
        fprintf(file, "leftswitch=%d\n", cartridge_left_switch);
    }
    /*byte cartridge_right_switch = 0;*/ // Default 
    if (cartridge_right_switch != 0) {
        fprintf(file, "rightswitch=%d\n", cartridge_right_switch);    
    }
    if (cartridge_swap_buttons) {
        fprintf(file, "swapbuttons=%s\n", cartridge_swap_buttons ? "true" : "false");
    }
    if (bothJoystick && cartridge_dualanalog) {
        fprintf(file, "dualanalog=%s\n", cartridge_dualanalog ? "true" : "false");
    }
    if (cart_settings.hblank != HBLANK_DEFAULT) {
        fprintf(file, "hblank=%d\n", cart_settings.hblank);
    }       
}

/**
 * Attempts to locate a hash in the specified source string. If it
 * is found, it is copied into dest.
 *
 * @param   source The source string
 * @param   dest The destination string
 * @return  non-zero if the hash was found and copied
 */
static int get_hash(char* source, char* dest) {
    int db_hash_len = 0;  // Length of the hash
    char* end_idx;        // End index of the hash
    char* start_idx;      // Start index of the hash

    start_idx = source;
    if (*start_idx == '[') {
        ++start_idx;
        end_idx = strrchr(start_idx, ']');
        if (end_idx != 0) {
            db_hash_len = end_idx - start_idx;
            strncpy(dest, start_idx, db_hash_len);
            dest[db_hash_len] = '\0';
            return 1;
        }
    }

    return 0;
}

/**
 * Attempts to find an entry for the specified hash
 *
 * @param   hash The hash of the game
 * @return  Whether the entry was found
 */
static bool db_find_entry(const char* hash) {
    char buff[255];      // The buffer to use when reading the file
    char db_hash[255];   // A hash found in the file we are reading from
    // The database file
    FILE* db_file = fopen(get_db_path(), "r");

    // A database file doesn't exist
    if (!db_file) {
        return false;
    }

    bool found = false;
    while (!found && fgets(buff, sizeof(buff), db_file) != 0) {
        // Check if we found a hash
        if (get_hash(buff, db_hash)) {
            if (!strcmp(hash, db_hash)) {
                found = true;
            }
        }
    }
    fclose(db_file);
    return found;
}


/**
 * Writes the settings for the current game
 *
 * @param   hash The hash of the game
 * @param   delete Whether to delete the entry
 * @return  Whether the write was successful
 */
static int db_write_entry(const char* hash, bool del) {
    char buff[255];      // The buffer to use when reading the file
    char db_hash[255];   // A hash found in the file we are reading from
    int copy_mode = 0;   // The current copy mode
    FILE* tmp_file = 0;  // The temp file
    FILE* old_file = 0;  // The old file

    // The database file
    FILE* db_file = fopen(get_db_path(), "r");

    // A database file doesn't exist, create a new one
    if (!db_file && !del) {
        db_file = fopen(get_db_path(), "w");
        if (!db_file) {
            // Unable to create DB file
            return 0;
        }

        // Write the entry
        write_entry(db_file, hash);

        fclose(db_file);
    } else {
        //
        // A database exists, search for the appropriate hash while copying
        // its current contents to a temp file
        //

        // Open up the temp file
        tmp_file = fopen(get_db_tmp_path(), "w");
        if (!tmp_file) {
            fclose(db_file);

            // Unable to create temp file
            return 0;
        }

        //
        // Loop and copy
        //

        while (fgets(buff, sizeof(buff), db_file) != 0) {
            // Check if we found a hash
            if (copy_mode < 2 && get_hash(buff, db_hash)) {
                if (copy_mode) {
                    copy_mode++;
                } else if (!strcmp(hash, db_hash)) {
                    // We have matching hashes, write out the new entry
                    if (!del) {
                        write_entry(tmp_file, hash);
                    }

                    copy_mode++;
                }
            }

            if (copy_mode != 1) {
                fprintf(tmp_file, "%s", buff);
            }
        }

        if (!copy_mode) {
            // We didn't find the hash in the database, add it          
            if (!del) {  
                write_entry(tmp_file, hash);
            }
        }

        fclose(db_file);
        fclose(tmp_file);

        //
        // Make sure the temporary file exists
        // We do this due to the instability of the Wii SD card
        //
        tmp_file = fopen(get_db_tmp_path(), "r");
        if (!tmp_file) {
            // Unable to find temp file
            return 0;
        }
        fclose(tmp_file);

        // Delete old file (if it exists)
        if ((old_file = fopen(get_db_old_path(), "r")) != 0) {
            fclose(old_file);
            if (remove(get_db_old_path()) != 0) {
                return 0;
            }
        }

        // Rename database file to old file
        if (rename(get_db_path(), get_db_old_path()) != 0) {
            return 0;
        }

        // Rename temp file to database file
        if (rename(get_db_tmp_path(), get_db_path()) != 0) {
            return 0;
        }
    }

    return 1;
}

/**
 * Deletes the entry from the database with the specified hash
 *
 * @param   hash The hash of the game
 * @return  Whether the delete was successful
 */
static int db_delete_entry(const char* hash) {
    return db_write_entry(hash, true);
}

/**
 * Checks to see if an entry exists in the database for the current game
 * 
 * @return  Whether an entry exists in the database for the current game
 */
bool wii_atari_db_check_exists() {
    cart_exists_in_db = db_find_entry(cartridge_digest.c_str());
    return cart_exists_in_db;
}

/**
 * Creates the menu entries for the cartridge settings menu
 */
void wii_atari_db_create_menu(TREENODE* cart_settings_menu) {  

    //
    // Cartridge settings
    //

    TREENODE* cart_settings = wii_create_tree_node(
        NODETYPE_CART_SETTINGS_CART_SETTINGS, "Cartridge settings");
    wii_add_child(cart_settings_menu, cart_settings);      

    TREENODE* child = wii_create_tree_node(NODETYPE_CART_SETTINGS_CART_TYPE, "Cartridge type");
    wii_add_child(cart_settings, child);      

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_REGION, "Region");
    wii_add_child(cart_settings, child);      

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_POKEY, "Pokey");
    wii_add_child(cart_settings, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_HSC, "High score cartridge (HSC)");
    wii_add_child(cart_settings, child);    

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_XM, "Expansion module (XM)");
    wii_add_child(cart_settings, child);

#ifdef ENABLE_BIOS_SUPPORT    
    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_DISABLE_BIOS, "BIOS");
    wii_add_child(cart_settings, child);
#endif    

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_WSYNC, "Handle WSYNC");
    wii_add_child(cart_settings, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_CYCLE_STEALING, "Cycle stealing");
    wii_add_child(cart_settings, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_HBLANK, "HBlank");
    wii_add_child(cart_settings, child);

    //
    // Controls settings
    //

    TREENODE* controls = wii_create_tree_node(
        NODETYPE_CART_SETTINGS_CONTROLS_SETTINGS, "Control settings");
    wii_add_child(cart_settings_menu, controls);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_CONTROLLER1,
                                 "Controller 1");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_CONTROLLER2,
                                 "Controller 2");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_SWAP_BUTTONS,
                                 "Swap buttons");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_CONTROLS_SPACER, "");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_DUAL_ANALOG,
                                 "Dual analog");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_X,
                                 "Lightgun offset X");
    wii_add_child(controls, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_Y,
                                 "Lightgun offset Y");
    wii_add_child(controls, child);

    //
    // Difficulty switch settings
    //

    TREENODE* diff_switch_settings = wii_create_tree_node(
        NODETYPE_CART_SETTINGS_DIFF_SWITCH_SETTINGS, "Difficulty switch settings");
    wii_add_child(cart_settings_menu, diff_switch_settings);      

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_LEFT_SWITCH, "Left switch");
    wii_add_child(diff_switch_settings, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_RIGHT_SWITCH, "Right switch");
    wii_add_child(diff_switch_settings, child);

    //
    // Save/Delete settings
    //

    child = wii_create_tree_node(NODETYPE_SPACER, "");
    wii_add_child(cart_settings_menu, child);

    child =
        wii_create_tree_node(NODETYPE_CART_SETTINGS_SAVE, "Save settings");
    wii_add_child(cart_settings_menu, child);

    child = wii_create_tree_node(NODETYPE_CART_SETTINGS_DELETE,
                                 "Delete settings");
    wii_add_child(cart_settings_menu, child);
}

/**
 * Updates the buffer with the name of the specified node
 *
 * @param   node The node
 * @param   buffer The name of the specified node
 * @param   value The value of the specified node
 */
void wii_atari_db_get_node_name(TREENODE* node, char* buffer, char* value) {
    const char* strmode = "";
    switch (node->node_type) {        
        case NODETYPE_CART_SETTINGS_CART_TYPE:
            switch (cart_settings.type) {
                case CARTRIDGE_TYPE_NORMAL:
                    strmode = "Normal";
                    break;
                case CARTRIDGE_TYPE_SUPERCART:
                    strmode = "SuperGame bank switched";
                    break;
                case CARTRIDGE_TYPE_SUPERCART_LARGE:
                    strmode = "SuperGame ROM at $4000 (bank 0)";
                    break;
                case CARTRIDGE_TYPE_SUPERCART_RAM:
                    strmode = "SuperGame RAM at $4000";
                    break;
                case CARTRIDGE_TYPE_SUPERCART_ROM:
                    strmode = "SuperGame ROM at $4000 (bank 6)";
                    break;
                case CARTRIDGE_TYPE_ABSOLUTE:
                    strmode = "Absolute";
                    break;
                case CARTRIDGE_TYPE_ACTIVISION:
                    strmode = "Activision";
                    break;
                case CARTRIDGE_TYPE_NORMAL_RAM:
                    strmode="Normal RAM at $4000";
                    break;
                default:
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_CART_SETTINGS_REGION:
            switch (cart_settings.region) {
                case REGION_NTSC:
                    strmode = "NTSC";
                    break;
                case REGION_PAL:
                    strmode = "PAL";
                    break;
                default:
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", strmode);
            break;
        case NODETYPE_CART_SETTINGS_WSYNC:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cart_settings.flags & CARTRIDGE_WSYNC_MASK ? "Disabled" : "Enabled");
            break;
        case NODETYPE_CART_SETTINGS_CYCLE_STEALING:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cart_settings.flags & CARTRIDGE_CYCLE_STEALING_MASK ? "Disabled" : "Enabled");
            break;
        case NODETYPE_CART_SETTINGS_XM:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", cart_settings.xm ? "Enabled" : "Disabled");
            break;
        case NODETYPE_CART_SETTINGS_SWAP_BUTTONS:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", cartridge_swap_buttons ? "Enabled" : "Disabled");
            break;
        case NODETYPE_CART_SETTINGS_POKEY:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cart_settings.pokey ? 
                    (cart_settings.pokey450 ? "Pokey at $0450" : "Pokey at $4000") :
                    "Disabled");
            break;
        case NODETYPE_CART_SETTINGS_DISABLE_BIOS:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cart_settings.disable_bios ? "Disabled" : "Enabled");
            break;
        case NODETYPE_CART_SETTINGS_HSC:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cart_settings.hsc_enabled ? "Enabled" : "Disabled");
            break;
        case NODETYPE_CART_SETTINGS_LEFT_SWITCH:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cartridge_left_switch ? "B" : "A");
            break;
        case NODETYPE_CART_SETTINGS_RIGHT_SWITCH:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cartridge_right_switch ? "B" : "A");
            break;
        case NODETYPE_CART_SETTINGS_CONTROLLER1:
        case NODETYPE_CART_SETTINGS_CONTROLLER2: {
            byte controller = cartridge_controller
                [node->node_type == NODETYPE_CART_SETTINGS_CONTROLLER1 ? 0 : 1];
            const char* ctype = "(Unknown)";
            switch (controller) {
                case CARTRIDGE_CONTROLLER_JOYSTICK:
                    ctype = "Joystick";
                    break;
                case CARTRIDGE_CONTROLLER_LIGHTGUN:
                    ctype = "Lightgun";
                    break;
            }
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", ctype);
        } break;
        case NODETYPE_CART_SETTINGS_DUAL_ANALOG:
            snprintf(value, WII_MENU_BUFF_SIZE, "%s", 
                cartridge_dualanalog ? "Enabled" : "Disabled");
            break;
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_X:
            snprintf(value, WII_MENU_BUFF_SIZE, "%d", 
                cartridge_crosshair_x);
            break;
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_Y:
            snprintf(value, WII_MENU_BUFF_SIZE, "%d", 
                cartridge_crosshair_y);
            break;
        case NODETYPE_CART_SETTINGS_HBLANK:
            snprintf(
                value, WII_MENU_BUFF_SIZE, "%d %s", cart_settings.hblank,
                (cart_settings.hblank == HBLANK_DEFAULT ? " (default)" : ""));
            break;
    }
}

/**
 * React to the "select" event for the specified node
 *
 * @param   node The node that was selected
 */
void wii_atari_db_select_node(TREENODE* node) {
    switch (node->node_type) {
        case NODETYPE_CART_SETTINGS_CART_SETTINGS:
        case NODETYPE_CART_SETTINGS_DIFF_SWITCH_SETTINGS:                
        case NODETYPE_CART_SETTINGS_CONTROLS_SETTINGS:
            wii_menu_push(node);
            break;
        case NODETYPE_CART_SETTINGS_CART_TYPE:
            cart_settings.type++;
            if (cart_settings.type > CARTRIDGE_TYPE_NORMAL_RAM) {
                cart_settings.type = 0;
            }
            wii_set_status_message(
                "Type changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_REGION:
            cart_settings.region++;
            if (cart_settings.region > REGION_PAL) {
                cart_settings.region = 0;
            }
            wii_set_status_message(
                "Region changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_WSYNC:
            cart_settings.flags ^= CARTRIDGE_WSYNC_MASK;
            wii_set_status_message(
                "WSYNC changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_CYCLE_STEALING:
            cart_settings.flags ^= CARTRIDGE_CYCLE_STEALING_MASK;
            wii_set_status_message(
                "Cycle stealing changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_POKEY:
            if(cart_settings.pokey) {
                if (!cart_settings.pokey450) {
                    cart_settings.pokey450 = true;
                } else {
                    cart_settings.pokey = false;
                    cart_settings.pokey450 = false;
                }
            } else {
                cart_settings.pokey = true;
            }
            wii_set_status_message(
                "Pokey changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_XM:
            cart_settings.xm ^= 1;
            wii_set_status_message(
                "XM changes not applied until cartridge is reloaded");
            break;            
        case NODETYPE_CART_SETTINGS_DISABLE_BIOS:
            cart_settings.disable_bios ^= 1;
            wii_set_status_message(
                "BIOS changes not applied until cartridge is reloaded");
            break;            
        case NODETYPE_CART_SETTINGS_LEFT_SWITCH:            
            cartridge_left_switch ^= 1;
            keyboard_data[15] = cartridge_left_switch;
            break;
        case NODETYPE_CART_SETTINGS_RIGHT_SWITCH:            
            cartridge_right_switch ^= 1;
            keyboard_data[16] = cartridge_right_switch;
            break;
        case NODETYPE_CART_SETTINGS_SWAP_BUTTONS:
            cartridge_swap_buttons ^= 1;
            break;            
        case NODETYPE_CART_SETTINGS_HSC:            
            cart_settings.hsc_enabled ^= 1;
            wii_set_status_message(
                "High score cart changes not applied until cartridge is reloaded");
            break;            
        case NODETYPE_CART_SETTINGS_CONTROLLER1:
        case NODETYPE_CART_SETTINGS_CONTROLLER2: {
            byte index = 
                node->node_type == NODETYPE_CART_SETTINGS_CONTROLLER1 ? 0 : 1;
            cartridge_controller[index]++;
            if (cartridge_controller[index] > CARTRIDGE_CONTROLLER_LIGHTGUN) {
                cartridge_controller[index] = CARTRIDGE_CONTROLLER_JOYSTICK;
            }

        } break;
        case NODETYPE_CART_SETTINGS_DUAL_ANALOG:
            cartridge_dualanalog ^= 1;
            break;
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_X:
            cartridge_crosshair_x++;
            if (cartridge_crosshair_x > 20) {
                cartridge_crosshair_x = -20;
            }
            break;            
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_Y:            
            cartridge_crosshair_y++;
            if (cartridge_crosshair_y > 20) {
                cartridge_crosshair_y = -20;
            }
            break;
        case NODETYPE_CART_SETTINGS_HBLANK:            
            cart_settings.hblank++;
            if (cart_settings.hblank > 50) {
                cart_settings.hblank = 0;
            }
            wii_set_status_message(
                "HBlank changes not applied until cartridge is reloaded");
            break;
        case NODETYPE_CART_SETTINGS_SAVE:
            if (db_write_entry(cartridge_digest.c_str(), false)) {
                wii_set_status_message(
                    "Successfully saved cartridge settings.");
            } else {
                wii_set_status_message(
                    "An error occurred saving cartridge settings.");
            }
            wii_atari_db_check_exists();
            break;
        case NODETYPE_CART_SETTINGS_DELETE:
            if (db_delete_entry(cartridge_digest.c_str())) {
                wii_menu_reset_indexes();
                wii_menu_move(wii_menu_stack[wii_menu_stack_head], 1);
                wii_set_status_message(
                    "Successfully deleted cartridge settings.");
            } else {
                wii_set_status_message(
                    "An error occurred deleting cartridge settings.");
            }
            wii_atari_db_check_exists();
            break;
    }
}

/**
 * Determines whether the node is currently visible
 *
 * @param   node The node
 * @return  Whether the node is visible
 */
BOOL wii_atari_db_is_node_visible(TREENODE* node) {
   switch (node->node_type) {
        case NODETYPE_CART_SETTINGS_DELETE:    
            return cart_exists_in_db;
        case NODETYPE_CART_SETTINGS_DUAL_ANALOG:
            return cartridge_controller[0] == CARTRIDGE_CONTROLLER_JOYSTICK &&
                   cartridge_controller[1] == CARTRIDGE_CONTROLLER_JOYSTICK;
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_X:
        case NODETYPE_CART_SETTINGS_LIGHTGUN_OFFSET_Y:
            return cartridge_controller[0] == CARTRIDGE_CONTROLLER_LIGHTGUN ||
                   cartridge_controller[1] == CARTRIDGE_CONTROLLER_LIGHTGUN;
        case NODETYPE_CART_SETTINGS_CONTROLS_SPACER:
            return cartridge_controller[0] != CARTRIDGE_CONTROLLER_JOYSTICK ||
                   cartridge_controller[1] != CARTRIDGE_CONTROLLER_JOYSTICK;
   }
   return TRUE;    
}

/**
 * Determines whether the node is selectable
 *
 * @param   node The node
 * @return  Whether the node is selectable
 */
BOOL wii_atari_db_is_node_selectable(TREENODE* node) {
    return node->node_type != NODETYPE_CART_SETTINGS_CONTROLS_SPACER;
}



