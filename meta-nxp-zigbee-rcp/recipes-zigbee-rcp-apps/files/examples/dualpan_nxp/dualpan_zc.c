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


#define ZB_TRACE_FILE_ID 63255
#include "zboss_api.h"

#include "dualpan.h"

#if ! defined ZB_COORDINATOR_ROLE
#error define ZB_COORDINATOR_ROLE to compile zc tests
#endif

/* In current implementation, the network is re-opened after each device announce */
#define ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK

/* Used endpoint */
#define ENDPOINT  5

#define MAX_DUALPAN_CLIENTS 64

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};


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


#ifdef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
static void send_permit_joining_req(zb_bufid_t param)
{
  zb_zdo_mgmt_permit_joining_req_param_t *request;

  TRACE_MSG(TRACE_ZDO2, "send_permit_joining_req %hd", (FMT__H, param));

  request = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_permit_joining_req_param_t);
  request->dest_addr = ZB_NWK_BROADCAST_ROUTER_COORDINATOR;
  request->permit_duration = 180;
  WCS_TRACE_DBGREL("Request to re-open the network for %us", request->permit_duration);
  zb_zdo_mgmt_permit_joining_req(param, NULL);
}
#endif /* ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK */


MAIN()
{
  ARGV_UNUSED;

  /* Trace disable */
  /* ZB_SET_TRACE_OFF(); */
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("dualpan_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_addr);
  zb_set_network_coordinator_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);
  zb_nwk_set_max_ed_capacity(MAX_DUALPAN_CLIENTS);
  zb_secur_setup_nwk_key(g_key, 0);

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
  /* Get application signal from the buffer */
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);
#ifdef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
  zb_uint8_t status = ZB_GET_APP_SIGNAL_STATUS(param);
#endif

  TRACE_MSG(TRACE_APP1, ">> zboss_signal_handler %h", (FMT__H, param));

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch(sig)
    {
#ifdef DEBUG
      case ZB_DEBUG_SIGNAL_TCLK_READY:
      {
        zb_debug_signal_tclk_ready_params_t *params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_debug_signal_tclk_ready_params_t);
        zb_debug_broadcast_aps_key(params->long_addr);
      }
      break;
#endif
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        /* Start steering */
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        break;

#ifdef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
/* [signal_device_annce] */
      case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
      {
        if (status == 0)
        {
          /* Send permit join req to open network for joined devices if required */
          ZB_SCHEDULE_APP_CALLBACK(send_permit_joining_req, param);
          param = 0;
        }
     }
      break;
/* [signal_permit_join_status] */
      case ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
      {
        zb_uint8_t *permit_duration = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_uint8_t);

        /* keep the nwk open */
        if (*permit_duration == 0)
        {
          zb_zdo_mgmt_permit_joining_req_param_t *request_param;
          request_param = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_permit_joining_req_param_t);
          ZB_BZERO(request_param, sizeof(zb_zdo_mgmt_permit_joining_req_param_t));
          request_param->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
          request_param->permit_duration = 180;
          request_param->tc_significance = 1;
          WCS_TRACE_DBGREL("Re-open network for %us", request_param->permit_duration);
          (void)zb_zdo_mgmt_permit_joining_req(param, NULL);
          param = 0;
        }
#endif /* ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK */
        break;
      } /* ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS */

      default:
        TRACE_MSG(TRACE_APP1, "Unknown signal", (FMT__0));
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
