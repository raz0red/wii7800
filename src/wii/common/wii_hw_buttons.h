/*
Copyright (C) 2010 raz0red
*/

#ifndef WII_HW_BUTTONS_H
#define WII_HW_BUTTONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>

// Hardware buttons (reset, power, etc.)
extern u8 wii_hw_button;

/*
 * Callback for the reset button on the Wii.
 */
extern void wii_reset_pressed();
 
/*
 * Callback for the power button on the Wii.
 */
extern void wii_power_pressed();
 
/*
 * Callback for the power button on the Wiimote.
 *
 * chan The Wiimote that pressed the button
 */
extern void wii_mote_power_pressed(s32 chan);

/*
 * Registers the hardware button callbacks 
 */
extern void wii_register_hw_buttons();

#ifdef __cplusplus
}
#endif

#endif
