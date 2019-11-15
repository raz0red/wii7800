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

#include <stdio.h>
#include <stdlib.h>
#include <sys/iosupport.h>

#include "Region.h"

#include "wii_app_common.h"
#include "wii_main.h"
#include "wii_app.h"
#include "wii_resize_screen.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"
#include "wii_util.h"

#include "wii_atari.h"
#include "wii_atari_emulation.h"
#include "wii_atari_snapshot.h"

extern void WII_VideoStart();
extern void WII_VideoStop();
extern int PauseAudio(int Switch);
extern void ResetAudio();

// Have we read the games list yet?
static BOOL games_read = FALSE;
// Have we read the save state list yet?
static BOOL save_states_read = FALSE;
// The index of the last rom that was run
static s16 last_rom_index = 1;

// Forward refs
static void wii_read_save_state_list( TREENODE *menu );
static void wii_read_game_list( TREENODE *menu );

/*
 * Initializes the Atari menu
 */
void wii_atari_menu_init()
{  
  //
  // The root menu
  //

  wii_menu_root = wii_create_tree_node( NODETYPE_ROOT, "root" );

  TREENODE* child = NULL;
  child = wii_create_tree_node( NODETYPE_RESUME, "Resume" );
  wii_add_child( wii_menu_root, child );

  child = NULL;
  child = wii_create_tree_node( NODETYPE_RESET, "Reset" );
  wii_add_child( wii_menu_root, child );

  child = wii_create_tree_node( NODETYPE_LOAD_ROM, "Load cartridge" );
  wii_add_child( wii_menu_root, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // Snapshot management
  //

  TREENODE *state = wii_create_tree_node( 
    NODETYPE_SNAPSHOT_MANAGEMENT, "Save state management" );
  wii_add_child( wii_menu_root, state );

  child = wii_create_tree_node( NODETYPE_AUTO_LOAD_SNAPSHOT, 
    "Auto load " );
  child->x = -2; child->value_x = -3;
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_AUTO_SAVE_SNAPSHOT, 
    "Auto save " );
  child->x = -2; child->value_x = -3;
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_LOAD_SNAPSHOT, 
    "Load saved state" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SAVE_SNAPSHOT, 
    "Save state (current game)" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_DELETE_SNAPSHOT, 
    "Delete saved state (current game)" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // The display settings menu
  //

  TREENODE *display = wii_create_tree_node( NODETYPE_DISPLAY_SETTINGS, 
    "Display settings" );
  wii_add_child( wii_menu_root, display );

  child = wii_create_tree_node( NODETYPE_RESIZE_SCREEN, 
    "Screen size " );      
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );     

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_DIFF_SWITCH_DISPLAY, 
    "Diff. switch display " );
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_LIGHTGUN_CROSSHAIR, 
    "Lightgun crosshair " );
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_LIGHTGUN_FLASH, 
    "Lightgun flash " );
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_MAX_FRAME_RATE, 
    "Max frame rate " );        
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );

  child = wii_create_tree_node( NODETYPE_VSYNC, 
    "Vertical sync " );      
  child->x = -2; child->value_x = -3;
  wii_add_child( display, child );   

  //
  // The controls settings menu
  //

  TREENODE *controls = wii_create_tree_node( NODETYPE_CONTROLS_SETTINGS, 
    "Control settings" );
  wii_add_child( wii_menu_root, controls );

  child = wii_create_tree_node( NODETYPE_SWAP_BUTTONS, 
    "Swap buttons " );
  child->x = -2; child->value_x = -3;
  wii_add_child( controls, child );

  child = wii_create_tree_node( NODETYPE_DIFF_SWITCH_ENABLED, 
    "Diff. switches " );
  child->x = -2; child->value_x = -3;
  wii_add_child( controls, child );

  //
  // The cartridge settings menu
  //

  TREENODE *cartridge = wii_create_tree_node( NODETYPE_CARTRIDGE_SETTINGS, 
    "Cartridge settings" );
  wii_add_child( wii_menu_root, cartridge );

  child = wii_create_tree_node( NODETYPE_HIGH_SCORE_MODE, 
    "High score cart. " );
  child->x = -2; child->value_x = -3;
  wii_add_child( cartridge, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( cartridge, child );

  child = wii_create_tree_node( NODETYPE_CARTRIDGE_WSYNC, 
    "Handle WSYNC " );
  child->x = -2; child->value_x = -3;
  wii_add_child( cartridge, child );

  child = wii_create_tree_node( NODETYPE_CARTRIDGE_CYCLE_STEALING, 
    "Cycle stealing " );
  child->x = -2; child->value_x = -3;
  wii_add_child( cartridge, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // The advanced menu
  //

  TREENODE *advanced = wii_create_tree_node( NODETYPE_ADVANCED, 
    "Advanced" );
  wii_add_child( wii_menu_root, advanced );    

  child = wii_create_tree_node( NODETYPE_DEBUG_MODE, 
    "Debug mode " );
  child->x = -2; child->value_x = -3;
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_TOP_MENU_EXIT, 
    "Top menu exit " );
  child->x = -2; child->value_x = -3;
  wii_add_child( advanced, child );

  wii_menu_push( wii_menu_root );	
}

