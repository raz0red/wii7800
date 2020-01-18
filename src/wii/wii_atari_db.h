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

#ifndef WII_ATARI_DB_H
#define WII_ATARI_DB_H

#include "wii_main.h"

/**
 * This method is invoked after a cartridge has been loaded. It provides an
 * opportunity for the current cartridge settings to be captured, so they can be
 * viewed and modified later.
 */
void wii_atari_db_after_load();


/**
 * Creates the menu entries for the cartridge settings menu
 */
void wii_atari_db_create_menu(TREENODE* cart_settings_menu);

/**
 * Updates the buffer with the name of the specified node
 *
 * @param   node The node
 * @param   buffer The name of the specified node
 * @param   value The value of the specified node
 */
void wii_atari_db_get_node_name(TREENODE* node, char* buffer, char* value);

/**
 * React to the "select" event for the specified node
 *
 * @param   node The node that was selected
 */
void wii_atari_db_select_node(TREENODE* node);

/**
 * Determines whether the node is currently visible
 *
 * @param   node The node
 * @return  Whether the node is visible
 */
BOOL wii_atari_db_is_node_visible(TREENODE* node);

/**
 * Determines whether the node is selectable
 *
 * @param   node The node
 * @return  Whether the node is selectable
 */
BOOL wii_atari_db_is_node_selectable(TREENODE* node);

/**
 * Checks to see if an entry exists in the database for the current game
 * 
 * @return  Whether an entry exists in the database for the current game
 */
bool wii_atari_db_check_exists();

#endif