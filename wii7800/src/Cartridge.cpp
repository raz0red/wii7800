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
// Cartridge.cpp
// ----------------------------------------------------------------------------
#include "Cartridge.h"
#include "Region.h"
#include "wii_app_common.h"
#include "wii_atari.h"
#include <string.h>
#define CARTRIDGE_SOURCE "Cartridge.cpp"

std::string cartridge_title;
std::string cartridge_description;
std::string cartridge_year;
std::string cartridge_maker;
std::string cartridge_digest;
std::string cartridge_filename;
byte cartridge_type;
byte cartridge_region;
bool cartridge_pokey;
byte cartridge_controller[2];
byte cartridge_bank;
uint cartridge_flags;
int cartridge_crosshair_x;
int cartridge_crosshair_y;
bool cartridge_dualanalog = false;
uint cartridge_hblank = 34;

// Whether the cartridge has accessed the high score ROM (indicates that the
// SRAM should be persisted when the cartridge is unloaded)
bool high_score_set = false;
// Whether the high score cart has been loaded
static bool high_score_cart_loaded = false;

static byte* cartridge_buffer = NULL;
static uint cartridge_size = 0;

// ----------------------------------------------------------------------------
// HasHeader
// ----------------------------------------------------------------------------
static bool cartridge_HasHeader(const byte* header) {
  const char HEADER_ID[ ] = {"ATARI7800"};
  for(int index = 0; index < 9; index++) {
    if(HEADER_ID[index] != header[index + 1]) {
      return false;
    }
  }
  return true;
}

// 1.3

