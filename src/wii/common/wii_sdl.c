/*
Copyright (C) 2010 raz0red
*/

#include <gctypes.h>

#include "font_ttf.h"

#include "wii_app.h"
#include "wii_sdl.h"

// The Wii surface
SDL_Surface *back_surface = NULL;
// The Coleco surface
SDL_Surface *blit_surface = NULL;

// Fonts
TTF_Font *sdl_font_18 = NULL;
TTF_Font *sdl_font_14 = NULL;
TTF_Font *sdl_font_13 = NULL;
TTF_Font *sdl_font_12 = NULL;

// Colors
SDL_Color SDL_COLOR_WHITE = { 255, 255, 255, 0 };
SDL_Color SDL_COLOR_BLACK = { 0, 0, 0, 0 };
SDL_Color SDL_COLOR_RED = { 255, 0, 0, 0 };

/*
* Maps the specified color into the back surface.
*
* return   The index of the specified color
*/
uint wii_sdl_rgb( u8 R, u8 G, u8 B )
{
  return SDL_MapRGB( back_surface->format, R, G, B );
}

/*
 * Renders black to both blit and back surfaces
 */
void wii_sdl_black_screen()
{
  SDL_FillRect( blit_surface, NULL, SDL_MapRGB(blit_surface->format, 0x0,0x0,0x0 ) );
  SDL_Flip( blit_surface );
  wii_sdl_black_back_surface();
}

/*
 * Blacks the back surface
 */
void wii_sdl_black_back_surface()
{
  SDL_FillRect( back_surface, NULL, SDL_MapRGB(back_surface->format, 0x0,0x0,0x0 ) );
  SDL_Flip( back_surface );
}

/*
 * Renders to the Wii surface. 
 * TODO: replace this lame scaling method w/ GX based scaler
 *
 * scale  The scale to render the surface (1 or 2)
 */
void wii_sdl_put_image_normal( int scale )
{
  int offsetx = ( ( WII_WIDTH - ( blit_surface->w * scale ) ) / 2 );
  int offsety =  ( ( WII_HEIGHT - ( blit_surface->h * scale ) ) / 2 );

  int src = 0, dst = 0, start = 0, x = 0, y = 0, i = 0;
  u8* backpixels = (u8*)back_surface->pixels;
  u8* blitpixels = (u8*)blit_surface->pixels;
  int startoffset =  0;
  for( y = 0; y < blit_surface->h; y++ )
  {    
    for( i = 0; i < scale; i++ )
    {
      start = startoffset + ( y * blit_surface->w );
      src = 0;        
      dst = ( ( ( ( y * scale ) + i ) + offsety ) * WII_WIDTH ) + offsetx;
      for( x = 0; x < blit_surface->w; x++ )
      {           
        int j;
        for( j = 0; j < scale; j++ )
        {
          backpixels[dst++] = blitpixels[start + src];
        }
        src++;
      }
    }            
  }
}

/*
 * Determines the size of the text displayed with the specified font
 *
 * font     The font
 * text     The text
 * w        The width (return value)
 * h        The height (return value)
 */
void wii_sdl_get_text_size( TTF_Font *font, const char *text, int *w, int *h )
{
  TTF_SizeText( font, text, w, h );
}

/*
 * Renders text to the Wii surface
 *
 * font       The font to render with
 * text       The text to display
 * destRect   The location to render the text
 * colorFG    The forground color (optional)
 * colorBG    The background color (optional)
 */
void wii_sdl_render_text( 
  TTF_Font *font, const char *text, 
  SDL_Rect *destRect, 
  SDL_Color *colorFG, SDL_Color *colorBG )
{ 
  SDL_Surface *sText = NULL;

  if( colorBG != NULL )
  {
    sText =
      TTF_RenderText_Shaded( font, text, 
        ( colorFG != NULL ? *colorFG : SDL_COLOR_WHITE ), SDL_COLOR_BLACK );
  }
  else
  {
    sText =
      TTF_RenderText_Solid( font, text, 
        ( colorFG != NULL ? *colorFG : SDL_COLOR_WHITE ) );
  }

  if( sText == NULL )
  {
    return;
  }

  SDL_Rect srcRect = { 0, 0, destRect->w, destRect->h };
  SDL_Rect* srcRectPtr = NULL;
  if( destRect->w > 0 && destRect->h > 0 )
  {
    srcRectPtr = &srcRect;
  }  

  SDL_Surface *sTextDF = SDL_DisplayFormat( sText );
  SDL_FreeSurface( sText );
  SDL_BlitSurface( sTextDF, srcRectPtr, back_surface, destRect );
  SDL_FreeSurface( sTextDF ); 
}

/*
 * Returns a pointer to the specified x,y location in the Wii surface
 *
 * surface  The surface
 * x        The x location
 * y        The y location
 * return   A pointer to the specified x,y location in the Wii surface
 */
