/*
Copyright (C) 2010 raz0red
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