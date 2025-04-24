/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2024 NXP
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: Dimmable light sample HAL
*/
#define ZB_TRACE_FILE_ID 40165
#include "bulb.h"
#include "bulb_hal.h"

#ifdef ZB_USE_BUTTONS

#define BULB_LED_POWER 0
#define BULB_LED_CONNECT 1
#if ZB_N_LEDS > 2
#define BULB_LED_LIGHT 2
#elif ZB_N_LEDS == 2
/* Reuse one of indicator LEDs for the bulb light */
#define BULB_LED_LIGHT BULB_LED_CONNECT
#else
/* Not all platforms define ZB_N_LEDS properly and by default the number of
 * LEDs is one. Do not print warning in this case (it can lead to a build
 * failure), assume that number of LEDs is sufficient. */
#define BULB_LED_LIGHT 2
/* #warning "At least 2 LEDs are needed for dimmable light sample" */
#endif
#define BULB_BUTTON_2 ZB_BOARD_BUTTON_1

/* Public interface */
void bulb_hal_init(void)
{
  zb_osif_led_button_init();

  zb_osif_led_level_init(BULB_LED_LIGHT);

  zb_osif_led_on(BULB_LED_POWER);
}

void bulb_hal_set_level(zb_uint8_t level)
{
  zb_osif_led_on_set_level(level);
}

void bulb_hal_set_on_off(zb_bool_t on)
{
  if (on)
  {
    zb_osif_led_on(BULB_LED_LIGHT);
  }
  else
  {
    zb_osif_led_on_set_level(0);
    zb_osif_led_off(BULB_LED_LIGHT);
  }
}

void bulb_hal_set_connect(zb_bool_t on)
{
  if (on)
  {
    zb_osif_led_on(BULB_LED_CONNECT);
  }
  else
  {
    zb_osif_led_off(BULB_LED_CONNECT);
  }
}

zb_bool_t bulb_hal_is_button_pressed(zb_uint8_t button_no)
{
  zb_bool_t ret = ZB_FALSE;
  ret = zb_osif_button_state(button_no);
  return ret;
}

#endif