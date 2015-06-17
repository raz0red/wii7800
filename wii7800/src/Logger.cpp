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
// Logger.cpp
// ----------------------------------------------------------------------------
#include "Logger.h"
#define LOGGER_FILENAME "ProSystem.log"

byte logger_level = LOGGER_LEVEL_DEBUG;
//# if 0 //LUDO:
static FILE* logger_file = NULL;
//# else
//# include <stdio.h>
//# define logger_file stdout
//# endif

#ifdef LOWTRACE
bool wii_lowtrace = false;
#endif

#ifdef DEBUG

// ----------------------------------------------------------------------------
// GetTime
// ----------------------------------------------------------------------------
static std::string logger_GetTime( ) {
  time_t current;
  time(&current);  
  std::string timestring = ctime(&current);
  return timestring.erase(timestring.find_first_of("\n"), 1);
}

// ----------------------------------------------------------------------------
// Log
// ----------------------------------------------------------------------------
static void logger_Log(std::string message, byte level, std::string source) {
#ifdef LOWTRACE
  if( !wii_lowtrace )
  {
      return;
  }
#endif

  if(logger_file != NULL) {
    std::string entry = ""; /* "[" + logger_GetTime( ) + "]";
    switch(level) {
      case LOGGER_LEVEL_ERROR:
        entry += "[ERROR]";
        break;
      case LOGGER_LEVEL_INFO:
        entry += "[INFO ]";
        break;
      default:
        entry += "[DEBUG]";
        break;
    }
    entry += " " + message;
    if(source.length( ) > 0) {
      entry += " {" + source + "}";
    }
    */
    entry += message;
    entry += "\n";
    fwrite(entry.c_str( ), 1, entry.length( ), logger_file);
    fflush(logger_file);
  }
}

// ----------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------
bool logger_Initialize( ) {
  logger_file = fopen(LOGGER_FILENAME, "w");
  return (logger_file != NULL);
}

// ----------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------
bool logger_Initialize(std::string filename) {
  logger_file = fopen(filename.c_str( ), "w");
  return (logger_file != NULL);
}

// ----------------------------------------------------------------------------
// LogError
// ----------------------------------------------------------------------------
void logger_LogError(std::string message) {
  logger_LogError(message, "");
}

// ----------------------------------------------------------------------------
// LogError
// ----------------------------------------------------------------------------
void logger_LogError(std::string message, std::string source) {
  if(logger_level == LOGGER_LEVEL_ERROR || logger_level == LOGGER_LEVEL_INFO || logger_level == LOGGER_LEVEL_DEBUG) {
    logger_Log(message, LOGGER_LEVEL_ERROR, source);
  }
}

// ----------------------------------------------------------------------------
// LogInfo
// ----------------------------------------------------------------------------
void logger_LogInfo(std::string message) {
  logger_LogInfo(message, "");
}

// ----------------------------------------------------------------------------
// LogInfo
// ----------------------------------------------------------------------------
void logger_LogInfo(std::string message, std::string source) {
  if(logger_level == LOGGER_LEVEL_INFO || logger_level == LOGGER_LEVEL_DEBUG) {
    logger_Log(message, LOGGER_LEVEL_INFO, source);
  }
}

// ----------------------------------------------------------------------------
// LogDebug
// ----------------------------------------------------------------------------
void logger_LogDebug(std::string message) {
  logger_LogDebug(message, "");
}

// ----------------------------------------------------------------------------
// LogDebug
// ----------------------------------------------------------------------------
void logger_LogDebug(std::string message, std::string source) {
  if(logger_level == LOGGER_LEVEL_DEBUG) {
    logger_Log(message, LOGGER_LEVEL_DEBUG, source);
  }
}

// ----------------------------------------------------------------------------
// Release
// ----------------------------------------------------------------------------
void logger_Release( ) {
  if(logger_file != NULL) {
    fclose(logger_file);
  }
}
# endif
