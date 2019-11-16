/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_RESIZE_SCREEN_H
#define WII_RESIZE_SCREEN_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Information about the resize operation
 */
typedef struct resize_info {
  float defaultX; 
  float defaultY; 
  float currentX; 
  float currentY;
} resize_info;

/*
 * Displays the resize user interface
 *
 * rinfo  Information for the resize operation
 */
extern void wii_resize_screen_gui( resize_info* rinfo );

/*
 * Draws a border around the surface that is to be scaled.
 *
 * surface  The surface to scale
 * startY   The Y offset into the surface to scale
 * height   The height to scale
 */
extern void wii_resize_screen_draw_border( SDL_Surface* surface, int startY, int height );

#ifdef __cplusplus
}
#endif

#endif
