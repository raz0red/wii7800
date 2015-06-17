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
