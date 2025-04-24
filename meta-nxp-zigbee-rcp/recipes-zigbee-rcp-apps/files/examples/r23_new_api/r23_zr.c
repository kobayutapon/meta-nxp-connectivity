/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 *  Copyright 2024 NXP
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
/* PURPOSE: R23 new API
*/

#define ZB_TRACE_FILE_ID 40565

#include "zboss_api.h"
#include "r23_applications_common.h"

/**
 * Global variables definitions
 */

/* IEEE address of the device */
static zb_ieee_addr_t g_r23_zr_ieee_addr = IEEE_ADDRESS_R23_ZR;

MAIN()
{
  ARGV_UNUSED;

  /* Global ZBOSS initialization */
  ZB_INIT("r23_zr");

  zb_set_long_address(g_r23_zr_ieee_addr);

  /* Set device role */
#ifndef ZB_APP_ENABLE_SUBGHZ_MODE
  {
    zb_set_network_router_role(APPLICATION_CHANNEL_MASK);
  }
#else
  {
    zb_channel_list_t channel_list;
    zb_channel_list_init(channel_list);

    zb_channel_page_list_set_mask(channel_list, APPLICATION_CHANNEL_PAGE, APPLICATION_CHANNEL_MASK);
    zb_set_network_router_role_ext(channel_list);
  }
#endif

  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* For ZR/ZED zboss_use_r23_behavior() sets all key negotiation methods and enables ZB_TLV_PSK_SECRET_AUTH_TOKEN by default
   * When IC or passcode are set, the corresponding PSK secret automatically enabled
   * (ZB_TLV_PSK_SECRET_INSTALL_CODE or ZB_TLV_PSK_SECRET_PAKE_PASSCODE) */
  zboss_use_r23_behavior();

  /* Initiate the stack start with starting the commissioning */
  if (zboss_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "ERROR zboss_start failed", (FMT__0));
  }
  else
  {
    /* Call the main loop */
    zboss_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_t sig = zb_get_app_signal(param, &sg_p);
  zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);

  TRACE_MSG(TRACE_APP1, ">> zboss_signal_handler: param %hd, status %hd, signal %hd",
    (FMT__H_H_H, param, status, sig));

  switch (sig)
  {
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      {
        if (status == RET_OK)
        {
          TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        }
        else
        {
          TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d",
            (FMT__D, status));
        }

        break;
      }

      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
      {
        if (status == RET_ERROR)
        {
          TRACE_MSG(TRACE_APP1, "Production config is not present or invalid", (FMT__0));
        }
        else {
          TRACE_MSG(TRACE_APP1, "Production config is ready", (FMT__0));
        }

        break;
      }
  }

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}
