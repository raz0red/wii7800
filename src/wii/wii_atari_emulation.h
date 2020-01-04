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

#ifndef WII_ATARI_EMULATION_H
#define WII_ATARI_EMULATION_H

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
                         const char* savefile = NULL,
                         BOOL reset = false,
                         BOOL resume = false);

/**
 * Resumes emulation of the current game
 */
void wii_resume_emulation();

/**
 * Resets the current game
 */
void wii_reset_emulation();

#endif
