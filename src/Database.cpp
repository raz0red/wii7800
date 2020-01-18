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
// Database.cpp
// ----------------------------------------------------------------------------
#include "Database.h"
#include "Common.h"

#ifdef WII
#include "wii_main.h"
#include "wii_app.h"
#include "wii_app_common.h"
#endif

#define DATABASE_SOURCE "Database.cpp"

bool cart_in_db = false;
bool database_enabled = true;
std::string database_filename = "./prosystem.dat";

static std::string database_GetValue(std::string entry) {
  int index = entry.rfind('=');
  return entry.substr(index + 1);
}

#ifdef WII
char database_loc[WII_MAX_PATH] = "";
#endif

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool database_Load(std::string digest) {    
  cart_in_db = false;
  if(database_enabled) {
#ifndef WII
      FILE* file = fopen(database_filename.c_str(), "r");
#else
      if (database_loc[0] == '\0') {
          snprintf(database_loc, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                   WII_PROSYSTEM_DB);
      }
      FILE* file = fopen(database_loc, "r");
#endif
      if (file == NULL) {
          return false;
      }

      // max count of items in the database
      static int count = 17;

      bool found = false;
      char buffer[256];
      while (fgets(buffer, 256, file) != NULL) {
          std::string line = buffer;
          if (line.compare(1, 32, digest.c_str()) == 0) {
              found = true;
              cart_in_db = true;
              std::string entry[count];
              for (int index = 0; index < count; index++) {
                  buffer[0] = '\0';
                  fgets(buffer, 256, file);
                  entry[index] = common_Remove(buffer, '\r');
                  entry[index] = common_Remove(entry[index], '\n');
              }

              cartridge_title = database_GetValue(entry[0]); 
              cartridge_type = common_ParseByte(database_GetValue(entry[1]));
              cartridge_pokey = common_ParseBool(database_GetValue(entry[2]));
              cartridge_controller[0] =
                  common_ParseByte(database_GetValue(entry[3]));
              cartridge_controller[1] =
                  common_ParseByte(database_GetValue(entry[4]));
              cartridge_region = common_ParseByte(database_GetValue(entry[5]));
              cartridge_flags = common_ParseUint(database_GetValue(entry[6]));

              //
              // Optionally load the lightgun crosshair offsets, hblank, dual
              // analog
              //
              for (int index = 7; index < count; index++) {
                  if (entry[index].find("crossx") != std::string::npos) {
                      cartridge_crosshair_x =
                          common_ParseInt(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("crossy") != std::string::npos) {
                      cartridge_crosshair_y =
                          common_ParseInt(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("hblank") != std::string::npos) {
                      cartridge_hblank =
                          common_ParseInt(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("dualanalog") != std::string::npos) {
                      cartridge_dualanalog =
                          common_ParseBool(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("pokey450") != std::string::npos) {
                      cartridge_pokey450 =
                          common_ParseBool(database_GetValue(entry[index]));
                      if (cartridge_pokey450) {
                          cartridge_pokey = true;
                      }
                  }
                  if (entry[index].find("xm") != std::string::npos) {
                      cartridge_xm =
                          common_ParseBool(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("disablebios") != std::string::npos) {
                      cartridge_disable_bios =
                          common_ParseBool(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("leftswitch") != std::string::npos) {
                      cartridge_left_switch =
                          common_ParseByte(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("rightswitch") != std::string::npos) {
                      cartridge_right_switch =
                          common_ParseByte(database_GetValue(entry[index]));
                  }
                  if (entry[index].find("swapbuttons") != std::string::npos) {
                      cartridge_swap_buttons =
                          common_ParseBool(database_GetValue(entry[index]));
                  }
              }
              break;
          }
      }

      if (wii_debug && !found) {
          fprintf(stderr, "unable to locate cartridge in database.\n");
      }

      fclose(file);  
  }
  return true;
}


