/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 * Copyright 2023-2025 NXP
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
/* PURPOSE: ZB Simple output device
*/
/* ![mem_config_max] */
#define ZB_TRACE_FILE_ID 40248
#include "zboss_api.h"
#include "zb_led_button.h"
#include "zboss_api_error.h"

/* Insert that include before any code or declaration. */
#ifdef ZB_CONFIGURABLE_MEM
#include "zb_mem_config_max.h"
#endif
/* Next define clusters, attributes etc. */
/* ![mem_config_max] */

/*
#if ! defined ZB_COORDINATOR_ROLE
#error define ZB_COORDINATOR_ROLE to compile zc tests
#endif
*/
#if !defined ZB_ROUTER_ROLE
#error define ZB_ROUTER_ROLE to build led bulb demo
#endif

#define SUBGHZ_PAGE 1
#define SUBGHZ_CHANNEL 25
#define ZCL_ONOFF_ENABLE_OPTIONAL_ATTRIBUTES 1

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}; /* IEEE address of the
                                                                              * device */
zb_uint16_t g_dst_addr;
zb_uint8_t g_addr_mode;
zb_uint8_t g_endpoint;

/* Uncomment to enable installcode usage */
/* #define TEST_USE_INSTALLCODE */

#ifdef TEST_USE_INSTALLCODE
/* IEEE address of the end device */
zb_ieee_addr_t g_ed_addr = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
char g_installcode[]= "966b9f3ef98ae605 9708";
#endif

#define ZB_REG_TEST_DEFAULT_NWK_KEY {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

static zb_uint8_t g_nwk_key[16] = ZB_REG_TEST_DEFAULT_NWK_KEY;


/* Used endpoint */
#define ZB_OUTPUT_ENDPOINT          5
#define ZB_OUTPUT_MAX_CMD_PAYLOAD_SIZE 2

/* Handler for specific ZCL commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

void test_device_interface_cb(zb_uint8_t param);
void button_press_handler(zb_uint8_t param);

/**
 * Declaring attributes for each cluster
 */
/* On/Off cluster attributes */
zb_uint8_t g_attr_on_off = ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;
#if (ZCL_ONOFF_ENABLE_OPTIONAL_ATTRIBUTES == 1)
/* On/Off cluster attributes additions */
zb_bool_t g_attr_global_scene_ctrl  = ZB_TRUE;
zb_uint16_t g_attr_on_time  = 0;
zb_uint16_t g_attr_off_wait_time  = 0;

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list, &g_attr_on_off,
    &g_attr_global_scene_ctrl, &g_attr_on_time, &g_attr_off_wait_time);
#else
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list, &g_attr_on_off);
#endif

/* Basic cluster attributes */
zb_uint8_t g_attr_zcl_version  = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_zcl_version, &g_attr_power_source);

/* Identify cluster attributes */
zb_uint16_t g_attr_identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &g_attr_identify_time);

/* Groups cluster attributes */
zb_uint8_t g_attr_name_support = 0;

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &g_attr_name_support);

#ifdef ZB_ZCL_SUPPORT_CLUSTER_SCENES
/* Scenes cluster attributes */
zb_uint8_t g_attr_scenes_scene_count = ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE;
zb_uint8_t g_attr_scenes_current_scene = ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE;
zb_uint16_t g_attr_scenes_current_group = ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE;
zb_uint8_t g_attr_scenes_scene_valid = ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE;
zb_uint16_t g_attr_scenes_name_support = ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE;

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list, &g_attr_scenes_scene_count,
    &g_attr_scenes_current_scene, &g_attr_scenes_current_group,
    &g_attr_scenes_scene_valid, &g_attr_scenes_name_support);
#else
zb_zcl_attr_t scenes_attr_list[] = { ZB_ZCL_NULL_ID, 0, 0, NULL };
#endif

/* Declare cluster list for the device */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CLUSTER_LIST(on_off_output_clusters,
                                         on_off_attr_list,
                                         basic_attr_list,
                                         identify_attr_list,
                                         groups_attr_list,
                                         scenes_attr_list);

