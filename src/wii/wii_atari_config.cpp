/*--------------------------------------------------------------------------*\
|                                                                            |
|     __      __.__.___________  ______ _______  _______                     |
|    /  \    /  \__|__\______  \/  __  \\   _  \ \   _  \                    |
|    \   \/\/   /  |  |   /    />      </  /_\  \/  /_\  \                   |
|     \        /|  |  |  /    //   --   \  \_/   \  \_/   \                  |
|      \__/\  / |__|__| /____/ \______  /\_____  /\_____  /                  |
|           \/                        \/       \/       \/                   |
|                                                                            |
|    Wii7800 by raz0red                                                      |
|    Wii port of the ProSystem emulator developed by Greg Stanton            |
|                                                                            |
|    [github.com/raz0red/wii7800]                                            |
|                                                                            |
+----------------------------------------------------------------------------+
|                                                                            |
|    This program is free software; you can redistribute it and/or           |
|    modify it under the terms of the GNU General Public License             |
|    as published by the Free Software Foundation; either version 2          |
|    of the License, or (at your option) any later version.                  |
|                                                                            |
|    This program is distributed in the hope that it will be useful,         |
|    but WITHOUT ANY WARRANTY; without even the implied warranty of          |
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
|    GNU General Public License for more details.                            |
|                                                                            |
|    You should have received a copy of the GNU General Public License       |
|    along with this program; if not, write to the Free Software             |
|    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           |
|    02110-1301, USA.                                                        |
|                                                                            |
\*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "wii_config.h"
#include "wii_main.h"
#include "wii_util.h"

#include "wii_atari.h"

#include "networkop.h"

/**
 * Handles reading a particular configuration value
 *
 * @param   name The name of the config value
 * @param   value The config value
 */
void wii_config_handle_read_value(char* name, char* value) {
    if (strcmp(name, "debug") == 0) {
        wii_debug = Util_sscandec(value);
    } else if (strcmp(name, "max_frame_rate") == 0) {
        wii_max_frame_rate = Util_sscandec(value);
    } else if (strcmp(name, "top_menu_exit") == 0) {
        wii_top_menu_exit = Util_sscandec(value);
    } else if (strcmp(name, "vsync") == 0) {
        wii_vsync = Util_sscandec(value);
    } else if (strcmp(name, "diff_switch_display") == 0) {
        wii_diff_switch_display = Util_sscandec(value);
    } else if (strcmp(name, "diff_switch_enabled") == 0) {
        wii_diff_switch_enabled = Util_sscandec(value);
    } else if (strcmp(name, "swap_buttons") == 0) {
        wii_swap_buttons = Util_sscandec(value);
    } else if (strcmp(name, "high_score_cart") == 0) {
        wii_hs_mode = Util_sscandec(value);
    } else if (strcmp(name, "cart_wsync") == 0) {
        wii_cart_wsync = Util_sscandec(value);
    } else if (strcmp(name, "cart_cycle_stealing") == 0) {
        wii_cart_cycle_stealing = Util_sscandec(value);
    } else if (strcmp(name, "lightgun_crosshair") == 0) {
        wii_lightgun_crosshair = Util_sscandec(value);
    } else if (strcmp(name, "lightgun_flash") == 0) {
        wii_lightgun_flash = Util_sscandec(value);
    } else if (strcmp(name, "screen_x") == 0) {
        wii_screen_x = Util_sscandec(value);
    } else if (strcmp(name, "screen_y") == 0) {
        wii_screen_y = Util_sscandec(value);
    } else if (strcmp(name, "mote_menu_vertical") == 0) {
        wii_mote_menu_vertical = Util_sscandec(value);
    } else if (strcmp(name, "roms_dir") == 0) {
        wii_set_roms_dir(value);
    } else if (strcmp(name, "share_ip") == 0) {
        setSmbAddress(value);
    } else if (strcmp(name, "share_name") == 0) {
        setSmbShare(value);
    } else if (strcmp(name, "share_user") == 0) {
        setSmbUser(value);
    } else if (strcmp(name, "share_pass") == 0) {
        setSmbPassword(value);
    } else if (strcmp(name, "16_9_correct") == 0) {
        wii_16_9_correction = Util_sscandec(value);
    } else if (strcmp(name, "full_widescreen") == 0) {
        wii_full_widescreen = Util_sscandec(value);
    } else if (strcmp(name, "video_filter") == 0) {
        wii_filter = Util_sscandec(value);
    } else if (strcmp(name, "vi_gx_scaler") == 0) {
        wii_gx_vi_scaler = Util_sscandec(value);
    } else if (strcmp(name, "double_strike") == 0) {
        wii_double_strike_mode = Util_sscandec(value);
    } else if (strcmp(name, "trap_filter") == 0) {
        wii_trap_filter = Util_sscandec(value);
    }
}

/**
 * Handles the writing of the configuration file
 *
 * @param   fp The file pointer
 */
void wii_config_handle_write_config(FILE* fp) {
    fprintf(fp, "debug=%d\n", wii_debug);
    fprintf(fp, "max_frame_rate=%d\n", wii_max_frame_rate);
    fprintf(fp, "top_menu_exit=%d\n", wii_top_menu_exit);
    fprintf(fp, "vsync=%d\n", wii_vsync);
    fprintf(fp, "diff_switch_display=%d\n", wii_diff_switch_display);
    fprintf(fp, "swap_buttons=%d\n", wii_swap_buttons);
    fprintf(fp, "diff_switch_enabled=%d\n", wii_diff_switch_enabled);
    fprintf(fp, "high_score_cart=%d\n", wii_hs_mode);
    fprintf(fp, "cart_wsync=%d\n", wii_cart_wsync);
    fprintf(fp, "cart_cycle_stealing=%d\n", wii_cart_cycle_stealing);
    fprintf(fp, "lightgun_crosshair=%d\n", wii_lightgun_crosshair);
    fprintf(fp, "lightgun_flash=%d\n", wii_lightgun_flash);
    fprintf(fp, "screen_x=%d\n", wii_screen_x);
    fprintf(fp, "screen_y=%d\n", wii_screen_y);
    fprintf(fp, "mote_menu_vertical=%d\n", wii_mote_menu_vertical);
    fprintf(fp, "roms_dir=%s\n", wii_get_roms_dir());
    fprintf(fp, "share_ip=%s\n", getSmbAddress());
    fprintf(fp, "share_name=%s\n", getSmbShare());
    fprintf(fp, "share_user=%s\n", getSmbUser());
    fprintf(fp, "share_pass=%s\n", getSmbPassword());
    fprintf(fp, "16_9_correct=%d\n", wii_16_9_correction);
    fprintf(fp, "double_strike=%d\n", wii_double_strike_mode);
    fprintf(fp, "full_widescreen=%d\n", wii_full_widescreen);
    fprintf(fp, "video_filter=%d\n", wii_filter);
    fprintf(fp, "vi_gx_scaler=%d\n", wii_gx_vi_scaler);
    fprintf(fp, "trap_filter=%d\n", wii_trap_filter);
}