/*
Copyright (C) 2010 raz0red
*/

#include <stdio.h>
#include <string.h>

#include "wii_app.h"
#include "wii_config.h"
#include "wii_util.h"

/*
 * Returns the path to the config file
 *
 * return   The path to the config file
 */
static char* get_config_file_path()
{
  return WII_CONFIG_FILE;  
}

/*
 * Read the configuration file
 *
 * return   Whether we read the configuration file successfully
 */
BOOL wii_read_config()
{
  char buff[512];

  FILE *fp;
  fp = fopen( get_config_file_path(), "r" );
  if (fp == NULL) 
  {	
    return FALSE;		
  }

  while( fgets( buff, sizeof( buff ), fp ) ) 
  {
    char *ptr;
    Util_chomp( buff );
    ptr = strchr( buff, '=' );
    if( ptr != NULL ) 
    {
      *ptr++ = '\0';
      Util_trim( buff );
      Util_trim( ptr );

      // Read the value
      wii_config_handle_read_value( buff, ptr );
    }
  }

  fclose(fp);

  return TRUE;
}

/*
 * Write the configuration file
 *
 * return   Whether we wrote the configuration file successfully
 */
BOOL wii_write_config()
{
  FILE *fp;
  fp = fopen( get_config_file_path(), "w" );
  if( fp == NULL ) 
  {
    return FALSE;
  }

  // Write the configuration file
  wii_config_handle_write_config( fp );

  fclose(fp);

  return TRUE;
}