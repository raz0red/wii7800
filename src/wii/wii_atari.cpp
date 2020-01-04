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

#include "Database.h"
#include "Sound.h"
#include "Timer.h"
#ifdef DEBUG
#include "Logger.h"
#endif

#include <gccore.h>

#include "font_ttf.h"

#include "wii_config.h"
#include "wii_gx.h"
#include "wii_hw_buttons.h"
#include "wii_input.h"
#include "wii_sdl.h"
#include "wii_main.h"
#include "wii_app.h"

#include "vi_encoder.h"

#include "wii_app_common.h"
#include "wii_atari.h"
#include "wii_atari_emulation.h"
#include "wii_atari_input.h"
#include "wii_atari_sdl.h"

// The size of the crosshair
#define CROSSHAIR_SIZE 11
// The offset from the center of the crosshair
#define CROSSHAIR_OFFSET 5
// The amount of time (in seconds) to display the difficulty switches when
// they are changed
#define DIFF_DISPLAY_LENGTH 5

/** The palette (8-bit) */
byte atari_pal8[256] = {0};
/** Whether to flash the screen */
BOOL wii_lightgun_flash = TRUE;
/** Whether to display a crosshair for the lightgun */
BOOL wii_lightgun_crosshair = TRUE;
/** Whether wsync is enabled/disabled */
u8 wii_cart_wsync = CART_MODE_AUTO;
/** Whether cycle stealing is enabled/disabled */
u8 wii_cart_cycle_stealing = CART_MODE_AUTO;
/** Whether high score cart is enabled */
BOOL wii_hs_enabled = TRUE;
/** What mode the high score cart is in */
BOOL wii_hs_mode = HSMODE_ENABLED_NORMAL;
/** Whether to swap buttons */
BOOL wii_swap_buttons = FALSE;
/** If the difficulty switches are enabled */
BOOL wii_diff_switch_enabled = FALSE;
/** When to display the difficulty switches */
BOOL wii_diff_switch_display = DIFF_SWITCH_DISPLAY_WHEN_CHANGED;
/** The screen X size */
int wii_screen_x = DEFAULT_SCREEN_X;
/** The screen Y size */
int wii_screen_y = DEFAULT_SCREEN_Y;
/** Whether to display debug info (FPS, etc.) */
short wii_debug = 0;
/** The maximum frame rate */
int wii_max_frame_rate = 0;
/** Whether to filter the display */
BOOL wii_filter = FALSE;
/** Whether to use the GX/VI scaler */
BOOL wii_gx_vi_scaler = TRUE;

/** The 7800 scanline that the lightgun is currently at */
int lightgun_scanline = 0;
/** The 7800 cycle that the lightgun is currently at */
float lightgun_cycle = 0;
/** Whether the lightgun is enabled for the current cartridge */
bool lightgun_enabled = false; 
/** Tracks the first time the lightgun is fired for the current cartridge */
bool lightgun_first_fire = true;

/** Whether this is a test frame */
bool wii_testframe = false;

/** Whether the left difficulty switch is on */
static bool left_difficulty_down = false;
/** Whether the right difficulty switch is on */
static bool right_difficulty_down = false;
/** The keyboard (controls) data */
static unsigned char keyboard_data[19];
/** The amount of time to wait before reading the difficulty switches */
static int diff_wait_count = 0;
/** The amount of time left to display the difficulty switch values */
static int diff_display_count = 0;
/** The current roms directory */
static char roms_dir[WII_MAX_PATH] = "";
/** The saves directory */
static char saves_dir[WII_MAX_PATH] = "";

/** The x location of the Wiimote (IR) */
int wii_ir_x = -100;
/** The y location of the Wiimote (IR) */
int wii_ir_y = -100;

// Forward reference
static void wii_atari_display_crosshairs(int x, int y, BOOL erase);

// Initializes the menu
extern void wii_atari_menu_init();

