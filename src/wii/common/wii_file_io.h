/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_FILE_IO_H
#define WII_FILE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>

/*
 * Unmounts the SD card
 */
extern void wii_unmount_sd();

/*
 * Mounts the SD card
 *
 * return   Whether we mounted the SD card successfully
 */
extern BOOL wii_mount_sd();

#ifdef __cplusplus
}
#endif

#endif