static u8* wii_sdl_get_vram_addr( SDL_Surface* surface, uint x, uint y )
{
  u8 *vram = (u8*)surface->pixels;
  return vram + x + ( y * surface->w );
}

/*
 * Renders a rectangle to the back (Wii) surface
 *
 * surface  The surface to render to
 * x        The x location
 * y        The y location
 * w        The width
 * h        The height
 * border   The border color
 * xor      Exclusive or...
 */
void wii_sdl_draw_rectangle( 
  SDL_Surface* surface, int x, int y, int w, int h, int border, BOOL exor ) 
{
  if( x < 0 ) { w += x; x = 0; }
  if( y < 0 ) { h += y; y = 0; }
  if( ( x + w ) > surface->w ) w = surface->w - x;
  if( ( y + h ) > surface->h ) h = surface->h - y;
  if( w <= 0 || h <= 0 ) return;

  u8 *vram = (u8*)wii_sdl_get_vram_addr( surface, x, y );

  int xo, yo;
  if( exor )
  {
    for( xo = 1; xo < (w - 1); xo++ ) 
    {
      vram[xo] ^= border;
      if( h > 1 )
      {
        vram[xo + (surface->w * (h - 1 ))] ^= border;
      }
    }
    for( yo = 0; yo < h; yo++ ) 
    {
      vram[yo * surface->w] ^= border;
      if( w > 1 )
      {
        vram[(yo * surface->w) + (w - 1)] ^= border;
      }
    }
  }
  else
  {
    for( xo = 1; xo < (w - 1); xo++ ) 
    {
      vram[xo] = border;
      if( h > 1 )
      {
        vram[xo + (surface->w * (h - 1 ))] = border;
      }
    }
    for( yo = 0; yo < h; yo++ ) 
    {
      vram[yo * surface->w] = border;
      if( w > 1 )
      {
        vram[(yo * surface->w) + (w - 1)] = border;
      }
    }
  }
}

/*
 * Renders a filled rectangle to the back (Wii) surface
 *
 * surface  The surface to render to
 * x        The x location
 * y        The y location
 * w        The width
 * h        The height
 * color    The color
 */
void wii_sdl_fill_rectangle( 
  SDL_Surface* surface, int x, int y, int w, int h, int color )
{
  if( x < 0 ) { w += x; x = 0; }
  if( y < 0 ) { h += y; y = 0; }
  if( ( x + w ) > surface->w ) w = surface->w - x;
  if( ( y + h ) > surface->h ) h = surface->h - y;
  if( w <= 0 || h <= 0 ) return;

  u8 *vram  = (u8*)wii_sdl_get_vram_addr( surface, x, y );
  int xo, yo;

  for( xo = 0; xo < w; xo++ ) 
  {
    for( yo = 0; yo < h; yo++ ) 
    {
      vram[xo + (yo * surface->w)] = color;            
    }
  }
}

/*
 * Initializes the SDL
 */
int wii_sdl_init()
{
  // App initialization of the SDL
  wii_sdl_handle_init();

  // Don't show the cursor
  SDL_ShowCursor( SDL_DISABLE );

  // True type fonts
  TTF_Init();

  SDL_RWops* rw = SDL_RWFromMem( (u8*)font_ttf, font_ttf_size );
  sdl_font_18 = TTF_OpenFontRW( rw, 1, 18 );
  rw = SDL_RWFromMem( (u8*)font_ttf, font_ttf_size );
  sdl_font_14 = TTF_OpenFontRW( rw, 1, 14 );
  rw = SDL_RWFromMem( (u8*)font_ttf, font_ttf_size );
  sdl_font_13 = TTF_OpenFontRW( rw, 1, 13 );
  rw = SDL_RWFromMem( (u8*)font_ttf, font_ttf_size );
  sdl_font_12 = TTF_OpenFontRW( rw, 1, 12 );  

  return 1;
}

/*
 * Renders the current back surface
 */
void wii_sdl_flip()
{
  SDL_Flip( back_surface );
}

/*
 * Frees the SDL resources
 */
void wii_sdl_free_resources()
{
  if( back_surface != NULL )
  {
    SDL_FreeSurface( back_surface );
  }
  if( blit_surface != NULL )
  {
    SDL_FreeSurface( blit_surface );
  }
  if( sdl_font_18 != NULL )
  {
    TTF_CloseFont( sdl_font_18 );    
  }
  if( sdl_font_14 != NULL )
  {
    TTF_CloseFont( sdl_font_14 );    
  }
  if( sdl_font_13 != NULL )
  {
    TTF_CloseFont( sdl_font_13 );    
  }
  if( sdl_font_12 != NULL )
  {
    TTF_CloseFont( sdl_font_12 );    
  }
}

