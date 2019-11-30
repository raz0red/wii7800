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
// Timer.cpp
// ----------------------------------------------------------------------------
#include "Timer.h"

#include <SDL.h>

typedef unsigned long long uInt64;

static uInt64 timer_currentTime;
static uInt64 timer_nextTime;
static uint timer_frameTime;

// ----------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------
void timer_Initialize( ) {
    timer_Reset();
}

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void timer_Reset( ) {
    timer_frameTime = (1000.0 / (double)prosystem_frequency) * 1000;
    timer_currentTime = ((uInt64)SDL_GetTicks()) * 1000;
    timer_nextTime = timer_currentTime + timer_frameTime;
}

// ----------------------------------------------------------------------------
// IsTime
// ----------------------------------------------------------------------------
bool timer_IsTime( ) {
    timer_currentTime = ((uInt64)SDL_GetTicks()) * 1000;

    if(timer_currentTime >= timer_nextTime) {
        timer_nextTime += timer_frameTime;
        return true;
    }
    return false;
}

