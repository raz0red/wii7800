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

#include <gccore.h>

#include "wii_main.h"
#include "wii_sdl.h"

#include "wii_atari.h"

/*
 * Returns the Atari blit surface
 *
 * return   The Atari blit surface
 */
u8* wii_sdl_get_blit_addr()
{
  return (u8*)blit_surface->pixels;  
}

/*
 * Initializes the SDL
 */
int wii_sdl_handle_init()
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0) {
    return 0;
  }

  if( SDL_InitSubSystem( SDL_INIT_VIDEO ) < 0 ) {
    return 0;
  }

  back_surface = 
    SDL_SetVideoMode(
    WII_WIDTH,
    WII_HEIGHT, 
    8, 
    SDL_DOUBLEBUF|SDL_HWSURFACE
    );

  if( !back_surface) 
  {
    return 0;
  }

  blit_surface = 
    SDL_CreateRGBSurface(
    SDL_SWSURFACE, 
    ATARI_WIDTH, 
    ATARI_BLIT_HEIGHT,
    back_surface->format->BitsPerPixel,
    back_surface->format->Rmask,
    back_surface->format->Gmask,
    back_surface->format->Bmask, 0);

  return 1;
}