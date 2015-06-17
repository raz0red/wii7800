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

#ifndef WII_ATARI_INPUT_H
#define WII_ATARI_INPUT_H

#include <wiiuse/wpad.h>

// In game controls
#define WII_BUTTON_ATARI_RESET ( WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS )
#define GC_BUTTON_ATARI_RESET ( PAD_BUTTON_START )

#define WII_BUTTON_ATARI_SELECT ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_BUTTON_ATARI_SELECT ( PAD_TRIGGER_L )

#define WII_BUTTON_ATARI_PAUSE ( WPAD_CLASSIC_BUTTON_ZL | WPAD_CLASSIC_BUTTON_ZR )
#define GC_BUTTON_ATARI_PAUSE ( PAD_TRIGGER_R )

#define WII_BUTTON_ATARI_RIGHT ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_ATARI_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_ATARI_UP ( WPAD_BUTTON_RIGHT )
#define GC_BUTTON_ATARI_UP ( PAD_BUTTON_UP )
#define WII_CLASSIC_ATARI_UP ( WPAD_CLASSIC_BUTTON_UP )
#define WII_BUTTON_ATARI_DOWN ( WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_DOWN )
#define GC_BUTTON_ATARI_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_ATARI_LEFT ( WPAD_BUTTON_UP )
#define WII_CLASSIC_ATARI_LEFT ( WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_ATARI_LEFT ( PAD_BUTTON_LEFT )

#define WII_BUTTON_ATARI_FIRE ( WPAD_BUTTON_2 )
#define GC_BUTTON_ATARI_FIRE ( PAD_BUTTON_A )
#define WII_CLASSIC_ATARI_FIRE ( WPAD_CLASSIC_BUTTON_A )
#define WII_NUNCHECK_ATARI_FIRE ( WPAD_NUNCHUK_BUTTON_C )

#define WII_CLASSIC_ATARI_FIRE_2 ( WPAD_CLASSIC_BUTTON_B )
#define WII_BUTTON_ATARI_FIRE_2 ( WPAD_BUTTON_1 )
#define GC_BUTTON_ATARI_FIRE_2 ( PAD_BUTTON_B )
#define WII_NUNCHECK_ATARI_FIRE_2 ( WPAD_NUNCHUK_BUTTON_Z )

#define WII_BUTTON_ATARI_DIFFICULTY_LEFT ( WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_FULL_L )
#define WII_BUTTON_ATARI_DIFFICULTY_LEFT_LG ( WPAD_BUTTON_1 | WPAD_CLASSIC_BUTTON_FULL_L )
#define GC_BUTTON_ATARI_DIFFICULTY_LEFT ( PAD_BUTTON_Y )

#define WII_BUTTON_ATARI_DIFFICULTY_RIGHT ( WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_FULL_R )
#define WII_BUTTON_ATARI_DIFFICULTY_RIGHT_LG ( WPAD_BUTTON_2 | WPAD_CLASSIC_BUTTON_FULL_R )
#define GC_BUTTON_ATARI_DIFFICULTY_RIGHT ( PAD_BUTTON_X )

#endif

