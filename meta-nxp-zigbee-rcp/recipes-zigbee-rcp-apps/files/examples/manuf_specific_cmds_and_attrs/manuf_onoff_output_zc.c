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
/* PURPOSE: ZB Simple output device
*/
/* ![mem_config_max] */
#define ZB_TRACE_FILE_ID 40098
#include "zboss_api.h"
#include "manuf_onoff_common.h"
#include "zb_led_button.h"

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

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_ed_addr = LONG_ADDR_ED;
zb_ieee_addr_t g_zc_addr = LONG_ADDR_ZC;

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
zb_uint8_t g_manuf_attr = 0;

ZB_ZCL_START_DECLARE_ATTRIB_LIST(on_off_attr_list)
ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, &g_attr_on_off)
ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(MANUF_CLST_ON_OFF_ATTR,
                                ZB_ZCL_ATTR_TYPE_BOOL,
                                ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_READ_WRITE,
                                MANUF_CODE,
                                (void*) &g_manuf_attr)
ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST;

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
ZB_HA_DECLARE_ON_OFF_OUTPUT_EP_MANUF_TEST(on_off_output_ep, ZB_OUTPUT_ENDPOINT, on_off_output_clusters);

/* Declare application's device context for single-endpoint device */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(on_off_output_ctx, on_off_output_ep);


static zb_ret_t check_attr_val_app(
  zb_uint16_t cluster_id,
  zb_uint8_t cluster_role,
  zb_uint8_t endpoint,
  zb_uint16_t attr_id,
  zb_uint16_t manuf_code,
  zb_uint8_t *value);

static zb_uint8_t disc_manuf_cmd_cb(
    zb_uint8_t direction,
    zb_uint8_t ep,
    zb_uint16_t cluster_id,
    zb_uint16_t manuf_id,
    zb_bool_t recv_cmd_type,
    zb_uint8_t **value);

MAIN()
{
  ARGV_UNUSED;

  /* Trace enable */
  ZB_SET_TRACE_ON();
  /* Traffic dump enable*/
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("manuf_onoff_output_zc");

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


  zb_nwk_set_max_ed_capacity(1);

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

  ZB_ZCL_CHECK_ATTR_VALUE_MANUF_CB(check_attr_val_app);
  ZB_ZCL_SET_DISC_MANUFACTURE_COMMAND_CB(disc_manuf_cmd_cb);

#ifdef ZB_USE_BUTTONS
  zb_button_register_handler(0, 0, button_press_handler);
#endif

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

static void bind_on_off_cluster_cb(zb_uint8_t param)
{
  zb_ret_t status = zb_buf_get_status(param);

  TRACE_MSG(TRACE_APP1, ">> bind_on_off_cluster_cb(), param %hd, ret %ld", (FMT__H_L, param, status));
  zb_buf_free(param);
  TRACE_MSG(TRACE_APP1, "<< bind_on_off_cluster_cb()", (FMT__0));
}

static void bind_on_off_cluster(zb_uint8_t param)
{
  zb_apsme_binding_req_t *req;

  TRACE_MSG(TRACE_APP1, ">> bind_on_off_cluster(), param %hd", (FMT__H, param));

  /* Bind On/Off cluster */
  req = ZB_BUF_GET_PARAM(param, zb_apsme_binding_req_t);

  ZB_IEEE_ADDR_COPY(req->src_addr, g_zc_addr);
  ZB_IEEE_ADDR_COPY(req->dst_addr.addr_long, g_ed_addr);

  req->src_endpoint = ZB_OUTPUT_ENDPOINT;
  req->clusterid = ZB_ZCL_CLUSTER_ID_ON_OFF;
  req->addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
  req->dst_endpoint = ZB_SWITCH_ENDPOINT;
  req->confirm_cb = bind_on_off_cluster_cb;

  ZB_SCHEDULE_APP_CALLBACK(zb_apsme_bind_request, param);

  TRACE_MSG(TRACE_APP1, "<< bind_on_off_cluster()", (FMT__0));
}

static void bind_on_off_cluster_delayed(zb_uint8_t param)
{
  ZVUNUSED(param);
  zb_buf_get_out_delayed(bind_on_off_cluster);
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
          TRACE_MSG(TRACE_APP1, "set ON", (FMT__0));
#ifdef ZB_USE_BUTTONS
          zb_osif_led_on(0);
#endif
        }
        else
        {
          TRACE_MSG(TRACE_APP1, "set OFF", (FMT__0));
#ifdef ZB_USE_BUTTONS
          zb_osif_led_off(0);
#endif
        }
      }
      break;

    default:
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

typedef struct {
  zb_uint8_t value;
} manuf_value_args_t;

