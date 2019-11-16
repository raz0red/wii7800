/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_GX_H
#define WII_GX_H

#include <gccore.h>

#include "FreeTypeGX.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Draws a rectangle at the specified position
 * 
 * x        The x position
 * y        The y position
 * width    The width of the rectangle
 * height   The height of the rectangle
 * color    The color of the rectangle
 * filled   Whether the rectangle is filled
 */
void wii_gx_drawrectangle( 
  int x, int y, int width, int height, GXColor color, BOOL filled );

/*
 * Draws text at the specified position
 * 
 * x          The x position
 * y          The y position
 * pixelSize  The pixel size
 * color      The color of the text
 * textStyle  The style(s) for the text (FreeTypeGX)
 */
void wii_gx_drawtext( 
  int16_t x, int16_t y, FT_UInt pixelSize, char *text, GXColor color, 
  uint16_t textStyle );

#ifdef __cplusplus
}
#endif

#endif