/*
 * Updates the buffer with the header message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the header message for the
 *          current menu.
 */
void wii_menu_handle_get_header( TREENODE* menu, char *buffer )
{
  switch( menu->node_type )
  {
  case NODETYPE_LOAD_ROM:
    if( !games_read )
    {
      snprintf( buffer, WII_MENU_BUFF_SIZE, "Reading game list..." );                
    }
    break;
  case NODETYPE_LOAD_SNAPSHOT:
    if( !save_states_read )
    {
      snprintf( buffer, WII_MENU_BUFF_SIZE, "Reading saved state list..." );                            
    }
    break;
  default:
    /* do nothing */
    break;
  }
}


/*
 * Updates the buffer with the footer message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the footer message for the
 *          current menu.
 */
void wii_menu_handle_get_footer( TREENODE* menu, char *buffer )
{
  switch( menu->node_type )
  {
  case NODETYPE_LOAD_ROM:
    if( games_read )
    {
      wii_get_list_footer( menu, "cartridge", buffer );
    }
    break;
  case NODETYPE_LOAD_SNAPSHOT:
    wii_get_list_footer( menu, "state save", buffer );
    break;
  default:
    break;
  }
}

/*
 * Updates the buffer with the name of the specified node
 *
 * node     The node
 * name     The name of the specified node
 * value    The value of the specified node
 */
