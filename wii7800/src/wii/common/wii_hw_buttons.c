/*
Copyright (C) 2010 raz0red
*/

#include <wiiuse/wpad.h>

// Hardware buttons (reset, power, etc.)
u8 wii_hw_button = 0;

/*
 * Callback for the reset button on the Wii.
 */
void wii_reset_pressed()
{
  wii_hw_button = SYS_RETURNTOMENU;
}

/*
 * Callback for the power button on the Wii.
 */
void wii_power_pressed()
{
  wii_hw_button = SYS_POWEROFF_STANDBY;
}

/*
 * Callback for the power button on the Wiimote.
 *
 * chan The Wiimote that pressed the button
 */
void wii_mote_power_pressed(s32 chan)
{
  wii_hw_button = SYS_POWEROFF_STANDBY;
} 

/*
 * Registers the hardware button callbacks 
 */
void wii_register_hw_buttons()
{
  SYS_SetResetCallback( wii_reset_pressed );
  SYS_SetPowerCallback( wii_power_pressed );
  WPAD_SetPowerButtonCallback( wii_mote_power_pressed );
}
