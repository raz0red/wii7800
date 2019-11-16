/*
Wii7800 : Port of the ProSystem Emulator for the Wii

Copyright (C) 2010 raz0red
*/


#include <stdio.h>
#include <string.h>

#include "wii_main.h"
#include "wii_util.h"

#include "wii_atari.h"

extern "C" {

/*
 * Handles reading a particular configuration value
 *
 * name   The name of the config value
 * value  The config value
 */
void wii_config_handle_read_value( char *name, char* value )
{
  if ( strcmp( name, "DEBUG" ) == 0 )
  {
    wii_debug = Util_sscandec( value );				
  }
  else if ( strcmp( name, "MAX_FRAME_RATE" ) == 0 )
  {
    wii_max_frame_rate = Util_sscandec( value );				
  }
  else if ( strcmp( name, "TOP_MENU_EXIT" ) == 0 )
  {
    wii_top_menu_exit = Util_sscandec( value );				
  }
  else if ( strcmp( name, "AUTO_LOAD_SNAPSHOT" ) == 0 )
  {
    wii_auto_load_snapshot = Util_sscandec( value );				
  }
  else if ( strcmp( name, "AUTO_SAVE_SNAPSHOT" ) == 0 )
  {
    wii_auto_save_snapshot = Util_sscandec( value );				
  }
  else if( strcmp( name, "VSYNC" ) == 0 )
  {
    wii_vsync = Util_sscandec( value );
  }
  else if( strcmp( name, "DIFF_SWITCH_DISPLAY" ) == 0 )
  {
    wii_diff_switch_display = Util_sscandec( value );
  }
  else if( strcmp( name, "DIFF_SWITCH_ENABLED" ) == 0 )
  {
    wii_diff_switch_enabled = Util_sscandec( value );
  }
  else if( strcmp( name, "SWAP_BUTTONS" ) == 0 )
  {
    wii_swap_buttons = Util_sscandec( value );
  }
  else if( strcmp( name, "HIGH_SCORE_CART" ) == 0 )
  {
    wii_hs_mode = Util_sscandec( value );
  }
  else if( strcmp( name, "CART_WSYNC" ) == 0 )
  {
    wii_cart_wsync = Util_sscandec( value );
  }
  else if( strcmp( name, "CART_CYCLE_STEALING" ) == 0 )
  {
    wii_cart_cycle_stealing = Util_sscandec( value );
  }
  else if( strcmp( name, "LIGHTGUN_CROSSHAIR" ) == 0 )
  {
    wii_lightgun_crosshair = Util_sscandec( value );
  }
  else if( strcmp( name, "LIGHTGUN_FLASH" ) == 0 )
  {
    wii_lightgun_flash = Util_sscandec( value );
  }
  else if( strcmp( name, "SCREEN_X" ) == 0 )
  {
    wii_screen_x = Util_sscandec( value );
  }
  else if( strcmp( name, "SCREEN_Y" ) == 0 )
  {
    wii_screen_y = Util_sscandec( value );
  }
}

/*
 * Handles the writing of the configuration file
 *
 * fp   The file pointer
 */
void wii_config_handle_write_config( FILE *fp )
{
  fprintf( fp, "DEBUG=%d\n", wii_debug );
  fprintf( fp, "MAX_FRAME_RATE=%d\n", wii_max_frame_rate );
  fprintf( fp, "TOP_MENU_EXIT=%d\n", wii_top_menu_exit );
  fprintf( fp, "AUTO_LOAD_SNAPSHOT=%d\n", wii_auto_load_snapshot );
  fprintf( fp, "AUTO_SAVE_SNAPSHOT=%d\n", wii_auto_save_snapshot );
  fprintf( fp, "VSYNC=%d\n", wii_vsync );
  fprintf( fp, "DIFF_SWITCH_DISPLAY=%d\n", wii_diff_switch_display );
  fprintf( fp, "SWAP_BUTTONS=%d\n", wii_swap_buttons );
  fprintf( fp, "DIFF_SWITCH_ENABLED=%d\n", wii_diff_switch_enabled );
  fprintf( fp, "HIGH_SCORE_CART=%d\n", wii_hs_mode );
  fprintf( fp, "CART_WSYNC=%d\n", wii_cart_wsync );
  fprintf( fp, "CART_CYCLE_STEALING=%d\n", wii_cart_cycle_stealing );
  fprintf( fp, "LIGHTGUN_CROSSHAIR=%d\n", wii_lightgun_crosshair );
  fprintf( fp, "LIGHTGUN_FLASH=%d\n", wii_lightgun_flash );
  fprintf( fp, "SCREEN_X=%d\n", wii_screen_x );
  fprintf( fp, "SCREEN_Y=%d\n", wii_screen_y );
}

}