void wii_menu_handle_get_node_name( 
  TREENODE* node, char *buffer, char* value )
{
  const char *strmode = NULL;
  snprintf( buffer, WII_MENU_BUFF_SIZE, "%s", node->name );

  switch( node->node_type )
  {
  case NODETYPE_RESIZE_SCREEN:
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
      ( ( wii_screen_x == DEFAULT_SCREEN_X && 
        wii_screen_y == DEFAULT_SCREEN_Y ) ? "(Default)" : "Custom" ) );
    break;
  case NODETYPE_DIFF_SWITCH_DISPLAY:
    switch( wii_diff_switch_display )
    {
    case DIFF_SWITCH_DISPLAY_DISABLED:
      strmode="Disabled";
      break;
    case DIFF_SWITCH_DISPLAY_WHEN_CHANGED:
      strmode="When changed (Default)";
      break;
    case DIFF_SWITCH_DISPLAY_ALWAYS:
      strmode="Always";
      break;
    default:
      break;
    }
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
    break;
  case NODETYPE_VSYNC:
    switch( wii_vsync )
    {
    case VSYNC_DISABLED:
      strmode="Disabled";
      break;
    case VSYNC_ENABLED:
      strmode="Enabled";
      break;
    default:
      break;
    }
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
    break;
  case NODETYPE_HIGH_SCORE_MODE:
    switch( wii_hs_mode )
    {
    case HSMODE_ENABLED_NORMAL:
      strmode="Enabled (excludes saved state)";
      break;
    case HSMODE_ENABLED_SNAPSHOTS:
      strmode="Enabled (includes saved state)";
      break;
    case HSMODE_DISABLED:
      strmode="Disabled";
      break;
    default:
      break;
    }
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
    break;
  case NODETYPE_CARTRIDGE_WSYNC:
    switch( wii_cart_wsync )
    {
    case CART_MODE_AUTO:
      strmode="(auto)";
      break;
    case CART_MODE_ENABLED:
      strmode="Enabled";
      break;
    case CART_MODE_DISABLED:
      strmode="Disabled";
      break;
    default:
      break;
    }
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
    break;
  case NODETYPE_CARTRIDGE_CYCLE_STEALING:
    switch( wii_cart_cycle_stealing )
    {
    case CART_MODE_AUTO:
      strmode="(auto)";
      break;
    case CART_MODE_ENABLED:
      strmode="Enabled";
      break;
    case CART_MODE_DISABLED:
      strmode="Disabled";
      break;
    }
    snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
    break;
  case NODETYPE_MAX_FRAME_RATE:
    if( wii_max_frame_rate == 0 )
    {
      snprintf( value, WII_MENU_BUFF_SIZE, "(Auto)" );
    }
    else
    {
      snprintf( value, WII_MENU_BUFF_SIZE, "%d", wii_max_frame_rate );
    }
    break;
  case NODETYPE_DEBUG_MODE:
  case NODETYPE_TOP_MENU_EXIT:
  case NODETYPE_AUTO_LOAD_SNAPSHOT:
  case NODETYPE_AUTO_SAVE_SNAPSHOT:
  case NODETYPE_SWAP_BUTTONS:
  case NODETYPE_DIFF_SWITCH_ENABLED:
  case NODETYPE_LIGHTGUN_CROSSHAIR:
  case NODETYPE_LIGHTGUN_FLASH:
    {
      BOOL enabled = FALSE;
      switch( node->node_type )
      {
      case NODETYPE_DEBUG_MODE:
        enabled = wii_debug;
        break;
      case NODETYPE_TOP_MENU_EXIT:
        enabled = wii_top_menu_exit;
        break;
      case NODETYPE_AUTO_LOAD_SNAPSHOT:
        enabled = wii_auto_load_snapshot;
        break;
      case NODETYPE_AUTO_SAVE_SNAPSHOT:
        enabled = wii_auto_save_snapshot;
        break;
      case NODETYPE_SWAP_BUTTONS:
        enabled = wii_swap_buttons;
        break;
      case NODETYPE_DIFF_SWITCH_ENABLED:
        enabled = wii_diff_switch_enabled;
        break;
      case NODETYPE_LIGHTGUN_CROSSHAIR:
        enabled = wii_lightgun_crosshair;
        break;
      case NODETYPE_LIGHTGUN_FLASH:
        enabled = wii_lightgun_flash;
        break;
      default:
        break;
      }

      snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
        enabled ? "Enabled" : "Disabled" );
      break;
    }
  default:	    
    break;
  }
}

extern void wii_atari_put_image_gu_normal();

/*
 * React to the "select" event for the specified node
 *
 * node     The node
 */
