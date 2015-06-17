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
  SDL_AudioCVT audio_convert;
  SDL_BuildAudioCVT(
    &audio_convert,
    AUDIO_U8,
    1,
    48000,
    AUDIO_S16MSB, 
    2,
    48000
    );

  audio_convert.buf = sample;
  audio_convert.len = length;
  SDL_ConvertAudio( &audio_convert );
  PlaySound( (u32*)sample, ( audio_convert.len_cvt / 4 ) );        

  wii_sound_length = length;
  wii_convert_length = audio_convert.len_cvt;
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
  
  int max = ( ( prosystem_frequency * prosystem_scanlines ) << 1 );
  while(targetIndex < length) {
    if(measurement >= max) {
      target[targetIndex++] = source[sourceIndex];
      measurement -= max;
    }
    else {
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

bool sound_Store( ) {

  if( sound_muted ) sound_SetMuted( false );

  byte sample[MAX_BUFFER_SIZE];
  memset( sample, 0, MAX_BUFFER_SIZE );  
  uint length = 48000 / prosystem_frequency; /* sound_GetSampleLength(sound_format.nSamplesPerSec, prosystem_frame, prosystem_frequency); */ /* 48000 / prosystem_frequency */
  sound_Resample(tia_buffer, sample, length);
  
  if(cartridge_pokey) {
    byte pokeySample[MAX_BUFFER_SIZE];
    memset( pokeySample, 0, MAX_BUFFER_SIZE );
    sound_Resample(pokey_buffer, pokeySample, length);
    for(uint index = 0; index < length; index++) {
      sample[index] += pokeySample[index];
      sample[index] = sample[index] / 2;
    }
  }  

  wii_storeSound( sample, length );  
     
  return true;
}

// ----------------------------------------------------------------------------
// Play
// ----------------------------------------------------------------------------
bool sound_Play( ) {
  byte sample[MAX_BUFFER_SIZE];
  memset( sample, 0, MAX_BUFFER_SIZE );
  wii_storeSound( sample, 1024 );
  //ResetAudio();
  return true;
}

// ----------------------------------------------------------------------------
// Stop
// ----------------------------------------------------------------------------
bool sound_Stop( ) {
  byte sample[MAX_BUFFER_SIZE];
  memset( sample, 0, MAX_BUFFER_SIZE );
  wii_storeSound( sample, 1024 );
  //StopAudio();
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

