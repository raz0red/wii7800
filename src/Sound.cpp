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
// Sound.cpp
// ----------------------------------------------------------------------------
#include "Sound.h"
#include "ProSystem.h"
#include <SDL.h>
#include "wii_direct_sound.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

#define SOUND_SOURCE "Sound.cpp"

int wii_sound_length = 0;
int wii_convert_length = 0;

#define MAX_BUFFER_SIZE 8192

/* LUDO: */
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef struct { 
    WORD  wFormatTag; 
    WORD  nChannels; 
    DWORD nSamplesPerSec; 
    DWORD nAvgBytesPerSec; 
    WORD  nBlockAlign; 
    WORD  wBitsPerSample; 
    WORD  cbSize; 
} WAVEFORMATEX; 

# define WAVE_FORMAT_PCM 0

static const WAVEFORMATEX SOUND_DEFAULT_FORMAT = {WAVE_FORMAT_PCM, 1, 48000, 48000, 1, 8, 0};
static WAVEFORMATEX sound_format = SOUND_DEFAULT_FORMAT;
static bool sound_muted = false;

static void wii_storeSound( byte* sample, int length )
{
  PlaySound( (u8*)sample, length );        
}

// ----------------------------------------------------------------------------
// GetSampleLength
// ----------------------------------------------------------------------------
static uint sound_GetSampleLength(uint length, uint unit, uint unitMax) {
  uint sampleLength = length / unitMax;  
  uint sampleRemain = length % unitMax;
  if(sampleRemain != 0 && sampleRemain >= unit) {
    sampleLength++;
  }
  return sampleLength;
}

// ----------------------------------------------------------------------------
// Resample
// ----------------------------------------------------------------------------
static void sound_Resample(const byte* source, byte* target, int length) {
  int measurement = sound_format.nSamplesPerSec;
  int sourceIndex = 0;
  int targetIndex = 0;

  int max = ((prosystem_frequency * prosystem_scanlines) << 1);
  while (targetIndex < length) {
      if (measurement >= max) {
          target[targetIndex++] = source[sourceIndex];
          measurement -= max;
      } else {
          sourceIndex++;
          measurement += sound_format.nSamplesPerSec;
      }
  }
}


// ----------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------
bool sound_Initialize() {
  InitialiseAudio();
  return true;
}

// ----------------------------------------------------------------------------
// SetFormat
// ----------------------------------------------------------------------------
bool sound_SetFormat(WAVEFORMATEX format) {
  sound_format = format;
  return true;
}

// ----------------------------------------------------------------------------
// Store
// ----------------------------------------------------------------------------

byte sample[MAX_BUFFER_SIZE] = {0};
byte pokeySample[MAX_BUFFER_SIZE] = {0};

#ifdef TRACE_SOUND
static int maxTia = 0, minTia = 0, maxPokey = 0, minPokey = 0;
static u64 sumTia = 0, sumPokey = 0;
static int scount = 0;
static u64 stotalCount = 0;
#endif

bool sound_Store( ) {
  bool pokey =  (cartridge_pokey || xm_pokey_enabled);

  if( sound_muted ) sound_SetMuted( false );
  memset( sample, 0, MAX_BUFFER_SIZE );  
  uint length = 48000 / prosystem_frequency; /* sound_GetSampleLength(sound_format.nSamplesPerSec, prosystem_frame, prosystem_frequency); */ /* 48000 / prosystem_frequency */
  sound_Resample(tia_buffer, sample, length);
  tia_Clear(); // WII
  
  if(pokey) {    
    memset( pokeySample, 0, MAX_BUFFER_SIZE );
    sound_Resample(pokey_buffer, pokeySample, length);    
    for(uint index = 0; index < length; index++) {
#ifdef TRACE_SOUND      
      stotalCount++;      
      if (sample[index] > maxTia) maxTia = sample[index];
      if (sample[index] < minTia) minTia = sample[index];
      if (pokeySample[index] > maxPokey) maxPokey = pokeySample[index];
      if (pokeySample[index] < minPokey) minPokey = pokeySample[index];
      sumTia += sample[index];
      sumPokey += pokeySample[index];            
#endif    
      u32 sound = sample[index] + pokeySample[index];      
      sample[index] = sound >> 1;
    }
  } 
#ifdef TRACE_SOUND  
  else {
    for(uint index = 0; index < length; index++) {
      stotalCount++;
      sumTia += sample[index];      
      if (sample[index] > maxTia) maxTia = sample[index];
      if (sample[index] < minTia) minTia = sample[index];      
    }
  }
#endif  
  pokey_Clear(); // WII

  wii_storeSound( sample, length );

#ifdef TRACE_SOUND
  if (scount++ == 60) {
    net_print_string(NULL, 0, "Pokey: max %d, min %d, avg: %f (sum:%llu, count:%llu)\n", 
      maxPokey, minPokey, (sumPokey/(double)stotalCount), sumPokey, stotalCount);
    net_print_string(NULL, 0, "TIA: max %d, min %d, avg: %f (sum:%llu, count:%llu)\n", 
      maxTia, minTia, (sumTia/(double)stotalCount), sumTia, stotalCount);      
    scount = stotalCount = 0;
    maxPokey = minPokey = sumPokey = 0;
    maxTia = minTia = sumTia = 0;
  }  
#endif
     
  return true;
}

// ----------------------------------------------------------------------------
// Play
// ----------------------------------------------------------------------------
bool sound_Play( ) {  
  ResetAudio();
  return true;
}

// ----------------------------------------------------------------------------
// Stop
// ----------------------------------------------------------------------------
bool sound_Stop( ) {
  StopAudio();
  return true;
}

// ----------------------------------------------------------------------------
// SetSampleRate
// ----------------------------------------------------------------------------
bool sound_SetSampleRate(uint rate) {
  sound_format.nSamplesPerSec = rate;
  sound_format.nAvgBytesPerSec = rate;
  return sound_SetFormat(sound_format);
}

// ----------------------------------------------------------------------------
// GetSampleRate
// ----------------------------------------------------------------------------
uint sound_GetSampleRate( ) {
  return sound_format.nSamplesPerSec;
}

// ----------------------------------------------------------------------------
// SetMuted
// ----------------------------------------------------------------------------
bool sound_SetMuted(bool muted) {
  if(sound_muted != muted) {
    if(!muted) {
      if(!sound_Play( )) {
        return false;
      }
    }
    else {
      if(!sound_Stop( )) {
        return false;
      }
    }
    sound_muted = muted;
  }
  return true;
}

// ----------------------------------------------------------------------------
// IsMuted
// ----------------------------------------------------------------------------
bool sound_IsMuted( ) {
  return sound_muted;
}

