/*
Wii7800 : Port of the ProSystem Emulator for the Wii

Copyright (C) 2010 raz0red
*/

#include <malloc.h>
#include <string.h>

#include "ProSystem.h"
#include "Sound.h"

#include "wii_app.h"
#include "wii_config.h"
#include "wii_input.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"

#include "wii_atari.h"

extern "C" void WII_VideoStart();
extern "C" void WII_VideoStop();

/*
 * Starts the emulator for the specified rom file.
 *
 * romfile  The rom file to run in the emulator
 * savefile The name of the save file to load. If this value is NULL, no save
 *          is explicitly loaded (auto-load may occur). If the value is "", 
 *          no save is loaded and auto-load is ignored (used for reset).
 * reset    Whether we are resetting the current game
 * resume   Whether we are resuming the current game
 */
void wii_start_emulation( char *romfile, const char *savefile, bool reset, bool resume )
{
  // Disabled the high score cartridge
  wii_hs_enabled = ( wii_hs_mode != HSMODE_DISABLED );

  // Write out the current config
  wii_write_config();

  VIDEO_ClearFrameBuffer( vmode, wii_xfb[0], COLOR_BLACK );
  VIDEO_ClearFrameBuffer( vmode, wii_xfb[1], COLOR_BLACK );			

  bool succeeded = true;
  char autosavename[WII_MAX_PATH] = "";

  // Determine the name of the save file
  if( wii_auto_save_snapshot || wii_auto_load_snapshot )
  {
    wii_snapshot_handle_get_name( romfile, autosavename );
  }

  // If a specific save file was not specified, and we are auto-loading 
  // see if a save file exists
  if( savefile == NULL &&
    ( wii_auto_load_snapshot && 
    wii_check_snapshot( autosavename ) == 0 ) )
  {
    savefile = autosavename;
  }

  if( succeeded )
  {
    // Start emulation
    if( !reset && !resume )
    {
      bool loadsave = ( savefile != NULL && strlen( savefile ) > 0 );

      // Disable the high score cart for saves if applicable
      if( loadsave && ( wii_hs_mode != HSMODE_ENABLED_SNAPSHOTS ) )
      {
        wii_hs_enabled = false;
      }

      wii_reset_keyboard_data();
      succeeded = wii_atari_load_rom( romfile, !loadsave );

      if( loadsave )
      {
        // Ensure the save is valid
        int sscheck = wii_check_snapshot( savefile );
        if( sscheck < 0 )
        {
          if( sscheck == -2 )            
          {
            wii_set_status_message(
              "The save specified is not valid." );                
          }
          else
          {
            wii_set_status_message(
              "Unable to find the specified save state file." );                
          }

          succeeded = false;
        }
        else
        {
          // Run some test frames
          //
          // TODO:
          //
          // This is definitely a ProSystem emu bug. Basically, if 
          // you don't load the ROM and run some frames prior to 
          // loading save state the game may not work. This is due to
          // the fact that it relies on state set when the ROM is 
          // initially loaded (I believe it is Maria state). This is
          // something that should be resolved by adding Maria state
          // to the save file. For now, we just load the ROM and let
          // it execute for a second.
          wii_atari_main_loop( prosystem_frequency );

          succeeded = prosystem_Load( savefile );                    
          if( succeeded )
          {
            wii_atari_pause( false );
          }
        }
      }
    }
    else if( reset )
    {
      wii_reset_keyboard_data();
      prosystem_Reset();            
    }
    else
    {
      prosystem_Pause( false );
    }

    if( succeeded )
    {
      if( wii_max_frame_rate != 0 )
      {
        prosystem_frequency = wii_max_frame_rate;
      }

      wii_sdl_black_screen();
      WII_VideoStart();      

      // Wait until no buttons are pressed
      wii_wait_until_no_buttons( 2 );

      wii_atari_main_loop();            

      // Auto save?
      if( wii_auto_save_snapshot )
      {
        wii_save_snapshot( autosavename, TRUE );
      }        

      // Store the name of the last rom (for resuming later)        
      // Do it in this order in case they passed in the pointer
      // to the last rom variable
      char *last = strdup( romfile );
      if( wii_last_rom != NULL )
      {
        free( wii_last_rom );    
      }

      wii_last_rom = last;

      if( wii_top_menu_exit )
      {
        // Pop to the top
        while( wii_menu_pop() != NULL );
      }
    }
  }

  if( !succeeded )
  {
    // Reset the last rom that was loaded
    if( wii_last_rom != NULL )
    {
      free( wii_last_rom );
      wii_last_rom = NULL;
    }
  }

  WII_VideoStop();
}

/*
 * Resumes emulation of the current game
 */
void wii_resume_emulation()
{
  wii_start_emulation( wii_last_rom, "", false, true );
}

/*
 * Resets the current game
 */
void wii_reset_emulation()
{
  wii_start_emulation( wii_last_rom, "", true, false );
}
