/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
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
/* PURPOSE: Level Control Server
*/
/* ![mem_config_max] */
#define ZB_TRACE_FILE_ID 40050

#include "level_control_zc.h"

#if ! defined ZB_COORDINATOR_ROLE
#error define ZB_COORDINATOR_ROLE to compile zc sample
#endif

#define ZB_LEVEL_CONTROL_SERVER_ENDPOINT 5
#define ZB_LEVEL_CONTROL_CLIENT_ENDPOINT 10

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ed_addr = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);
void test_device_interface_cb(zb_uint8_t param);
void button_press_handler(zb_uint8_t param);

/* Identify cluster attributes data */
zb_uint16_t g_attr_identify_identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
/* On/Off cluster attributes data */
zb_bool_t g_attr_on_off_on_off = ZB_ZCL_ON_OFF_IS_ON;
/* Level Control cluster attributes data */
zb_uint8_t g_attr_level_control_current_level = ZB_ZCL_LEVEL_CONTROL_CURRENT_LEVEL_DEFAULT_VALUE;
zb_uint16_t g_attr_level_control_remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list, &g_attr_on_off_on_off);
ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST(level_control_attr_list, &g_attr_level_control_current_level, &g_attr_level_control_remaining_time);
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &g_attr_identify_identify_time);

ZB_ZCL_DECLARE_LEVEL_CONTROL_OUTPUT_CLUSTER_LIST(level_control_switch_clusters, on_off_attr_list, level_control_attr_list, identify_attr_list);
ZB_ZCL_DECLARE_LEVEL_CONTROL_OUTPUT_EP(level_control_switch_ep, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, level_control_switch_clusters);
ZB_ZCL_DECLARE_LEVEL_CONTROL_OUTPUT_CTX(level_control_zc_ctx, level_control_switch_ep);

MAIN()
{
  ARGV_UNUSED;

  /* Trace enable */
  ZB_SET_TRACE_ON();
  /* Traffic dump enable*/
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("level_control_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_addr);
  zb_set_network_coordinator_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);

  zb_nwk_set_max_ed_capacity(1);

/* [af_register_device_context] */
 /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&level_control_zc_ctx);
/* [af_register_device_context] */

  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(ZB_LEVEL_CONTROL_SERVER_ENDPOINT, zcl_specific_cluster_cmd_handler);

/* [zcl_register_device_cb] */
  /* Set Device user application callback */
  ZB_ZCL_REGISTER_DEVICE_CB(test_device_interface_cb);
/* [zcl_register_device_cb] */

  /* Initiate the stack start without starting the commissioning */
  if (zboss_start_no_autostart() != RET_OK)
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

/* [zcl_register_device_cb_example_cb] */
void test_device_interface_cb(zb_uint8_t param)
{
  zb_zcl_device_callback_param_t *device_cb_param =
    ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);

  TRACE_MSG(TRACE_APP1, "> test_device_interface_cb param %hd id %hd", (FMT__H_H,
      param, device_cb_param->device_cb_id));

  device_cb_param->status = RET_OK;

  switch (device_cb_param->device_cb_id)
  {
    default:
      device_cb_param->status = RET_OK;
      break;
  }

  TRACE_MSG(TRACE_APP1, "< test_device_interface_cb %hd", (FMT__H, device_cb_param->status));
}
/* [zcl_register_device_cb_example_cb] */

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_zcl_parsed_hdr_t cmd_info;

  TRACE_MSG(TRACE_APP1, "> zcl_specific_cluster_cmd_handler", (FMT__0));

  ZB_ZCL_COPY_PARSED_HEADER(param, &cmd_info);

  ZB_ZCL_DEBUG_DUMP_HEADER(&cmd_info);
  TRACE_MSG(TRACE_APP3, "payload size: %i", (FMT__D, zb_buf_len(param)));

  if (cmd_info.cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)
  {
    TRACE_MSG(
        TRACE_ERROR,
        "Unsupported \"from server\" command direction",
        (FMT__0));
  }

  TRACE_MSG(TRACE_APP1, "< zcl_specific_cluster_cmd_handler", (FMT__0));
  return ZB_FALSE;
}

/* Callback to handle the stack events */
void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_t sig = zb_get_app_signal(param, &sg_p);

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
      case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        zboss_start_continue();
        break;

#ifdef ZB_MACSPLIT_HOST
      case ZB_MACSPLIT_DEVICE_BOOT:
        {
          zb_zdo_signal_macsplit_dev_boot_params_t *boot_params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_macsplit_dev_boot_params_t);
          TRACE_MSG(TRACE_APP1, "ZB_MACSPLIT_DEVICE_BOOT dev version %d", (FMT__D, boot_params->dev_version));
        }
        break;
#endif /* ZB_MACSPLIT_HOST */

      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      {
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        break;
      }

/* [zb_bdb_finding_binding_target_usage] */
      case ZB_BDB_SIGNAL_STEERING:
        TRACE_MSG(TRACE_APP1, "Successful steering, start f&b target", (FMT__0));
        zb_bdb_finding_binding_target(ZB_LEVEL_CONTROL_SERVER_ENDPOINT);
        break;
/* [zb_bdb_finding_binding_target_usage] */
      default:
        TRACE_MSG(TRACE_APP1, "Unknown signal %d", (FMT__D, (zb_uint16_t)sig));
    }
  }
  else if (sig == ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY)
  {
    TRACE_MSG(TRACE_APP1, "Production config is not present or invalid", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d sig %d", (FMT__D_D, ZB_GET_APP_SIGNAL_STATUS(param), sig));
  }

  /* Free the buffer if it is not used */
  if (param)
  {
    zb_buf_free(param);
  }
}