/* Declare endpoint */
ZB_HA_DECLARE_ON_OFF_OUTPUT_EP(on_off_output_ep, ZB_OUTPUT_ENDPOINT, on_off_output_clusters);

/* Declare application's device context for single-endpoint device */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(on_off_output_ctx, on_off_output_ep);


static zb_bool_t error_ind_handler(zb_uint8_t severity,
                                   zb_ret_t error_code,
                                   void *additional_info);

#ifdef ZB_MAC_CONFIGURABLE_TX_POWER /* Test API zb_set_tx_power */
static void tx_power_cb(zb_bufid_t param)
{
  zb_tx_power_params_t *power_params = zb_buf_begin(param);

  WCS_TRACE_INFO("%s_tx_power() response %s: channel %d, page %d, power 0x%02x (%d dBm)",
    (getenv("ZB_TX_POWER"))?("set"):("get"),
    wcs_get_error_str(power_params->status),
    power_params->channel,
    power_params->page,
    power_params->tx_power&0xFF,
    power_params->tx_power);

  zb_buf_free(param);
}

static void request_tx_power(zb_bufid_t param)
{
  zb_tx_power_params_t *power_params;
  char *tmp = getenv("ZB_TX_POWER");

  TRACE_MSG(TRACE_APP1, "param %hd", (FMT__H, param));

  power_params = zb_buf_initial_alloc(param, sizeof(zb_tx_power_params_t));

  power_params->page     = zb_get_current_page();
  power_params->channel  = zb_get_current_channel();
  if(tmp)
    power_params->tx_power = (zb_uint8_t)atoi(tmp);
  power_params->cb = tx_power_cb;

  ZB_SCHEDULE_APP_CALLBACK((tmp)?(zb_set_tx_power_async):(zb_get_tx_power_async), param);
}
#endif

