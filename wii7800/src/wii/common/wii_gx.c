/*
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

#include "FreeTypeGX.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"

extern Mtx gx_view;

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
  int x, int y, int width, int height, GXColor color, BOOL filled )
{
  u8 fmt;
  long n;
  int i;
  int x2 = x+width;
  int y2 = y-height;
  if( !filled ) { x+=1; y-=1; }
  guVector v[] = {{x,y,0}, {x2,y,0}, {x2,y2,0}, {x,y2,0}, {x,y+1,0}};

  if(!filled)
  {
    fmt = GX_LINESTRIP;
    n = 5;
  }
  else
  {
    fmt = GX_TRIANGLEFAN;
    n = 4;
  }

  GX_Begin( fmt, GX_VTXFMT0, n );
  for(i=0; i<n; i++)
  {
    GX_Position3s16( v[i].x, v[i].y, v[i].z );
    GX_Color4u8( color.r, color.g, color.b, color.a );
  }
  GX_End();
}

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
  uint16_t textStyle )
{
  FT_DrawText( x, y, pixelSize, text, color, textStyle );
}