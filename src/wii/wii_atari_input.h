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

#ifndef WII_ATARI_INPUT_H
#define WII_ATARI_INPUT_H

#include <wiiuse/wpad.h>

// In game controls
#define WII_BUTTON_ATARI_RESET (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS)
#define GC_BUTTON_ATARI_RESET (PAD_BUTTON_START)

#define WII_BUTTON_ATARI_SELECT (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS)
#define GC_BUTTON_ATARI_SELECT (PAD_TRIGGER_L)

#define WII_BUTTON_ATARI_PAUSE (WPAD_CLASSIC_BUTTON_ZL | WPAD_CLASSIC_BUTTON_ZR)
#define GC_BUTTON_ATARI_PAUSE (PAD_TRIGGER_R)

#define WII_BUTTON_ATARI_RIGHT (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_RIGHT)
#define GC_BUTTON_ATARI_RIGHT (PAD_BUTTON_RIGHT)
#define WII_BUTTON_ATARI_UP (WPAD_BUTTON_RIGHT)
#define GC_BUTTON_ATARI_UP (PAD_BUTTON_UP)
#define WII_CLASSIC_ATARI_UP (WPAD_CLASSIC_BUTTON_UP)
#define WII_BUTTON_ATARI_DOWN (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_DOWN)
#define GC_BUTTON_ATARI_DOWN (PAD_BUTTON_DOWN)
#define WII_BUTTON_ATARI_LEFT (WPAD_BUTTON_UP)
#define WII_CLASSIC_ATARI_LEFT (WPAD_CLASSIC_BUTTON_LEFT)
#define GC_BUTTON_ATARI_LEFT (PAD_BUTTON_LEFT)

#define WII_BUTTON_ATARI_FIRE (WPAD_BUTTON_2)
#define GC_BUTTON_ATARI_FIRE (PAD_BUTTON_A)
#define WII_CLASSIC_ATARI_FIRE (WPAD_CLASSIC_BUTTON_A)
#define WII_NUNCHECK_ATARI_FIRE (WPAD_NUNCHUK_BUTTON_C)

#define WII_CLASSIC_ATARI_FIRE_2 (WPAD_CLASSIC_BUTTON_B)
#define WII_BUTTON_ATARI_FIRE_2 (WPAD_BUTTON_1)
#define GC_BUTTON_ATARI_FIRE_2 (PAD_BUTTON_B)
#define WII_NUNCHECK_ATARI_FIRE_2 (WPAD_NUNCHUK_BUTTON_Z)

#define WII_BUTTON_ATARI_DIFFICULTY_LEFT \
    (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_FULL_L)
#define WII_BUTTON_ATARI_DIFFICULTY_LEFT_LG \
    (WPAD_BUTTON_1 | WPAD_CLASSIC_BUTTON_FULL_L)
#define GC_BUTTON_ATARI_DIFFICULTY_LEFT (PAD_BUTTON_Y)

#define WII_BUTTON_ATARI_DIFFICULTY_RIGHT \
    (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_FULL_R)
#define WII_BUTTON_ATARI_DIFFICULTY_RIGHT_LG \
    (WPAD_BUTTON_2 | WPAD_CLASSIC_BUTTON_FULL_R)
#define GC_BUTTON_ATARI_DIFFICULTY_RIGHT (PAD_BUTTON_X)

#endif