MAIN()
{
  ARGV_UNUSED;

  /* Trace enable */
  ZB_SET_TRACE_ON();
  /* Traffic dump enable*/
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("on_off_output_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_addr);
#ifndef ZB_APP_ENABLE_SUBGHZ_MODE
  {
    zb_set_network_coordinator_role(1l<<21);
  }
#else
  {
    zb_channel_list_t channel_list;
    zb_channel_list_init(channel_list);
    zb_channel_page_list_set_mask(channel_list, (ZB_CHANNEL_LIST_PAGE28_IDX), (1l << 24));
    zb_set_network_coordinator_role_ext(channel_list);
  }
#endif
  zb_set_nvram_erase_at_start(ZB_FALSE);
  zb_secur_setup_nwk_key(g_nwk_key, 0);

#ifdef ZB_ENABLE_PTA
  zb_enable_pta(0);
#endif
  zb_nwk_set_max_ed_capacity(3);

/* [af_register_device_context] */
 /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&on_off_output_ctx);
/* [af_register_device_context] */

  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(ZB_OUTPUT_ENDPOINT, zcl_specific_cluster_cmd_handler);

/* [zcl_register_device_cb] */
  /* Set Device user application callback */
  ZB_ZCL_REGISTER_DEVICE_CB(test_device_interface_cb);
/* [zcl_register_device_cb] */

#ifdef ZB_USE_BUTTONS
  zb_button_register_handler(0, 0, button_press_handler);
#endif

  zb_error_register_app_handler(error_ind_handler);

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


static zb_bool_t error_ind_handler(zb_uint8_t severity,
                                    zb_ret_t error_code,
                                    void *additional_info)
{
  zb_bool_t ret = ZB_FALSE;
  ZVUNUSED(additional_info);
  /* Unused without trace. */
  ZVUNUSED(severity);

  TRACE_MSG(TRACE_APP1, "error_ind_handler severity %hd error_code %d",
            (FMT__H_D, severity, error_code));

// #ifdef ZB_MACSPLIT_HOST
//   if (error_code == ERROR_CODE(ERROR_CATEGORY_MACSPLIT, ZB_ERROR_MACSPLIT_RADIO_HANG_NO_ACK) ||
//       error_code == ERROR_CODE(ERROR_CATEGORY_MACSPLIT, ZB_ERROR_MACSPLIT_RADIO_CONFIRM_TIMEOUT) ||
//       error_code == ERROR_CODE(ERROR_CATEGORY_MACSPLIT, ZB_ERROR_MACSPLIT_RADIO_REBOOT))
//   {
//     TRACE_MSG(TRACE_APP1, "Macsplit error", (FMT__0));
// #if 0 // Keep the stack manage the error: this code is an example how the application decide to manager the error
// #ifdef ZB_ZBOSS_DEINIT
//     {
//       zb_bufid_t param = zb_buf_get_out();
//       if (param != ZB_BUF_INVALID)
//       {
//         /* return TRUE to prevent default error handling by the stack */
//         ret = ZB_TRUE;
//         if (error_code == ERROR_CODE(ERROR_CATEGORY_MACSPLIT, ZB_ERROR_MACSPLIT_RADIO_HANG_NO_ACK))
//         {
//           TRACE_MSG(TRACE_APP1, "Fatal macsplit error", (FMT__0));
//           zboss_shut_with_host_reset(param);
//         }
//         else /* ZB_ERROR_MACSPLIT_RADIO_REBOOT */
//         {
//           TRACE_MSG(TRACE_APP1, "macsplit radio reboot", (FMT__0));
//           zboss_start_shut(param);
//         }
//       }
//     }
// #endif
//     if (!ret)
//     {
//       ZB_ASSERT(0);
//     }
//     /* return TRUE to prevent default error handling by the stack */
// #endif
//   }
// #endif
  TRACE_MSG(TRACE_APP1, "error_ind_handler ret %d ", (FMT__D, ret));
  return ret;
}


/* [zcl_register_device_cb_example_cb] */
void test_device_interface_cb(zb_uint8_t param)
{
  zb_zcl_device_callback_param_t *device_cb_param =
    ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);

  TRACE_MSG(TRACE_APP1, "> test_device_interface_cb param %hd id 0x%hx", (FMT__H_H,
      param, device_cb_param->device_cb_id));

  device_cb_param->status = RET_OK;

  switch (device_cb_param->device_cb_id)
  {
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
      if (device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
          device_cb_param->cb_param.set_attr_value_param.attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
      {
        if (device_cb_param->cb_param.set_attr_value_param.values.data8)
        {
          WCS_TRACE_DBGREL("set ON");
          TRACE_MSG(TRACE_APP1, "set ON", (FMT__0));
#ifdef ZB_USE_BUTTONS
          zb_osif_led_on(0);
#endif
        }
        else
        {
          WCS_TRACE_DBGREL("set OFF");
          TRACE_MSG(TRACE_APP1, "set OFF", (FMT__0));
#ifdef ZB_USE_BUTTONS
          zb_osif_led_off(0);
#endif
        }
      }
      else if (device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_IDENTIFY &&
               device_cb_param->cb_param.set_attr_value_param.attr_id == ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID)
      {
        TRACE_MSG(TRACE_APP1, "identify time changed to %d (0x%x)",
                  (FMT__D_D, device_cb_param->cb_param.set_attr_value_param.values.data16, device_cb_param->cb_param.set_attr_value_param.values.data16));
      }
      else
      {
        /* MISRA rule 15.7 requires empty 'else' branch. */
        WCS_TRACE_WARNING("device_callback: SET_ATTR_VALUE of cluster %d attr %d not implemented", device_cb_param->cb_param.set_attr_value_param.cluster_id, device_cb_param->cb_param.set_attr_value_param.attr_id);
      }
      break;

    default:
      WCS_TRACE_WARNING("device_callback: device_cb_id %d not implemented", device_cb_param->device_cb_id);
      device_cb_param->status = RET_NOT_IMPLEMENTED; /* Use not implemented instead of error to have in ZCL Cmd Resp Status unsup. cluster instead of hw failure */
      break;
  }

  TRACE_MSG(TRACE_APP1, "< test_device_interface_cb %hd", (FMT__H, device_cb_param->status));
}
/* [zcl_register_device_cb_example_cb] */

/* [zb_zdo_get_diag_data_async_example] */
static void handle_diag_data_resp(zb_bufid_t buf)
{
  zb_zdo_get_diag_data_resp_params_t *resp_params;

  resp_params = ZB_BUF_GET_PARAM(buf, zb_zdo_get_diag_data_resp_params_t);

  ZVUNUSED(resp_params);

  TRACE_MSG(TRACE_APP1, "handle_diag_data_resp, status: %d, addr: 0x%x, lqi: %d, rssi: %d",
            (FMT__D_D_D_D, resp_params->status, resp_params->short_address,
             resp_params->lqi, resp_params->rssi));

  zb_buf_free(buf);
}

static void send_diag_data_req(zb_uint16_t short_address)
{
  zb_zdo_get_diag_data_req_params_t *req;
  zb_bufid_t buf;

  buf = zb_buf_get_out();
  if (buf != ZB_BUF_INVALID)
  {
    req = ZB_BUF_GET_PARAM(buf, zb_zdo_get_diag_data_req_params_t);
    ZB_BZERO(req, sizeof(*req));

    req->short_address = short_address;
    zb_zdo_get_diag_data_async(buf, handle_diag_data_resp);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Failed to get a buffer", (FMT__0));
  }
}
/* [zb_zdo_get_diag_data_async_example] */

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_zcl_parsed_hdr_t cmd_info;

  TRACE_MSG(TRACE_APP1, "> zcl_specific_cluster_cmd_handler", (FMT__0));

  ZB_ZCL_COPY_PARSED_HEADER(param, &cmd_info);

  g_dst_addr = ZB_ZCL_PARSED_HDR_SHORT_DATA(&cmd_info).source.u.short_addr;
  g_endpoint = ZB_ZCL_PARSED_HDR_SHORT_DATA(&cmd_info).src_endpoint;
  g_addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;

  ZB_ZCL_DEBUG_DUMP_HEADER(&cmd_info);
  TRACE_MSG(TRACE_APP3, "payload size: %i", (FMT__D, zb_buf_len(param)));

  send_diag_data_req(g_dst_addr);

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

void button_press_handler(zb_uint8_t param)
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_APP1, "button is pressed, do nothing", (FMT__0));
}

