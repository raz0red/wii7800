// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ----------------------------------------------------------------------------
// Sound.h
// ----------------------------------------------------------------------------
#ifndef SOUND_H
#define SOUND_H

#define SOUND_LATENCY_NONE 0
#define SOUND_LATENCY_VERY_LOW 1
#define SOUND_LATENCY_LOW 2
#define SOUND_LATENCY_MEDIUM 3
#define SOUND_LATENCY_HIGH 4
#define SOUND_LATENCY_VERY_HIGH 5

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;

extern bool sound_Store( );
extern bool sound_CheckTimer();
extern bool sound_Play( );
extern bool sound_SetSampleRate(uint rate);
extern bool sound_Initialize();
extern bool sound_SetMuted(bool muted);

extern int wii_sound_length;
extern int wii_convert_length;

#endif
