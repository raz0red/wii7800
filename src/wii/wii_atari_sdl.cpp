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

#include <gccore.h>

#include "wii_main.h"
#include "wii_sdl.h"

#include "wii_atari.h"

/**
 * Returns the Atari blit surface
 *
 * return   The Atari blit surface
 */
u8* wii_sdl_get_blit_addr() {
    return (u8*)blit_surface->pixels;
}

/**
 * Initializes the SDL
 */
int wii_sdl_handle_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    back_surface = SDL_SetVideoMode(WII_WIDTH, WII_HEIGHT, 8,
                                    SDL_DOUBLEBUF | SDL_HWSURFACE);

    if (!back_surface) {
        return 0;
    }

    blit_surface = SDL_CreateRGBSurface(
        SDL_SWSURFACE, ATARI_WIDTH, ATARI_BLIT_HEIGHT,
        back_surface->format->BitsPerPixel, back_surface->format->Rmask,
        back_surface->format->Gmask, back_surface->format->Bmask, 0);

    return 1;
}