extern "C" {
void WII_SetFilter(BOOL filter);
void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);
void WII_SetRenderCallback(void (*cb)(void));
void WII_SetWidescreen(int wide);
void WII_SetFilter(BOOL filter);
void WII_SetDefaultVideoMode();
void WII_SetStandardVideoMode(int xscale, int yscale, int width);
void WII_SetDoubleStrikeVideoMode(int xscale, int yscale, int width);
void WII_SetInterlaceVideoMode(int xscale, int yscale, int width);
void WII_VideoStop();
void WII_VideoStart();
}

//
// For debug output
//

extern int hs_sram_write_count;
extern unsigned int riot_timer_count;
extern byte riot_drb;
extern byte RANDOM;
extern uint dbg_saved_cycles;
extern uint dbg_wsync_count;
extern uint dbg_maria_cycles;
extern uint dbg_p6502_cycles;
extern bool dbg_wsync;
extern bool dbg_cycle_stealing;

static float wii_fps_counter;
static int wii_dbg_scanlines;

/**
 * Returns the current roms directory
 *
 * @return  The current roms directory
 */
char* wii_get_roms_dir() {
    return roms_dir;
}

/**
 * Sets the current roms directory
 *
 * @param   newDir The new roms directory
 */
void wii_set_roms_dir(const char* newDir) {
    Util_strlcpy(roms_dir, newDir, sizeof(roms_dir));
#ifdef WII_NETTRACE
    net_print_string(NULL, 0, "RomsDir: \"%s\"\n", roms_dir);
#endif
}

/**
 * Returns the saves directory
 *
 * @return  The saves directory
 */
char* wii_get_saves_dir() {
    if (saves_dir[0] == '\0') {
        snprintf(saves_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 WII_SAVES_DIR);
    }
    return saves_dir;
}

/**
 * Returns the base directory for the application
 *
 * @return  The base directory for the application
 */
const char* wii_get_app_base_dir() {
    return WII_BASE_APP_DIR;
}

/**
 * Returns the location of the config file
 *
 * @return  The location of the config file
 */
const char* wii_get_config_file_path() {
    return WII_CONFIG_FILE;
}

/**
 * Returns the location of the data directory
 *
 * @return  The location of the data directory
 */
const char* wii_get_data_path() {
    return WII_FILES_DIR;
}

/**
 * Initializes the application
 */