zb_uint8_t handle_manuf_cmd(zb_uint8_t param)
{
  zb_uint8_t handled = ZB_FALSE;
  zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(param, zb_zcl_parsed_hdr_t);
  zb_zcl_status_t zcl_status;

  TRACE_MSG(TRACE_APP1, "> handle_manuf_cmd common %d cmd %d",
            (FMT__D_D, cmd_info->is_common_command, cmd_info->cmd_id));

  do {
    manuf_value_args_t *args;

    if (cmd_info->is_common_command ||
        cmd_info->cmd_id != MANUF_CLST_ON_OFF_CMD_SET)
    {
      handled = ZB_FALSE;
      break;
    }

    if (zb_buf_len(param) < sizeof(*args))
    {
      handled = ZB_FALSE;
      break;
    }

    args = (manuf_value_args_t *)zb_buf_begin(param);
    if (args == NULL)
    {
      zcl_status = ZB_ZCL_PARSE_STATUS_FAILURE;
      break;
    }

    /* Commands with even value will be handled manually.
       Commands with odd value will be handled in stack */
    handled = (args->value % 2 == 0);
    if (!handled)
    {
      break;
    }

    (void)zb_buf_cut_left(param, sizeof(*args));

    /* Update manuf-specific attribute */
    zcl_status = zb_zcl_set_attr_val_manuf(
      ZB_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_ON_OFF,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      MANUF_CLST_ON_OFF_ATTR,
      MANUF_CODE,
      (zb_uint8_t*)&args->value,
      /* check_access s*/ ZB_TRUE);
  } while(0);

  if (handled)
  {
    TRACE_MSG(TRACE_APP1, "manuf command with even param is handled manually", (FMT__0));
    ZB_ZCL_PROCESS_COMMAND_FINISH(param, cmd_info, zcl_status);
  }
  else
  {
    TRACE_MSG(TRACE_APP1, "command with will be handled by ZBOSS", (FMT__0));
  }

  TRACE_MSG(TRACE_APP1, "< handle_manuf_cmd handled %d", (FMT__D, handled));
  return handled;
}

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_uint8_t res = ZB_FALSE;
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
    TRACE_MSG(TRACE_ERROR, "Unsupported \"from server\" command direction", (FMT__0));
  }
  else
  {
    if (cmd_info.is_manuf_specific && cmd_info.manuf_specific == MANUF_CODE)
    {
      res = handle_manuf_cmd(param);
    }
  }

  TRACE_MSG(TRACE_APP1, "< zcl_specific_cluster_cmd_handler %d", (FMT__D, res));
  return res;
}

static zb_ret_t check_attr_val_app(
  zb_uint16_t cluster_id,
  zb_uint8_t cluster_role,
  zb_uint8_t endpoint,
  zb_uint16_t attr_id,
  zb_uint16_t manuf_code,
  zb_uint8_t *value)
{
  zb_ret_t ret;

  TRACE_MSG(TRACE_APP1, "> check_attr_val_app()", (FMT__0));
  TRACE_MSG(TRACE_APP1, "cluster_id 0x%x, cluster_role %hd, endpoint 0x%hx, attr_id 0x%x, manuf_code 0x%x, value 0x%hx",
            (FMT__D_H_H_D_H, cluster_id, cluster_role, endpoint, attr_id, manuf_code, *value));

  ret = ((attr_id == MANUF_CLST_ON_OFF_ATTR) && (manuf_code == MANUF_CODE)) ? RET_OK : RET_IGNORE;

  TRACE_MSG(TRACE_APP1, "< check_attr_val_app(), ret 0x%lx", (FMT__L, ret));

  return ret;
}

static zb_uint8_t disc_manuf_cmd_cb(
    zb_uint8_t direction,
    zb_uint8_t ep,
    zb_uint16_t cluster_id,
    zb_uint16_t manuf_id,
    zb_bool_t recv_cmd_type,
    zb_uint8_t **value)
{
  zb_uint8_t cmd_num = 0U;
  zb_uint8_t cmd_list[1] = { 0 };

  TRACE_MSG(TRACE_APP1, "> disc_manuf_cmd_cb()", (FMT__0));
  TRACE_MSG(TRACE_APP1, "recv_cmd_type %hd, direction %hd, ep %hd, cluster_id 0x%x, manuf_id 0x%x",
            (FMT__H_H_H_D_D, recv_cmd_type, direction, ep, cluster_id, manuf_id));

  *value = cmd_list;

  if (direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV
      && ep == ZB_OUTPUT_ENDPOINT
      && cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
  {
    /* discover received commands */
    if (recv_cmd_type == ZB_TRUE)
    {
      cmd_num = 1U;
      cmd_list[0] = MANUF_CLST_ON_OFF_CMD_SET;
    }
    /* discover generated commands */
    else
    {
      TRACE_MSG(TRACE_APP1, "there are no generated manuf commands!", (FMT__0));
    }
  }

  TRACE_MSG(TRACE_APP1, "< disc_manuf_cmd_cb(), cmd_num %hd, cmd_list %p, cmd_list[0] 0x%hx",
            (FMT__H_P_H, cmd_num, cmd_list, cmd_list[0]));

  return cmd_num;
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
        TRACE_MSG(TRACE_APP1, "Device (%d) STARTED OK", (FMT__D, device_type));
        /* [zb_get_device_type_example] */

        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        ZB_SCHEDULE_APP_ALARM(bind_on_off_cluster_delayed, 0, 12.5 * ZB_TIME_ONE_SECOND);
        break;
      }

/* [zb_bdb_finding_binding_target_usage] */
      case ZB_BDB_SIGNAL_STEERING:
        TRACE_MSG(TRACE_APP1, "Successful steering, start f&b target", (FMT__0));
        zb_bdb_finding_binding_target(ZB_OUTPUT_ENDPOINT);
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