void wii_menu_handle_select_node( TREENODE *node )
{
  switch( node->node_type )
  {
  case NODETYPE_RESIZE_SCREEN:
    {
      int blity = ( cartridge_region == REGION_NTSC ? 
        NTSC_ATARI_BLIT_TOP_Y : PAL_ATARI_BLIT_TOP_Y );
      int height = ( cartridge_region == REGION_NTSC ? 
        NTSC_ATARI_HEIGHT : PAL_ATARI_HEIGHT );
      wii_resize_screen_draw_border( blit_surface, blity, height );
      wii_atari_put_image_gu_normal();
      wii_sdl_flip(); 
      resize_info rinfo = { 
        DEFAULT_SCREEN_X, DEFAULT_SCREEN_Y, wii_screen_x, wii_screen_y };
      wii_resize_screen_gui( &rinfo );
      wii_screen_x = rinfo.currentX;
      wii_screen_y = rinfo.currentY;
    }
    break;
  case NODETYPE_DIFF_SWITCH_DISPLAY:
    wii_diff_switch_display++;
    if( wii_diff_switch_display > 2 ) 
    {
      wii_diff_switch_display = 0;
    }			
    break;
  case NODETYPE_CARTRIDGE_CYCLE_STEALING:
    wii_cart_cycle_stealing++;
    if( wii_cart_cycle_stealing > 2 ) 
    {
      wii_cart_cycle_stealing = 0;
    }			
    break;
  case NODETYPE_CARTRIDGE_WSYNC:
    wii_cart_wsync++;
    if( wii_cart_wsync > 2 ) 
    {
      wii_cart_wsync = 0;
    }			
    break;
  case NODETYPE_HIGH_SCORE_MODE:
    wii_hs_mode++;
    if( wii_hs_mode > 2 ) 
    {
      wii_hs_mode = 0;
    }
    break;
  case NODETYPE_VSYNC:
    wii_set_vsync( wii_vsync ^ 1 );
    break;
  case NODETYPE_LIGHTGUN_CROSSHAIR:
    wii_lightgun_crosshair ^= 1;
    break;
  case NODETYPE_LIGHTGUN_FLASH:
    wii_lightgun_flash ^= 1;
    break;
  case NODETYPE_ROM:
    char buff[WII_MAX_PATH];
    snprintf( buff, sizeof(buff), "%s%s", WII_ROMS_DIR, node->name );             
    last_rom_index = wii_menu_get_current_index();
    wii_start_emulation( buff );
    break;
  case NODETYPE_RESUME:
    wii_resume_emulation();
    break;
  case NODETYPE_RESET:
    wii_reset_emulation();
    break;
  case NODETYPE_SNAPSHOT:
    snprintf( buff, sizeof(buff), "%s%s", WII_SAVES_DIR, node->name );  
    wii_start_snapshot( buff );
    break;
  case NODETYPE_MAX_FRAME_RATE:
    wii_max_frame_rate += 1;
    if( wii_max_frame_rate > 70 )
    {
      wii_max_frame_rate = 0;
    }
    else if( wii_max_frame_rate == 1 )
    {
      wii_max_frame_rate = 30;
    }
    break;
  case NODETYPE_TOP_MENU_EXIT:
    wii_top_menu_exit ^= 1;
    break;
  case NODETYPE_DEBUG_MODE:
    wii_debug ^= 1;
    break;
  case NODETYPE_SWAP_BUTTONS:
    wii_swap_buttons ^= 1;
    break;
  case NODETYPE_DIFF_SWITCH_ENABLED:
    wii_diff_switch_enabled ^= 1;
    break;
  case NODETYPE_AUTO_LOAD_SNAPSHOT:
    wii_auto_load_snapshot ^= 1;
    break;
  case NODETYPE_AUTO_SAVE_SNAPSHOT:
    wii_auto_save_snapshot ^= 1;
    break;
  case NODETYPE_SNAPSHOT_MANAGEMENT:
  case NODETYPE_ADVANCED:
  case NODETYPE_LOAD_ROM:     
  case NODETYPE_CARTRIDGE_SETTINGS:
  case NODETYPE_DISPLAY_SETTINGS:
  case NODETYPE_CONTROLS_SETTINGS:
    wii_menu_push( node );
    if( node->node_type == NODETYPE_LOAD_ROM )
    {
      wii_menu_move( node, last_rom_index );
    }
    break;
  case NODETYPE_LOAD_SNAPSHOT:
    wii_menu_clear_children( node );
    wii_menu_push( node );
    save_states_read = FALSE;
    break;
  case NODETYPE_SAVE_SNAPSHOT:
    wii_save_snapshot( NULL, TRUE );
    break;
  case NODETYPE_DELETE_SNAPSHOT:
    wii_delete_snapshot();
    break;
  default:
    break;
  }
}

/*
 * Determines whether the node is currently visible
 *
 * node     The node
 * return   Whether the node is visible
 */
BOOL wii_menu_handle_is_node_visible( TREENODE *node )
{
  switch( node->node_type )
  {
  case NODETYPE_SAVE_SNAPSHOT:
  case NODETYPE_RESET:
  case NODETYPE_RESUME:
    return wii_last_rom != NULL;
    break;
  case NODETYPE_DELETE_SNAPSHOT:
    if( wii_last_rom != NULL )
    {
      char savename[WII_MAX_PATH];
      wii_snapshot_handle_get_name( wii_last_rom, savename );
      return Util_fileexists( savename );
    }
    return FALSE;
    break;
  default:
    break;
  }

  return TRUE;
}

