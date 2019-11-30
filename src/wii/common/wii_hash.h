/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_HASH_H
#define WII_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>

/*
 * Computes the hash of the specified source
 *
 * source	The source to calculate the hash for
 * length	The length of the source
 * result	The string to receive the result of the hash computation
 */
void wii_hash_compute( const u8* source, u32 length, char result[33] );

#ifdef __cplusplus
}
#endif

#endif
