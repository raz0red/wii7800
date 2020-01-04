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

#ifndef WII_ATARI_SNAPSHOT_H
#define WII_ATARI_SNAPSHOT_H

#include <gccore.h>

/**
 * Starts emulation with the current snapshot
 *
 * @return  Whether emulation was successfully started
 */
BOOL wii_start_snapshot();

/**
 * Resets snapshot related information. This method is typically invoked when
 * a new rom file is loaded.
 *
 * @param   setIndexToLatest Whether to set the snapshot index to the latest for
 *          the current rom
 */
void wii_snapshot_reset(BOOL setIndexToLatest = FALSE);

/**
 * Refreshes state of snapshot (does it exist, etc.)
 */
void wii_snapshot_refresh();

/**
 * Returns the index of the current snapshot.
 *
 * @param   isLatest (out) Whether the current snapshot index is the latest
 *              snapshot (most recent)
 * @return  The index of the current snapshot
 */
int wii_snapshot_current_index(BOOL* isLatest);

/**
 * Returns whether the current snapshot exists
 *
 * @return  Whether the current snapshot exists
 */
BOOL wii_snapshot_current_exists();

/**
 * Moves to the next snapshot (next index)
 *
 * @return  The index that was moved to
 */
int wii_snapshot_next();

#endif
