/*
Wii7800 : Port of the ProSystem Emulator for the Wii

Copyright (C) 2010 raz0red
*/

#ifndef WII_ATARI_SNAPSHOT_H
#define WII_ATARI_SNAPSHOT_H

#include <gccore.h>

/*
 * Starts the emulator for the specified snapshot file.
 *
 * savefile The name of the save file to load. 
 */
extern BOOL wii_start_snapshot( char *savefile );

#endif