/*
 * Determines whether the node is selectable
 *
 * node     The node
 * return   Whether the node is selectable
 */
BOOL wii_menu_handle_is_node_selectable( TREENODE *node )
{
  return TRUE;
}

/*
 * Provides an opportunity for the specified menu to do something during 
 * a display cycle.
 *
 * menu     The menu
 */
void wii_menu_handle_update( TREENODE *menu )
{
  switch( menu->node_type )
  {
  case NODETYPE_LOAD_ROM:
    if( !games_read )
    {
      wii_read_game_list( menu );  
      wii_menu_reset_indexes();    
      wii_menu_move( menu, 1 );
      wii_menu_force_redraw = 1;
    }
    break;
  case NODETYPE_LOAD_SNAPSHOT:
    if( !save_states_read )
    {
      wii_read_save_state_list( menu );    
      wii_menu_reset_indexes();    
      wii_menu_move( menu, 1 );
      wii_menu_force_redraw = 1;            
    }
    break;
  default:
    /* do nothing */
    break;
  }
}

/*
 * Reads the list of games into the specified menu
 *
 * menu     The menu to read the games into
 */
static void wii_read_game_list( TREENODE *menu )
{
  DIR *romdir = opendir( WII_ROMS_DIR );
  if( romdir != NULL)
  {
    struct dirent *dent;
    struct stat statbuf;
    while ((dent = readdir(romdir)) != NULL)
    {
      char* filepath = dent->d_name;
      char path[WII_MAX_PATH];
      sprintf(path,"%s/%s", WII_ROMS_DIR, filepath);
      stat(path, &statbuf);
      if( strcmp( ".", filepath ) != 0 && 
        strcmp( "..", filepath ) != 0 )
      {
        if( !S_ISDIR( statbuf.st_mode ) )
        {
          TREENODE *child = 
            wii_create_tree_node( NODETYPE_ROM, filepath );

          wii_add_child( menu, child );
        }
      }
    }

    closedir( romdir );
  }
  else
  {
    wii_set_status_message( "Error opening roms directory." );
  }

  // Sort the games list
  qsort( menu->children, menu->child_count, 
    sizeof(*(menu->children)), wii_menu_name_compare );

  games_read = 1;
}

/*
 * Reads the list of snapshots into the specified menu
 *
 * menu     The menu to read the snapshots into
 */
static void wii_read_save_state_list( TREENODE *menu )
{
  DIR *ssdir = opendir( WII_SAVES_DIR );
  if( ssdir != NULL)
  { 
    struct dirent *dent;
    struct stat statbuf;
    char ext[WII_MAX_PATH];
    char filepath[WII_MAX_PATH];
    while ((dent = readdir(ssdir)) != NULL)
    {
      char* filepath = dent->d_name;
      char path[WII_MAX_PATH];
      sprintf(path,"%s/%s", WII_ROMS_DIR, filepath);
      stat(path, &statbuf);            
      if( strcmp( ".", filepath ) != 0 && 
        strcmp( "..", filepath ) != 0 )
      {                
        Util_getextension( filepath, ext );
        if( strcmp( ext, WII_SAVE_GAME_EXT ) == 0 )
        {                    
          if( !S_ISDIR( statbuf.st_mode ) )
          {
            // TODO: Check to see if a rom exists for the snapshot
            // TODO: Provide option to display cart info from 
            //       header
            TREENODE *child = 
              wii_create_tree_node( NODETYPE_SNAPSHOT, filepath );

            wii_add_child( menu, child );
          }
        }
      }
    }

    closedir( ssdir );
  }
  else
  {
    wii_set_status_message( "Error opening state saves directory." );
  }

  // Sort the games list
  qsort( menu->children, menu->child_count, 
    sizeof(*(menu->children)), wii_menu_name_compare );

  save_states_read = TRUE;
}

/*
 * Invoked after exiting the menu loop
 */
void wii_menu_handle_post_loop()
{
}

/*
 * Invoked prior to entering the menu loop
 */
void wii_menu_handle_pre_loop()
{
}

/*
 * Invoked when the home button is pressed when the 
 * menu is being displayed
 */
void wii_menu_handle_home_button()
{
}