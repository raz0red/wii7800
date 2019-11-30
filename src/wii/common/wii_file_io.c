/*
Copyright (C) 2010 raz0red
*/

#include <fat.h>
#include <sdcard/wiisd_io.h>

#include "wii_app.h"

// Is the SD card mounted?
static BOOL sd_mounted = FALSE;

/*
 * Unmounts the SD card
 */
void wii_unmount_sd()
{
  if( sd_mounted )
  {
    fatUnmount( "sd:/" );
    __io_wiisd.shutdown();

    sd_mounted = FALSE;
  }
}

/*
 * Mounts the SD card
 *
 * return    Whether we mounted the SD card successfully
 */
BOOL wii_mount_sd()
{
  if( !sd_mounted )
  {
    if( !__io_wiisd.startup() )
    {
      return FALSE;
    }
    else if( !fatMountSimple( "sd", &__io_wiisd ) )
    {
      return FALSE;
    }
    else
    {
      //fatSetReadAhead( "sd:/", 6, 64 );
    }

    chdir( wii_get_app_path() );

    sd_mounted = TRUE;
  }

  return TRUE;
}
