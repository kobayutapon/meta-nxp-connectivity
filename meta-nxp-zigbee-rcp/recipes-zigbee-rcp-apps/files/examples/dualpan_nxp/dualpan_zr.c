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
/* PURPOSE: Dualpan application
*/

#define ZB_TRACE_FILE_ID 63256
#include "zboss_api.h"

#include "dualpan.h"

#if ! defined ZB_ROUTER_ROLE
#error define ZB_ROUTER_ROLE to compile zr tests
#endif

/* Used endpoint */
#define ENDPOINT  10

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zr_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

/**
 * Declaring attributes for each cluster
 */

/* Basic cluster attributes */
zb_uint8_t g_attr_zcl_version  = ZB_ZCL_VERSION;                     /* ZCL version attribute */
zb_uint8_t g_attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN;  /* Power source attribute */

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_zcl_version, &g_attr_power_source);

/* Declare cluster list for a device */
ZB_HA_DECLARE_DUALPAN_CLUSTER_LIST(dualpan_clusters,
          basic_attr_list);

/* Declare endpoint */
ZB_HA_DECLARE_DUALPAN_EP(dualpan_ep, ENDPOINT, dualpan_clusters);

/* Declare application's device context for single-endpoint device */
ZBOSS_DECLARE_DEVICE_CTX_1_EP(dualpan_ctx, dualpan_ep);


MAIN()
{
  ARGV_UNUSED;

  /* Trace disable */
  /* ZB_SET_TRACE_OFF(); */
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("dualpan_zr");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zr_addr);
  zb_set_network_router_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&dualpan_ctx);

#ifdef NXP_DUALPAN_SENSE_PAN_CHANNEL
  zb_buf_get_out_delayed(zb_get_pan_channel);
#endif  /* NXP_DUALPAN_SENSE_PAN_CHANNEL */

  /* Initiate the stack start with starting the commissioning */
  if (zboss_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zboss_start failed", (FMT__0));
  }
  else
  {
    /* Call the main loop */
    zboss_main_loop();
  }

  /* Deinitialize trace */
  TRACE_DEINIT();

  MAIN_RETURN(0);
}

/* Callback to handle the stack events */
void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, NULL);

  TRACE_MSG(TRACE_APP1, ">> zboss_signal_handler %h", (FMT__H, param));

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch(sig)
    {
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        break;

      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        TRACE_MSG(TRACE_APP1, "Production configuration block is ready", (FMT__0));
        break;

     case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        break;

      default:
        TRACE_MSG(TRACE_APP1, "Unknown signal %hd", (FMT__H, sig));
    }
  }
  else if (sig == ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY)
  {
    TRACE_MSG(TRACE_APP1, "Production config is not present or invalid", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, ZB_GET_APP_SIGNAL_STATUS(param)));
  }

  /* Free the buffer if it is not used */
  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}