// ----------------------------------------------------------------------------
// Header for CC2 hack
// ----------------------------------------------------------------------------
static bool cartridge_CC2(const byte* header) {
  const char HEADER_ID[ ] = {">>"};
  for(int index = 0; index < 2; index++) {
    if(HEADER_ID[index] != header[index+1]) {
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
// GetBankOffset
// ----------------------------------------------------------------------------
static uint cartridge_GetBankOffset(byte bank) {
  return bank * 16384;
}

// ----------------------------------------------------------------------------
// WriteBank
// ----------------------------------------------------------------------------
static void cartridge_WriteBank(word address, byte bank) {
  uint offset = cartridge_GetBankOffset(bank);
  if(offset < cartridge_size) {
    memory_WriteROM(address, 16384, cartridge_buffer + offset);
    cartridge_bank = bank;
  }
}

// ----------------------------------------------------------------------------
// ReadHeader
// ----------------------------------------------------------------------------
static void cartridge_ReadHeader(const byte* header) {

  if( wii_debug )
  {
      fprintf( stderr, "reading cartridge header:\n" );
  }

  char temp[33] = {0};
  for(int index = 0; index < 32; index++) {
    temp[index] = header[index + 17];  
  }
  cartridge_title = temp;
    
  cartridge_size  = header[49] << 32; // Why 32?  
  cartridge_size |= header[50] << 16;
  cartridge_size |= header[51] << 8;
  cartridge_size |= header[52];

  if(header[53] == 0) {
    if(cartridge_size > 131072) {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_LARGE;
    }
    else if(header[54] == 2 || header[54] == 3) {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART;
    }
    else if(header[54] == 4 || header[54] == 5 || header[54] == 6 || header[54] == 7) {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_RAM;
    }
    else if(header[54] == 8 || header[54] == 9 || header[54] == 10 || header[54] == 11) {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_ROM;
    }
    else {
      cartridge_type = CARTRIDGE_TYPE_NORMAL;
    }
  }
  else {
    if(header[53] == 1) {
      cartridge_type = CARTRIDGE_TYPE_ABSOLUTE;
    }
    else if(header[53] == 2) {
      cartridge_type = CARTRIDGE_TYPE_ACTIVISION;
    }
    else {
      cartridge_type = CARTRIDGE_TYPE_NORMAL;
    }
  }
  
  cartridge_pokey = (header[54] & 1)? true: false;
  cartridge_controller[0] = header[55];
  cartridge_controller[1] = header[56];
  cartridge_region = header[57];
  cartridge_flags = 0;
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
static bool cartridge_Load(const byte* data, uint size) {
  if(size <= 128) {
    logger_LogError("Cartridge data is invalid.", CARTRIDGE_SOURCE);
    return false;
  }

  cartridge_Release( );
  
  byte header[128] = {0};
  for(int index = 0; index < 128; index++) {
    header[index] = data[index];
  }

  // 1.3
  if (cartridge_CC2(header)) {
    logger_LogError("Prosystem doesn't support CC2 hacks.", CARTRIDGE_SOURCE);
    return false;
  }

  uint offset = 0;
  if(cartridge_HasHeader(header)) {
    cartridge_ReadHeader(header);
    size -= 128;
    offset = 128;
  }
  else {
    cartridge_size = size;
  }
  
  cartridge_buffer = new byte[cartridge_size];
  for(int index = 0; index < cartridge_size; index++) {
    cartridge_buffer[index] = data[index + offset];
  }
  
  cartridge_digest = hash_Compute(cartridge_buffer, cartridge_size);
  return true;
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------

uint cartridge_Read(std::string filename, byte **outData ) {

  byte *data = NULL;
  uint size = archive_GetUncompressedFileSize(filename);
  if(size == 0) {
    FILE *file = fopen(filename.c_str( ), "rb");
    if(file == NULL) {
      logger_LogError("Failed to open the cartridge file " + filename + " for reading.", CARTRIDGE_SOURCE);
      return 0;  
    }

    if(fseek(file, 0L, SEEK_END)) {
      fclose(file);
      logger_LogError("Failed to find the end of the cartridge file.", CARTRIDGE_SOURCE);
      return 0;
    }
    size = ftell(file);
    if(fseek(file, 0L, SEEK_SET)) {
      fclose(file);
      logger_LogError("Failed to find the size of the cartridge file.", CARTRIDGE_SOURCE);
      return 0;
    }

    data = new byte[size];
    if(fread(data, 1, size, file) != size && ferror(file)) {
      fclose(file);
      logger_LogError("Failed to read the cartridge data.", CARTRIDGE_SOURCE);
      cartridge_Release( );
      delete [ ] data;
      return 0;
    }    

    fclose(file);    
  }
  else {
    data = new byte[size];
    archive_Uncompress(filename, data, size);
  }

  *outData = data;
  return size;
}

bool cartridge_Load(std::string filename) {
  if(filename.empty( ) || filename.length( ) == 0) {
    logger_LogError("Cartridge filename is invalid.", CARTRIDGE_SOURCE);
    return false;
  }

  cartridge_Release();

  logger_LogInfo("Opening cartridge file " + filename + ".");

  byte *data = NULL;
  uint size = cartridge_Read( filename, &data );
  if( data == NULL )
  {
      return false;
  }    
  
  if(!cartridge_Load(data, size)) {
    logger_LogError("Failed to load the cartridge data into memory.", CARTRIDGE_SOURCE);
    delete [ ] data;
    return false;
  }
  if(data != NULL) {
    delete [ ] data;
  }  
  cartridge_filename = filename;

  return true;
}

bool cartridge_Load_buffer(char* rom_buffer, int rom_size) {
  cartridge_Release();
  byte* data = (byte *)rom_buffer;
  uint size = rom_size;

  if(!cartridge_Load(data, size)) {
    return false;
  }
  cartridge_filename = "";
  return true;
}

// The memory location of the high score cartridge SRAM
#define HS_SRAM_START 0x1000
// The size of the high score cartridge SRAM
#define HS_SRAM_SIZE 2048

/*
 * Saves the high score cartridge SRAM
 *
 * return   Whether the save was successful
 */
bool cartridge_SaveHighScoreSram() 
{    
    if( !high_score_cart_loaded || !high_score_set )
    {
        // If we didn't load the high score cartridge, or the game didn't
        // access the high score ROM, don't save.
        return false;
    }

    std::string filename( WII_HIGH_SCORE_CART_SRAM );
    FILE* file = fopen( filename.c_str(), "wb" );
    if( file == NULL ) 
    {
        logger_LogError("Failed to open the file " + filename + " for writing.");
        return false;
    }

    if( fwrite( &(memory_ram[HS_SRAM_START]), 1, HS_SRAM_SIZE, file ) != HS_SRAM_SIZE ) 
    {
        fclose( file );
        logger_LogError("Failed to write highscore sram data to the file " + filename + ".");
        return false;
    }

    fflush(file);
    fclose(file);

    return true;
}

/*
 * Loads the high score cartridge SRAM
 *
 * return   Whether the load was successful
 */
static bool cartridge_LoadHighScoreSram() 
{    
    std::string filename( WII_HIGH_SCORE_CART_SRAM );
    FILE* file = fopen( filename.c_str(), "rb" );
    if( file == NULL ) 
    {
        return false;
    }

    byte sram[HS_SRAM_SIZE];
    if( fread( sram, 1, HS_SRAM_SIZE, file ) != HS_SRAM_SIZE ) 
    {
        fclose( file );
        logger_LogError("Failed to read highscore sram data from the file " + filename + ".");
        return false;
    }

    for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
    {
        memory_Write( HS_SRAM_START + i, sram[i] );
    }

    fclose(file);

    return true;
}

/*
 * Loads the high score cartridge
 *
 * return   Whether the load was successful
 */
bool cartridge_LoadHighScoreCart() {

    if( !wii_hs_enabled || cartridge_region != REGION_NTSC ) 
    {
        // Only load the cart if it is enabled and the region is NTSC
        return false;
    }

    byte* high_score_buffer = NULL;
    uint hsSize = cartridge_Read( WII_HIGH_SCORE_CART, &high_score_buffer );
    if( high_score_buffer != NULL )
    {
        logger_LogInfo("Found high score cartridge.");
        std::string digest = hash_Compute( high_score_buffer, hsSize );
        if( digest == std::string("c8a73288ab97226c52602204ab894286") ) 
        {
            cartridge_LoadHighScoreSram();
            for( uint i = 0; i < hsSize; i++ )
            {
                //memory_WriteROM( 0x3000, hsSize, high_score_buffer );
                memory_Write( 0x3000 + i, high_score_buffer[i] );
            }
            high_score_cart_loaded = true;
        }
        else
        {
            logger_LogError("High score cartridge hash is invalid.");
        }

        delete [] high_score_buffer;
        return high_score_cart_loaded;
    }
    else
    {
        logger_LogInfo("Unable to locate high score cartridge.");
    }

    return false;
}

// ----------------------------------------------------------------------------
// Store
// ----------------------------------------------------------------------------
void cartridge_Store( ) {
  switch(cartridge_type) {
    case CARTRIDGE_TYPE_NORMAL:
      memory_WriteROM(65536 - cartridge_size, cartridge_size, cartridge_buffer);
      break;
    case CARTRIDGE_TYPE_SUPERCART:
      if(cartridge_GetBankOffset(7) < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + cartridge_GetBankOffset(7));
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      if(cartridge_GetBankOffset(8) < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + cartridge_GetBankOffset(8));
        memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(0));
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_RAM:
      if(cartridge_GetBankOffset(7) < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + cartridge_GetBankOffset(7));
        memory_ClearROM(16384, 16384);
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      if(cartridge_GetBankOffset(7) < cartridge_size && cartridge_GetBankOffset(6) < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + cartridge_GetBankOffset(7));
        memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(6));
      }
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      memory_WriteROM(16384, 16384, cartridge_buffer);
      memory_WriteROM(32768, 32768, cartridge_buffer + cartridge_GetBankOffset(2));
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      if(122880 < cartridge_size) {
        memory_WriteROM(40960, 16384, cartridge_buffer);
        memory_WriteROM(16384, 8192, cartridge_buffer + 106496);
        memory_WriteROM(24576, 8192, cartridge_buffer + 98304);
        memory_WriteROM(32768, 8192, cartridge_buffer + 122880);
        memory_WriteROM(57344, 8192, cartridge_buffer + 114688);
      }
      break;
  }
}

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
void cartridge_Write(word address, byte data) {
  switch(cartridge_type) {
    case CARTRIDGE_TYPE_SUPERCART:
    case CARTRIDGE_TYPE_SUPERCART_RAM:
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      if(address >= 32768 && address < 49152 && data < 9) {
        cartridge_StoreBank(data);
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      if(address >= 32768 && address < 49152 && data < 9) {
        cartridge_StoreBank(data + 1);
      }
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      if(address == 32768 && (data == 1 || data == 2)) {
        cartridge_StoreBank(data - 1);
      }
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      if(address >= 65408) {
        cartridge_StoreBank(address & 7);
      }
      break;
  }

  if(cartridge_pokey && address >= 0x4000 && address <= 0x400f) {
    switch(address) {
      case POKEY_AUDF1:
        pokey_SetRegister(POKEY_AUDF1, data);
        break;
      case POKEY_AUDC1:
        pokey_SetRegister(POKEY_AUDC1, data);
        break;
      case POKEY_AUDF2:
        pokey_SetRegister(POKEY_AUDF2, data);
        break;
      case POKEY_AUDC2:
        pokey_SetRegister(POKEY_AUDC2, data);
        break;
      case POKEY_AUDF3:
        pokey_SetRegister(POKEY_AUDF3, data);
        break;
      case POKEY_AUDC3:
        pokey_SetRegister(POKEY_AUDC3, data);
        break;
      case POKEY_AUDF4:
        pokey_SetRegister(POKEY_AUDF4, data);
        break;
      case POKEY_AUDC4:
        pokey_SetRegister(POKEY_AUDC4, data);
        break;
      case POKEY_AUDCTL:
        pokey_SetRegister(POKEY_AUDCTL, data);
        break;
      case POKEY_SKCTLS:
        pokey_SetRegister(POKEY_SKCTLS, data);
        break;
    }
  }
}

// ----------------------------------------------------------------------------
// StoreBank
// ----------------------------------------------------------------------------
void cartridge_StoreBank(byte bank) {
  switch(cartridge_type) {
    case CARTRIDGE_TYPE_SUPERCART:
      cartridge_WriteBank(32768, bank);
      break;
    case CARTRIDGE_TYPE_SUPERCART_RAM:
      cartridge_WriteBank(32768, bank);
      break;
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      cartridge_WriteBank(32768, bank);
      break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      cartridge_WriteBank(32768, bank);        
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      cartridge_WriteBank(16384, bank);
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      cartridge_WriteBank(40960, bank);
      break;
  }  
}

// ----------------------------------------------------------------------------
// IsLoaded
// ----------------------------------------------------------------------------
bool cartridge_IsLoaded( ) {
  return (cartridge_buffer != NULL)? true: false;
}

// ----------------------------------------------------------------------------
// Release
// ----------------------------------------------------------------------------
void cartridge_Release( ) {
  high_score_cart_loaded = false;

  if(cartridge_buffer != NULL) {
    delete [ ] cartridge_buffer;
    cartridge_size = 0;
    cartridge_buffer = NULL;

    //
    // WII
    //
    // These values need to be reset so that moving between carts works
    // consistently. This seems to be a ProSystem emulator bug.
    //
    cartridge_type = 0;
    cartridge_region = 0;
    cartridge_pokey = 0;
    memset( cartridge_controller, 0, sizeof( cartridge_controller ) );
    cartridge_bank = 0;
    cartridge_flags = 0;
    cartridge_crosshair_x = 0;
    cartridge_crosshair_y = 0;    
    high_score_set = false;
    cartridge_hblank = 34;
    cartridge_dualanalog = false;
  }
}
