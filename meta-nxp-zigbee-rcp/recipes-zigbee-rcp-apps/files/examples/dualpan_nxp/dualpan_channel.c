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
/* PURPOSE: Dualpan channel management
*/


#define ZB_TRACE_FILE_ID 63255
#include "zboss_api.h"

#ifdef NXP_DUALPAN_SENSE_PAN_CHANNEL

#include "dualpan.h"

static void set_pan_channel_cb(zb_bufid_t param)
{
//  zb_pan_channel_params_t *channel_params = ZB_BUF_GET_PARAM(param, zb_pan_channel_params_t);
  zb_pan_channel_params_t *channel_params = zb_buf_begin(param);

  if(channel_params->status != RET_OK)
  {
    WCS_TRACE_INFO("%s() failed %s", __FUNCTION__, wcs_get_error_str(channel_params->status));
  }
  else
  {
    WCS_TRACE_INFO("%s() response OK channel %d, page %d", __FUNCTION__, channel_params->pan_channel, channel_params->page);
  }

  zb_buf_free(param);
}

static void get_pan_channel_cb(zb_bufid_t param)
{
//  zb_pan_channel_params_t *channel_params = ZB_BUF_GET_PARAM(param, zb_pan_channel_params_t);
  zb_pan_channel_params_t *channel_params = zb_buf_begin(param);

  if(channel_params->status != RET_OK)
  {
    WCS_TRACE_INFO("%s() failed %s", __FUNCTION__, wcs_get_error_str(channel_params->status));
  }
  else
  {
    zb_uint32_t current_mask = zb_get_channel_mask();

    WCS_TRACE_INFO("%s() response OK channel %u, page %u", __FUNCTION__, channel_params->pan_channel, channel_params->page);

    /* Compare the channel used by OpenThread to our channel */
    if(channel_params->pan_channel != 0  && 1U<<channel_params->pan_channel != current_mask)
    {
      /* OpenThread uses a different channel as us, update our config */
      char env_param[32];
      int ret;

      WCS_TRACE_INFO("get_pan_channel_cb() Zigbee channel mask is 0x%08x, update it to match OpenThread channel", current_mask);

      snprintf(env_param, sizeof(env_param), "MACSPLIT_CHANNEL=%u", channel_params->pan_channel);

      WCS_TRACE_NOTICE("15.4 PAN Channel SET ENV Variable %s", env_param);
      ret = putenv(env_param);
      if(ret == 0)
      {
        WCS_TRACE_NOTICE("15.4 PAN Channel is already configured from side PAN Thread Network!!");
        WCS_TRACE_NOTICE("Re-Configuring Zigbee PAN channel to %u & RESTART APP", channel_params->pan_channel);
        /* Re-start the application */
#ifndef ZB_RESET_AUTORESTART
#error ZB_RESET_AUTORESTART is mandatory to be able to restart the application
#endif
        zb_reset(param);
        param = 0;
      }
      else
      {
        WCS_TRACE_INFO("get_pan_channel_cb() failed to update env %s, TEST FAILED", env_param);
      }
    }
    else
    {
      /* Openthread is nor running or as the same channel as us, just request that channel */
      if(channel_params->pan_channel == 0)
        while(1U<<channel_params->pan_channel != current_mask)
          channel_params->pan_channel++;
      channel_params->cb = set_pan_channel_cb;

      WCS_TRACE_INFO("%s() Set ZB PAN channel %d, page %d", __FUNCTION__,   channel_params->pan_channel, channel_params->page);

      ZB_SCHEDULE_APP_CALLBACK(zb_set_pan_channel_async, param);
      param = 0;
    }
  }

  if(param)
    zb_buf_free(param);
}

void zb_get_pan_channel(zb_bufid_t param)
{
  /* Get from firmware dualpan_channel config */
  zb_pan_channel_params_t *channel_params;

  channel_params = zb_buf_initial_alloc(param, sizeof(zb_pan_channel_params_t));
//  channel_params = ZB_BUF_GET_PARAM(param, zb_pan_channel_params_t);
  channel_params->page = ZB_CHANNEL_PAGE0_2_4_GHZ;
  channel_params->pan_channel = -1;
  channel_params->cb = get_pan_channel_cb;

  WCS_TRACE_INFO("%s() Get 15.4 side PAN channel of page %d", __FUNCTION__, channel_params->page);

  ZB_SCHEDULE_APP_CALLBACK(zb_get_pan_channel_async, param);
}
#endif  /* NXP_DUALPAN_SENSE_PAN_CHANNEL */

