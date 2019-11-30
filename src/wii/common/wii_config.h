/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_CONFIG_H
#define WII_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <gctypes.h>

/*
 * Read the configuration file
 *
 * return   Whether we read the configuration file successfully
 */
extern BOOL wii_read_config();

/*
 * Write the configuration file
 *
 * return   Whether we wrote the configuration file successfully
 */
extern BOOL wii_write_config();

//
// Methods to be implemented by application
//

/*
 * Handles reading a particular configuration value
 *
 * name   The name of the config value
 * value  The config value
 */
extern void wii_config_handle_read_value( char* name, char* value );

/*
 * Handles the writing of the configuration file
 *
 * fp   The file pointer
 */
extern void wii_config_handle_write_config( FILE *fp );

#ifdef __cplusplus
}
#endif

#endif  