/* Callback to handle the stack events */
void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

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
#ifdef TEST_USE_INSTALLCODE
        zb_secur_ic_str_add(g_ed_addr, g_installcode, NULL);
#endif
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
        /* [zb_get_device_type_example] */
        zb_nwk_device_type_t device_type = ZB_NWK_DEVICE_TYPE_NONE;
        device_type = zb_get_device_type();
        ZVUNUSED(device_type);
        TRACE_MSG(TRACE_APP1, "Device (%d) STARTED OK", (FMT__D, device_type));
        /* [zb_get_device_type_example] */

#ifdef ZB_MAC_CONFIGURABLE_TX_POWER
        request_tx_power(param);
        param = 0;
#endif
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        break;
      }

/* [zb_bdb_finding_binding_target_usage] */
      case ZB_BDB_SIGNAL_STEERING:
        TRACE_MSG(TRACE_APP1, "Successful steering, start f&b target", (FMT__0));
        zb_bdb_finding_binding_target(ZB_OUTPUT_ENDPOINT);
        break;
/* [zb_bdb_finding_binding_target_usage] */

#ifdef ZB_ZBOSS_DEINIT
      case ZB_SIGNAL_READY_TO_SHUT:
        TRACE_MSG(TRACE_APP1, "Got ZB_SIGNAL_READY_TO_SHUT", (FMT__0));
        zboss_complete_shut();
        break;
#endif

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