void wii_handle_init() {
    logger_Initialize();

    wii_read_config();

    // Startup the SDL
    if (!wii_sdl_init()) {
        fprintf(stderr, "FAILED : Unable to init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // FreeTypeGX
    InitFreeType((uint8_t*)font_ttf, (FT_Long)font_ttf_size);

    sound_Initialize();
    sound_SetMuted(true);

    // Initialize the Atari menu
    wii_atari_menu_init();
}

/**
 * Frees resources prior to the application exiting
 */
void wii_handle_free_resources() {
    wii_write_config();
    wii_sdl_free_resources();

    SDL_Quit();

#ifdef DEBUG
    logger_Release();
#endif
}

/**
 * Runs the application (main loop)
 */
void wii_handle_run() {
    // Start specified rom (if applicable)
    if (wii_initial_rom[0] != '\0') {
        wii_start_emulation(wii_initial_rom, "", FALSE, FALSE);
    }

    // Show the menu (starts the menu loop)
    wii_menu_show();
}

/**
 * Initializes the 8-bit palette
 */
static void wii_atari_init_palette8() {
    const byte* palette;
    if (cartridge_region == REGION_PAL) {
        palette = REGION_PALETTE_PAL;
    } else {
        palette = REGION_PALETTE_NTSC;
    }

    for (uint index = 0; index < 256; index++) {
        word r = palette[(index * 3) + 0];
        word g = palette[(index * 3) + 1];
        word b = palette[(index * 3) + 2];
        atari_pal8[index] = wii_sdl_rgb(r, g, b);
    }
}

/**
 * Pauses the emulator
 *
 * @param   pause Whether to pause or resume
 */
void wii_atari_pause(bool pause) {
    sound_SetMuted(pause);
    prosystem_Pause(pause);

    if (!pause) {
        timer_Reset();
    }
}

/**
 * Resets the keyboard (control) information
 */
void wii_reset_keyboard_data() {
    memset(keyboard_data, 0, sizeof(keyboard_data));

    // Left difficulty switch defaults to off
    keyboard_data[15] = 1;
    left_difficulty_down = false;

    // Right difficulty swtich defaults to on
    keyboard_data[16] = 0;
    right_difficulty_down = true;

    diff_wait_count = prosystem_frequency * 0.75;
    diff_display_count = 0;
}

/**
 * Loads the specified ROM
 *
 * @param   filename The filename of the ROM
 * @param   loadbios Whether or not to load the Atari BIOS
 * @return  Whether the load was successful
 */
bool wii_atari_load_rom(char* filename, bool loadbios) {
    std::string std_filename(filename);
    if (!cartridge_Load(std_filename))
        return false;

    database_Load(cartridge_digest);

    bios_enabled = false;
    if (loadbios) {
        char boot_rom[WII_MAX_PATH];
        snprintf(boot_rom, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 (cartridge_region == REGION_PAL ? WII_ROOT_BOOT_ROM_PAL
                                                 : WII_ROOT_BOOT_ROM_NTSC));

        if (bios_Load(boot_rom)) {
            bios_enabled = true;
        } else {
            bios_enabled = false;
        }
    }

    wii_reset_keyboard_data();
    wii_atari_init_palette8();
    prosystem_Reset();

    wii_atari_pause(false);

    return true;
}

/**
 * Renders the current frame to the Wii
 */
void wii_atari_put_image_gu_normal() {
    int atari_height =
        (cartridge_region == REGION_PAL ? PAL_ATARI_HEIGHT : NTSC_ATARI_HEIGHT);
    int atari_offsety =
        (cartridge_region == REGION_PAL ? PAL_ATARI_BLIT_TOP_Y
                                        : NTSC_ATARI_BLIT_TOP_Y);

    int offsetx = (WII_WIDTH - ATARI_WIDTH) / 2;
    int offsety = (WII_HEIGHT - atari_height) / 2;

    int src = 0, dst = 0, start = 0, x = 0, y = 0;
    byte* backpixels = (byte*)back_surface->pixels;
    byte* blitpixels = (byte*)blit_surface->pixels;
    int startoffset = atari_offsety * ATARI_WIDTH;
    for (y = 0; y < atari_height; y++) {
        start = startoffset + (y * ATARI_WIDTH);
        src = 0;
        dst = ((y  + offsety) * WII_WIDTH) + offsetx;
        for (x = 0; x < ATARI_WIDTH; x++) {
            backpixels[dst++] = blitpixels[start + src];
            src++;
        }
    }
}

/**
 * Displays the Atari difficulty switch settings
 */
static void wii_atari_display_diff_switches() {
    if (diff_display_count > 0) {
        diff_display_count--;
    }

    if ((wii_diff_switch_display == DIFF_SWITCH_DISPLAY_ALWAYS) ||
        ((wii_diff_switch_display == DIFF_SWITCH_DISPLAY_WHEN_CHANGED) &&
         diff_display_count > 0)) {
        GXColor red = (GXColor){0xff, 0x00, 0x00, 0xff};
        GXColor black = (GXColor){0x00, 0x00, 0x00, 0xff};

        // wii_sdl_draw_rectangle( 9, 444, 22, 10, 0x0 );
        wii_gx_drawrectangle(-26, 
            (wii_double_strike_mode ? -102 : -204), 22, 
            (wii_double_strike_mode ? 5 : 10), black, TRUE);
        if (!keyboard_data[15]) {
            // wii_sdl_fill_rectangle( 10, 445, 20, 8, color );
            wii_gx_drawrectangle(-25, 
                (wii_double_strike_mode ? -103 : -205) , 20, 
                (wii_double_strike_mode ? 4 : 8), red, TRUE);
        } else {
            // wii_sdl_draw_rectangle( 10, 445, 20, 8, color );
            wii_gx_drawrectangle(-25, 
                (wii_double_strike_mode ? -103 : -205), 20, 
                (wii_double_strike_mode ? 4 : 8), red, FALSE);
        }

        // wii_sdl_draw_rectangle( 39, 444, 22, 10, 0x0 );
        wii_gx_drawrectangle(4, 
            (wii_double_strike_mode ? -102 : -204), 22, 
            (wii_double_strike_mode ? 5 : 10), black, TRUE);
        if (!keyboard_data[16]) {
            // wii_sdl_fill_rectangle( 40, 445, 20, 8, color );
            wii_gx_drawrectangle(5, 
                (wii_double_strike_mode ? -103 : -205), 20, 
                (wii_double_strike_mode ? 4 : 8), red, TRUE);
        } else {
            // wii_sdl_draw_rectangle( 40, 445, 20, 8, color );
            wii_gx_drawrectangle(5, 
                (wii_double_strike_mode ? -103 : -205), 20, 
                (wii_double_strike_mode ? 4 : 8), red, FALSE);
        }
    }
}

/**
 * Refreshes the Wii display
 *
 * @param   sync Whether vsync is available for the current frame
 * @param   testframes The number of testframes to run (for loading saves)
 */
static void wii_atari_refresh_screen(bool sync, int testframes) {
    if (diff_wait_count > 0) {
        // Reduces the number of frames remaining to display the difficulty
        // switches.
        diff_wait_count--;
    }

    BOOL drawcrosshair = lightgun_enabled && wii_lightgun_crosshair;
    if (drawcrosshair) {
        // Display the crosshairs
        wii_atari_display_crosshairs(wii_ir_x, wii_ir_y, FALSE);
    }

    wii_atari_put_image_gu_normal();

    if (drawcrosshair) {
        // Erase the crosshairs
        wii_atari_display_crosshairs(wii_ir_x, wii_ir_y, TRUE);
    }

    if (sync) {
#if 0        
        wii_sync_video();
#else
        // TODO: Evaluate how to evaluate this 
        VIDEO_WaitVSync();
#endif
    }

    if (testframes < 0) {
        wii_sdl_flip();
    }
}

/**
 * Displays the crosshairs for the lightgun
 *
 * @param   x The x location
 * @param   y The y location
 * @param   erase Whether we are erasing the crosshairs
 */
static void wii_atari_display_crosshairs(int x, int y, BOOL erase) {
    if (x < 0 || y < 0)
        return;

    uint color = (erase ? wii_sdl_rgb(0, 0, 0) : wii_sdl_rgb(0xff, 0xff, 0xff));

    int cx = (x - CROSSHAIR_OFFSET) + cartridge_crosshair_x;
    int cy = (y - CROSSHAIR_OFFSET) + cartridge_crosshair_y;

    float xratio = (float)ATARI_WIDTH / (float)WII_WIDTH;
    float yratio = (float)NTSC_ATARI_HEIGHT / (float)WII_HEIGHT;

    float x0 = 0;
    float y0 = 0;

    cx = x0 + (cx * xratio);
    cy = y0 + (cy * yratio);

    wii_sdl_draw_rectangle(blit_surface, cx, cy + CROSSHAIR_OFFSET,
                           CROSSHAIR_SIZE, 1, color, !erase);

    wii_sdl_draw_rectangle(blit_surface, cx + CROSSHAIR_OFFSET, cy, 1,
                           CROSSHAIR_SIZE, color, !erase);
}

/**
 * Stores the current location of the Wiimote (IR)
 */
static void wii_atari_update_wiimote_ir() {
    // Necessary as the SDL seems to keep resetting the resolution
    WPAD_SetVRes(WPAD_CHAN_0, 640, 480);

    ir_t ir;
    WPAD_IR(WPAD_CHAN_0, &ir);

    if (ir.valid) {
        wii_ir_x = ir.x;
        wii_ir_y = ir.y;
    } else {
        wii_ir_x = -100;
        wii_ir_y = -100;
    }
}

// The number of cycles per scanline that the 7800 checks for a hit
#define LG_CYCLES_PER_SCANLINE 318
// The number of cycles indented (after HBLANK) prior to checking for a hit
#define LG_CYCLES_INDENT 52

/**
 * Updates the joystick state
 *
 * @param   joyIndex The joystick index
 * @param   keyboard_data The keyboard (controls) state
 */
static void wii_atari_update_joystick(int joyIndex,
                                      unsigned char keyboard_data[19]) {
    // Check the state of the controllers
    u32 down = WPAD_ButtonsDown(joyIndex);
    u32 held = WPAD_ButtonsHeld(joyIndex);
    u32 gcDown = PAD_ButtonsDown(joyIndex);
    u32 gcHeld = PAD_ButtonsHeld(joyIndex);

    // Check to see if the lightgun is enabled (lightgun only works for
    // joystick index 0).
    bool lightgun = (lightgun_enabled && (joyIndex == 0));

    if (lightgun) {
        // Determine the Y offset of the lightgun location
        int yoffset =
            (cartridge_region == REGION_NTSC ? (NTSC_ATARI_BLIT_TOP_Y)
                                             : (PAL_ATARI_BLIT_TOP_Y - 28));

        // The number of scanlines for the current cartridge
        int scanlines = (cartridge_region == REGION_NTSC ? NTSC_ATARI_HEIGHT
                                                         : PAL_ATARI_HEIGHT);
        wii_dbg_scanlines = scanlines;

        // We track the first time the lightgun is fired due to the fact that
        // when a catridge is launched (via the Wii7800 menu) the state of the
        // fire button (down) is used to determine whether the console has a
        // joystick or lightgun plugged in.
        if (lightgun_first_fire) {
            if (!(held & (WPAD_BUTTON_B | WPAD_BUTTON_A))) {
                // The button is not down, enable lightgun firing.
                lightgun_first_fire = false;
            }
            keyboard_data[3] = true;
        } else {
            keyboard_data[3] = !(held & (WPAD_BUTTON_B | WPAD_BUTTON_A));
        }

        //
        // TODO: These values should be cached
        //
        float yratio = ((float)scanlines / (float)WII_HEIGHT);
        float xratio = ((float)LG_CYCLES_PER_SCANLINE / (float)WII_WIDTH);
        lightgun_scanline =
            (((float)wii_ir_y * yratio) +
             (maria_visibleArea.top - maria_displayArea.top + 1) + yoffset);
        lightgun_cycle =
            (HBLANK_CYCLES + LG_CYCLES_INDENT + ((float)wii_ir_x * xratio));
        if (lightgun_cycle > CYCLES_PER_SCANLINE) {
            lightgun_scanline++;
            lightgun_cycle -= CYCLES_PER_SCANLINE;
        }
    } else {
        expansion_t exp;
        WPAD_Expansion(joyIndex, &exp);
        bool isClassic = (exp.type == WPAD_EXP_CLASSIC);

        float expX = wii_exp_analog_val(&exp, TRUE, FALSE);
        float expY = wii_exp_analog_val(&exp, FALSE, FALSE);
        s8 gcX = PAD_StickX(joyIndex);
        s8 gcY = PAD_StickY(joyIndex);

        float expRjsX = 0, expRjsY = 0;
        s8 gcRjsX = 0, gcRjsY = 0;

        // Dual analog support
        if (cartridge_dualanalog && joyIndex == 1) {
            expansion_t exp0;
            WPAD_Expansion(0, &exp0);
            if (exp0.type == WPAD_EXP_CLASSIC) {
                expRjsX = wii_exp_analog_val(&exp0, TRUE, TRUE);
                expRjsY = wii_exp_analog_val(&exp0, FALSE, TRUE);
            }

            gcRjsX = PAD_SubStickX(0);
            gcRjsY = PAD_SubStickY(0);
        }

        int offset = (joyIndex == 0 ? 0 : 6);

        // | 00 06     | Joystick 1 2 | Right
        keyboard_data[0 + offset] =
            (held & WII_BUTTON_ATARI_RIGHT || gcHeld & GC_BUTTON_ATARI_RIGHT ||
             wii_analog_right(expX, gcX) || wii_analog_right(expRjsX, gcRjsX));
        // | 01 07     | Joystick 1 2 | Left
        keyboard_data[1 + offset] =
            (held & (WII_BUTTON_ATARI_LEFT |
                     (isClassic ? WII_CLASSIC_ATARI_LEFT : 0)) ||
             gcHeld & GC_BUTTON_ATARI_LEFT || wii_analog_left(expX, gcX) ||
             wii_analog_left(expRjsX, gcRjsX));
        // | 02 08     | Joystick 1 2 | Down
        keyboard_data[2 + offset] =
            (held & WII_BUTTON_ATARI_DOWN || gcHeld & GC_BUTTON_ATARI_DOWN ||
             wii_analog_down(expY, gcY) || wii_analog_down(expRjsY, gcRjsY));
        // | 03 09     | Joystick 1 2 | Up
        keyboard_data[3 + offset] =
            (held & (WII_BUTTON_ATARI_UP |
                     (isClassic ? WII_CLASSIC_ATARI_UP : 0)) ||
             gcHeld & GC_BUTTON_ATARI_UP || wii_analog_up(expY, gcY) ||
             wii_analog_up(expRjsY, gcRjsY));
        // | 04 10     | Joystick 1 2 | Button 1
        keyboard_data[wii_swap_buttons ? 4 + offset : 5 + offset] =
            (held & (WII_BUTTON_ATARI_FIRE |
                     (isClassic ? WII_CLASSIC_ATARI_FIRE
                                : WII_NUNCHECK_ATARI_FIRE)) ||
             gcHeld & GC_BUTTON_ATARI_FIRE);
        // | 05 11     | Joystick 1 2 | Button 2
        keyboard_data[wii_swap_buttons ? 5 + offset : 4 + offset] =
            (held & (WII_BUTTON_ATARI_FIRE_2 |
                     (isClassic ? WII_CLASSIC_ATARI_FIRE_2
                                : WII_NUNCHECK_ATARI_FIRE_2)) ||
             gcHeld & GC_BUTTON_ATARI_FIRE_2);
    }

    if (joyIndex == 0) {
        // | 12       | Console      | Reset
        keyboard_data[12] =
            (held & WII_BUTTON_ATARI_RESET || gcHeld & GC_BUTTON_ATARI_RESET);
        // | 13       | Console      | Select
        keyboard_data[13] =
            (held & WII_BUTTON_ATARI_SELECT || gcHeld & GC_BUTTON_ATARI_SELECT);
        // | 14       | Console      | Pause
        keyboard_data[14] =
            (held & WII_BUTTON_ATARI_PAUSE || gcHeld & GC_BUTTON_ATARI_PAUSE);

        if (wii_diff_switch_enabled) {
            // | 15       | Console      | Left Difficulty
            if ((diff_wait_count == 0) &&
                ((gcDown & GC_BUTTON_ATARI_DIFFICULTY_LEFT) ||
                 ((!lightgun && (down & WII_BUTTON_ATARI_DIFFICULTY_LEFT)) ||
                  (lightgun &&
                   (down & WII_BUTTON_ATARI_DIFFICULTY_LEFT_LG))))) {
                if (!left_difficulty_down) {
                    keyboard_data[15] = !keyboard_data[15];
                    left_difficulty_down = true;
                    diff_display_count =
                        prosystem_frequency * DIFF_DISPLAY_LENGTH;
                }
            } else {
                left_difficulty_down = false;
            }
            // | 16       | Console      | Right Difficulty
            if ((diff_wait_count == 0) &&
                ((gcDown & GC_BUTTON_ATARI_DIFFICULTY_RIGHT) ||
                 ((!lightgun && (down & WII_BUTTON_ATARI_DIFFICULTY_RIGHT)) ||
                  (lightgun &&
                   (down & WII_BUTTON_ATARI_DIFFICULTY_RIGHT_LG))))) {
                if (!right_difficulty_down) {
                    keyboard_data[16] = !keyboard_data[16];
                    right_difficulty_down = true;
                    diff_display_count =
                        prosystem_frequency * DIFF_DISPLAY_LENGTH;
                }
            } else {
                right_difficulty_down = false;
            }
        }

        if ((down & WII_BUTTON_HOME) || (gcDown & GC_BUTTON_HOME) ||
            wii_hw_button) {
            wii_atari_pause(true);
        }
    }
}

/**
 * Updates the Atari keys (controls) state
 *
 * @param   keyboard_data The keyboard (controls) state
 */
static void wii_atari_update_keys(unsigned char keyboard_data[19]) {
    WPAD_ScanPads();
    PAD_ScanPads();

    if (lightgun_enabled) {
        wii_atari_update_wiimote_ir();
    }
    wii_atari_update_joystick(0, keyboard_data);
    wii_atari_update_joystick(1, keyboard_data);
}

extern Mtx gx_view;

/**
 * GX render callback
 */
void wii_render_callback() {
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);

    Mtx m;   // model matrix.
    Mtx mv;  // modelview matrix.

    guMtxIdentity(m);
    guMtxTransApply(m, m, 0, 0, -100);
    guMtxConcat(gx_view, m, mv);
    GX_LoadPosMtxImm(mv, GX_PNMTX0);

    // Diff switches
    wii_atari_display_diff_switches();

    //
    // Debug
    //

    static int dbg_count = 0;

    if (wii_debug && !wii_testframe && !wii_double_strike_mode && !wii_gx_vi_scaler) {
        static char text[256] = "";
        static char text2[256] = "";
        dbg_count++;

        int pixelSize = 12;
        int h = pixelSize;
        int padding = 2;

        if (dbg_count % 60 == 0) {
            /* a: %d, %d, c: 0x%x,0x%x,0x%x*/
            /* wii_sound_length, wii_convert_length, memory_ram[CTLSWB],
             * riot_drb, memory_ram[SWCHB] */
            sprintf(text,
                    "v: %.2f, hs: %d, %d, timer: %d, wsync: %s, %d, stl: %s, "
                    "mar: %d, cpu: %d, ext: %d, rnd: %d, hb: %d",
                    wii_fps_counter, high_score_set, hs_sram_write_count,
                    (riot_timer_count % 1000), (dbg_wsync ? "1" : "0"),
                    dbg_wsync_count, (dbg_cycle_stealing ? "1" : "0"),
                    dbg_maria_cycles, dbg_p6502_cycles, dbg_saved_cycles,
                    RANDOM, cartridge_hblank);
        }

        GXColor color = (GXColor){0x0, 0x0, 0x0, 0x80};
        int w = wii_gx_gettextwidth(pixelSize, text);
        int x = -310;
        int y = 196;

        wii_gx_drawrectangle(x + -padding, y + h + padding, w + (padding << 1),
                             h + (padding << 1), color, TRUE);

        wii_gx_drawtext(x, y, pixelSize, text, ftgxWhite, FTGX_ALIGN_BOTTOM);

        if (lightgun_enabled) {
            // ir_t ir;
            // WPAD_IR(WPAD_CHAN_0, &ir);

            sprintf( text2, 
        "lightgun: %d, %d, %d, %.2f, %d, [%d, %d]", 
        /*"lightgun: %d, %d, %d, %.2f, %d, [%d, %d] %d, %d, %d, %d", */
        cartridge_crosshair_x, cartridge_crosshair_y,
        lightgun_scanline, lightgun_cycle, wii_dbg_scanlines, 
        wii_ir_x, wii_ir_y /*,
        ir.vres[0], ir.vres[1], ir.offset[0], ir.offset[1]*/ );

            int w = wii_gx_gettextwidth(pixelSize, text2);
            int x = -310;
            int y = -210;

            wii_gx_drawrectangle(x + -padding, y + h + padding,
                                 w + (padding << 1), h + (padding << 1), color,
                                 TRUE);

            wii_gx_drawtext(x, y, pixelSize, text2, ftgxWhite,
                            FTGX_ALIGN_BOTTOM);
            // wii_gx_drawtext(-310, -210, 14, text2, ftgxWhite, 0);
        }
    }
}

/**
 * Returns the current size to use for the screen
 *
 * @param   inX Input x value
 * @param   inY Input y value
 * @param   x (out) Output x value
 * @param   y (out) Output y value
 */
void wii_get_screen_size(int inX, int inY, int* x, int* y) {
    int xs = inX;
    int ys = inY;

    // 4:3 correct is applicable and enabled
    if (wii_16_9_correction == WS_AUTO ? is_widescreen : wii_16_9_correction) {
        xs = (xs * 3) / 4;  // Widescreen correct
    }

    // Round up
    xs = ((xs + 1) & ~1);
    ys = ((ys + 1) & ~1);

    // Set output values
    *x = xs;
    *y = ys;
}

/**
 * Sets the video mode for the Wii
 *
 * @param   allowVi Whether to allow VI-based modes (GX+VI and Double-strike)
 */
static void wii_set_video_mode(BOOL allowVi) {
    // Get the screen size
    int x, y;
    wii_get_screen_size(wii_screen_x, wii_screen_y, &x, &y);

    if (allowVi && wii_double_strike_mode) {
        WII_SetFilter(FALSE);
        WII_SetDoubleStrikeVideoMode(x, y >> 1, ATARI_WIDTH);        
    } else if (allowVi && wii_gx_vi_scaler) {
        // VI+GX
        WII_SetFilter(FALSE);
        WII_SetStandardVideoMode(x, y, ATARI_WIDTH);
    } else {
        // Scale the screen (GX)
        WII_SetDefaultVideoMode();
        WII_SetFilter(wii_filter);
        WII_ChangeSquare(x, y, 0, 0);
    }
}

/**
 * Runs the main Atari emulator loop
 *
 * @param   testframes The number of testframes to run (for loading saves)
 */
void wii_atari_main_loop(int testframes) {
    // Track the first fire of the lightgun (so that the catridge can properly
    // detect joystick versus lightgun.)
    lightgun_first_fire = true;

    // Only enable lightgun if the cartridge supports it and we are displaying
    // at 2x.
    lightgun_enabled =
        (cartridge_controller[0] & CARTRIDGE_CONTROLLER_LIGHTGUN);

    float fps_counter;
    u32 timerCount = 0;
    u32 start_time = SDL_GetTicks();

    timer_Reset();

    if (testframes < 0) {
        wii_sdl_black_screen();
        VIDEO_SetTrapFilter(wii_trap_filter);
        wii_set_video_mode(TRUE);              
        wii_gx_push_callback(&wii_render_callback, TRUE, NULL);                                    
    }                                                        

    while (!prosystem_paused) {
        if (testframes < 0) {
            wii_atari_update_keys(keyboard_data);
            wii_testframe = false;
        } else {
            wii_testframe = true;
        }

        if (prosystem_active && !prosystem_paused) {
            prosystem_ExecuteFrame(keyboard_data);

            while (!timer_IsTime())
                ;

            fps_counter =
                (((float)timerCount++ / (SDL_GetTicks() - start_time)) *
                 1000.0);
            wii_atari_refresh_screen(wii_vsync, testframes);

            if (testframes < 0) {
                sound_Store();
            }

            wii_fps_counter = fps_counter;

            if (testframes > 0) {
                --testframes;
            } else if (testframes == 0) {
                return;
            }
        }
    }

    if (testframes < 0) {
        // Remove callback
        WII_VideoStop();                                                        
        wii_gx_pop_callback();
        wii_gx_push_callback( NULL, FALSE, NULL ); // Blank screen   
        WII_SetDefaultVideoMode();
        VIDEO_WaitVSync();
        WII_VideoStart(); 
        VIDEO_WaitVSync();
        wii_gx_pop_callback();
    }

    // Save the high score SRAM
    cartridge_SaveHighScoreSram();
}
