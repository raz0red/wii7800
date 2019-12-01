/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_FREETYPE_H
#define WII_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initializes the FreeType library
 *
 * returns  If an error occurred (non-zero)
 */
extern int wii_ft_init();

/*
 * Sets the font size
 *
 * pixelsize    The font size
 * returns      If an error occurred (non-zero)
 */
extern int wii_ft_set_fontsize( int pixelsize );

/*
 * Sets the font color
 *
 * r  Red
 * g  Green
 * b  Blue
 */
extern void wii_ft_set_fontcolor( u8 r, u8 g, u8 b );

/*
 * Draws the specified text
 *
 * xfb    The framebuffer
 * x      The x location 
 *          (-1 auto center, -2 left of center, -3 right of center)
 * y      The y location
 * text   The text to draw
 */
extern void wii_ft_drawtext( u32* xfb, int x, int y, char *text );

/*
 * Calculates the width of the specified text
 *
 * text     The text
 * return   The width of the text 
 */
extern int wii_ft_get_textwidth( char *text );

#ifdef __cplusplus
}
#endif

#endif

