/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2023-2024 NXP
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
/* PURPOSE: Dimmable light sample (HA profile)
*/
/* [trace_file_id] */
#define ZB_TRACE_FILE_ID 40166
#include "bulb.h"
/* [trace_file_id] */
#ifdef ZB_USE_BUTTONS
#include "bulb_hal.h"
#endif
#include "zb_ha_bulb.h"

#define ZB_HA_DEFINE_DEVICE_DIMMABLE_LIGHT    /* Enable HA Dimmable Light device definitions */

#ifdef ZB_ASSERT_SEND_NWK_REPORT
void assert_indication_cb(zb_uint16_t file_id, zb_int_t line_number);
#endif

/* In current implementation, the network is re-opened on reception of permit join request signal */
#define ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK

#if !defined ZB_ROUTER_ROLE
#error define ZB_ROUTER_ROLE to build led bulb demo
#endif

#define ZCL_ONOFF_ENABLE_OPTIONAL_ATTRIBUTES 0

static void open_network(zb_uint8_t param);
static void close_network(zb_uint8_t param);
static void permit_joining(zb_uint8_t param, zb_uint8_t permit_duration);
static void permit_joining_cb(zb_uint8_t param);

void report_attribute_cb(zb_zcl_addr_t *addr, zb_uint8_t ep, zb_uint16_t cluster_id,
                              zb_uint16_t attr_id, zb_uint8_t attr_type, zb_uint8_t *value);

zb_ieee_addr_t g_zr_addr = BULB_IEEE_ADDRESS; /* IEEE address of the device */

/**
 * Declaration of Zigbee device data structures
 */
bulb_device_ctx_t g_dev_ctx; /* Global device context */

/**
 * Declaring attributes for each cluster
 */

/* Identify cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list,
                                    &g_dev_ctx.identify_attr.identify_time); /* Declaring Identify cluster attribute list */


/* Groups cluster attributes data */
zb_uint8_t g_attr_name_support = 0; /* Variable to store name_support attribute value */

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &g_attr_name_support); /* Declaring Groups cluster attribute list */

/* Scenes cluster attribute data */
zb_uint8_t g_attr_scenes_scene_count;    /* Number of scenes currently in the device's scene table */
zb_uint8_t g_attr_scenes_current_scene;  /* Scene ID of the scene last invoked */
zb_uint8_t g_attr_scenes_scene_valid;    /* Indicates whether the state of the device corresponds to
                                          * that associated with the CurrentScene and CurrentGroup attributes*/
zb_uint8_t g_attr_scenes_name_support;   /* Indicates support for scene names */
zb_uint16_t g_attr_scenes_current_group; /* Group ID of the scene last invoked */

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list,
                                  &g_attr_scenes_scene_count,
                                  &g_attr_scenes_current_scene,
                                  &g_attr_scenes_current_group,
                                  &g_attr_scenes_scene_valid,
                                  &g_attr_scenes_name_support); /* Declaring Scenes cluster attribute list */

/* Basic cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
  basic_attr_list,
  &g_dev_ctx.basic_attr.zcl_version,
  &g_dev_ctx.basic_attr.app_version,
  &g_dev_ctx.basic_attr.stack_version,
  &g_dev_ctx.basic_attr.hw_version,
  &g_dev_ctx.basic_attr.mf_name,
  &g_dev_ctx.basic_attr.model_id,
  &g_dev_ctx.basic_attr.date_code,
  &g_dev_ctx.basic_attr.power_source,
  &g_dev_ctx.basic_attr.location_id,
  &g_dev_ctx.basic_attr.ph_env,
  &g_dev_ctx.basic_attr.sw_build_id); /* Declaring Basic cluster attribute list */

/* On/Off cluster attributes data */
#if (ZCL_ONOFF_ENABLE_OPTIONAL_ATTRIBUTES == 1)
/* On/Off cluster attributes additions data */
zb_bool_t g_attr_global_scene_ctrl  = ZB_TRUE;
zb_uint16_t g_attr_on_time  = 0;
zb_uint16_t g_attr_off_wait_time  = 0;

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list, &g_dev_ctx.on_off_attr.on_off,
    &g_attr_global_scene_ctrl, &g_attr_on_time, &g_attr_off_wait_time);
#else
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list, &g_dev_ctx.on_off_attr.on_off); /* Declaring On/Off cluster attribute list */
#endif

/* Level control cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST(level_control_attr_list,
                                         &g_dev_ctx.level_control_attr.current_level,
                                         &g_dev_ctx.level_control_attr.remaining_time); /* Declaring Level control cluster attribute list */


/* Declare cluster list for a device */
ZB_HA_DECLARE_LIGHT_CLUSTER_LIST(
  dimmable_light_clusters, basic_attr_list,
  identify_attr_list,
  groups_attr_list,
  scenes_attr_list,
  on_off_attr_list,
  level_control_attr_list
);

/* Declare endpoint */
ZB_HA_DECLARE_LIGHT_EP(
  dimmable_light_ep, HA_DIMMABLE_LIGHT_ENDPOINT,
  dimmable_light_clusters);

#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
static zb_bulb_zdd_ctx_t g_zdd_ctx;

/* Basic cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
  zdd_basic_attr_list,
  &g_zdd_ctx.basic_attr.zcl_version,
  &g_zdd_ctx.basic_attr.app_version,
  &g_zdd_ctx.basic_attr.stack_version,
  &g_zdd_ctx.basic_attr.hw_version,
  &g_zdd_ctx.basic_attr.mf_name,
  &g_zdd_ctx.basic_attr.model_id,
  &g_zdd_ctx.basic_attr.date_code,
  &g_zdd_ctx.basic_attr.power_source,
  &g_zdd_ctx.basic_attr.location_id,
  &g_zdd_ctx.basic_attr.ph_env,
  &g_zdd_ctx.basic_attr.sw_build_id);

/* Identify cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(zdd_identify_attr_list,
                                    &g_zdd_ctx.identify_attr.identify_time); /* Declaring Identify cluster attribute list */

/* Zigbee Direct Configuration cluster attributes data initiated into global device context */
ZB_ZCL_DECLARE_DIRECT_CONFIGURATION_INFORMATION_ATTRIB_LIST(
    zdd_zbd_configuration_attr_list,
    &g_zdd_ctx.zbd_configuration.interface_state,
    &g_zdd_ctx.zbd_configuration.anonymous_join_timeout);

/* Declare cluster list for a device */
ZB_DECLARE_ZDD_CLUSTER_LIST(zdd_clusters,
  zdd_basic_attr_list,
  zdd_identify_attr_list,
  zdd_zbd_configuration_attr_list);

/* Declare endpoint */
ZB_DECLARE_ZDD_EP(
  zdd_ep, ZB_BULB_ZDD_ENDPOINT,
  zdd_clusters);

ZB_DECLARE_BULB_ZDD_CTX(dimmable_light_ctx, zdd_ep, dimmable_light_ep);
#else
/* Declare application's device context for single-endpoint device */
ZB_HA_DECLARE_LIGHT_CTX(
  dimmable_light_ctx,
  dimmable_light_ep);
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

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
/* [trace_64_example_variable] */
  zb_ieee_addr_t addr;        /* Local variable for IEEE address */
/* [trace_64_example_variable] */
  const zb_char_t *version;   /* Local variable for ZBOSS version */
  zb_uint8_t rx_on_when_idle; /* Local flag for rx on when idle state */
#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
  zb_ret_t zdd_init_ret; /* Local flag with ZDD initialization status */
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

  ARGV_UNUSED;

/* [switch_trace_on] */
  ZB_SET_TRACE_ON();
/* [switch_trace_on] */

  /* Uncomment to enable traffic dump */
  /* ZB_SET_TRAF_DUMP_ON(); */

  /* Global ZBOSS initialization */
/* [zboss_main_loop_init] */
  ZB_INIT("bulb");
/* [zboss_main_loop_init] */

/* HAL initialisation for hardware */
#ifdef ZB_USE_BUTTONS
  bulb_hal_init();
#endif

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zr_addr);

  /* In case of enabled ZDD role start device in unprovisioned
   * mode without pre-configured channel mask  */
#ifndef DIMMABLE_LIGHT_ENABLE_ZDD

#ifndef ZB_APP_ENABLE_SUBGHZ_MODE
  {
    zb_set_network_router_role(BULB_DEFAULT_APS_CHANNEL_MASK);
  }
#else
  {
    zb_channel_list_t channel_list;
    zb_channel_list_init(channel_list);
    zb_channel_page_list_set_mask(channel_list, (ZB_CHANNEL_LIST_PAGE28_IDX), (1l << 24));
    zb_set_network_router_role_ext(channel_list);
  }
#endif

#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

  zb_nwk_set_max_ed_capacity(64);

#ifdef ZB_USE_BUTTONS
  /* Erase NVRAM if BUTTON2 is pressed on start */
  zb_set_nvram_erase_at_start(bulb_hal_is_button_pressed(BULB_BUTTON_2_IDX));
#else
  /*
  Do not erase NVRAM to save the network parameters after device reboot or power-off
  NOTE: If this option is set to ZB_FALSE then do full device erase for all network
  devices before running other samples.
  */
  zb_set_nvram_erase_at_start(ZB_FALSE);
#endif

  bulb_device_app_init(0);

  /* Test get functions */
  version = zb_get_version(STACK_VERSION);

  /* Unused without trace. */
  ZVUNUSED(version);

  TRACE_MSG(TRACE_APP1, "ZB version is %s", (FMT__P, version));

  zb_get_long_address(addr);
/* [trace_64_example] */
  TRACE_MSG(TRACE_APP1, "Long address is " TRACE_FORMAT_64,
              (FMT__A, TRACE_ARG_64(addr)));
/* [trace_64_example] */

  rx_on_when_idle = zb_get_rx_on_when_idle();

  /* Unused without trace. */
  ZVUNUSED(rx_on_when_idle);

  TRACE_MSG(TRACE_APP1, "rx on when idle = %hd",
            (FMT__H, rx_on_when_idle));

#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
  zdd_init_ret = zbd_proxy_init(BULB_INIT_BASIC_MANUF_NAME, ZB_BULB_ZDD_ENDPOINT);
  TRACE_MSG(TRACE_APP1, "ZDD started, status %d", (FMT__D, zdd_init_ret));
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

/* [zboss_main_loop] */
  if (
#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
  /* Initiate the stack start without starting the commissioning to trigger it later by ZVD */
    zboss_start_no_autostart()
#else
  /* Initiate the stack start with starting the commissioning */
    zboss_start()
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */
      != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "ERROR dev_start failed", (FMT__0));
  }
  else
  {
    /* Call the main loop */
    zboss_main_loop();
  }
/* [zboss_main_loop] */

  /* Deinitialize trace */
  TRACE_DEINIT();

#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
  zbd_zdd_uninit();
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

  MAIN_RETURN(0);
}

void bulb_device_app_init(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, ">> bulb_device_app_init", (FMT__0));

  ZVUNUSED(param);

  /* Set Device user application callback */
  ZB_ZCL_REGISTER_DEVICE_CB(test_device_cb);
  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&dimmable_light_ctx);
  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT,
                             zcl_specific_cluster_cmd_handler);
  /* Set a callback being called for need to set attribute to default value */
  ZB_ZCL_SET_DEFAULT_VALUE_CB(zcl_reset_to_defaults_cb);

/* [zb_zcl_set_report_attr_cb] */
  /* Sets a callback being called on receive attribute report */
  ZB_ZCL_SET_REPORT_ATTR_CB(report_attribute_cb);
/* [zb_zcl_set_report_attr_cb] */

  /* Set identify notification handler for endpoint */
  ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT, bulb_do_identify);

  /* Initialization of global device context */
  bulb_app_ctx_init();

  /* Initialization of HA attributes */
  bulb_clusters_attr_init(0);

  /* Set the light level for hardware */
#ifdef ZB_USE_BUTTONS
  bulb_hal_set_level(g_dev_ctx.level_control_attr.current_level);
#endif

#ifdef ZB_USE_NVRAM
/* [register_app_nvram_cb] */
  /* Register application callback for reading application data from NVRAM */
  zb_nvram_register_app1_read_cb(bulb_nvram_read_app_data);
  /* Register application callback for writing application data to NVRAM */
  zb_nvram_register_app1_write_cb(bulb_nvram_write_app_data, bulb_get_nvram_data_size);
/* [register_app_nvram_cb] */
#endif

  TRACE_MSG(TRACE_APP1, "<< bulb_device_app_init", (FMT__0));
}

void bulb_clusters_attr_init(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, ">> bulb_clusters_attr_init", (FMT__0));
  ZVUNUSED(param);
  /* Basic cluster attributes data */
  g_dev_ctx.basic_attr.zcl_version  = ZB_ZCL_VERSION;
  g_dev_ctx.basic_attr.app_version = BULB_INIT_BASIC_APP_VERSION;
  g_dev_ctx.basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
  g_dev_ctx.basic_attr.hw_version = BULB_INIT_BASIC_HW_VERSION;

  ZB_ZCL_SET_STRING_VAL(
    g_dev_ctx.basic_attr.mf_name,
    BULB_INIT_BASIC_MANUF_NAME,
    ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

  ZB_ZCL_SET_STRING_VAL(
    g_dev_ctx.basic_attr.model_id,
    BULB_INIT_BASIC_MODEL_ID,
    ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

  ZB_ZCL_SET_STRING_VAL(
    g_dev_ctx.basic_attr.date_code,
    BULB_INIT_BASIC_DATE_CODE,
    ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));


  g_dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;

  ZB_ZCL_SET_STRING_VAL(
    g_dev_ctx.basic_attr.location_id,
    BULB_INIT_BASIC_LOCATION_ID,
    ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_ID));


  g_dev_ctx.basic_attr.ph_env = BULB_INIT_BASIC_PH_ENV;
  ZB_ZCL_SET_STRING_LENGTH(g_dev_ctx.basic_attr.sw_build_id, 0);

  /* Identify cluster attributes data */
  g_dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

  /* On/Off cluster attributes data */
  g_dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;

  /* Level control cluster attributes data */
  g_dev_ctx.level_control_attr.current_level = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE;
  g_dev_ctx.level_control_attr.remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

  TRACE_MSG(TRACE_APP1, "<< bulb_clusters_attr_init", (FMT__0));
}

void bulb_app_ctx_init(void)
{
  TRACE_MSG(TRACE_APP1, ">> bulb_app_ctx_init", (FMT__0));

  ZB_MEMSET(&g_dev_ctx, 0, sizeof(g_dev_ctx));

  TRACE_MSG(TRACE_APP1, "<< bulb_app_ctx_init", (FMT__0));
}

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_bufid_t zcl_cmd_buf = param;
  zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

  TRACE_MSG(TRACE_APP1, ">> zcl_specific_cluster_cmd_handler %i", (FMT__H, param));

  ZB_ZCL_DEBUG_DUMP_HEADER(cmd_info);
  TRACE_MSG(TRACE_APP1, "payload size: %i", (FMT__D, zb_buf_len(zcl_cmd_buf)));

  WCS_TRACE_DBGREL("cmd_handler() seq_num %d, %s, %s, cluster %s, profile %04x",
      cmd_info->seq_number,
      (cmd_info->cmd_direction)?("ToCli"):("ToSrv"),
      get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id),
      get_cluster_id_str(cmd_info->cluster_id),
      cmd_info->profile_id);

  TRACE_MSG(TRACE_APP1, "<< zcl_specific_cluster_cmd_handler", (FMT__0));

  return ZB_FALSE;
}

static void open_network(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, ">> open_network, param %hd", (FMT__H, param));

  if (param)
  {
    permit_joining(param, 0xFEU);
  }
  else
  {
    zb_buf_get_out_delayed(open_network);
  }

  TRACE_MSG(TRACE_APP1, "<< open_network", (FMT__0));
}

static void close_network(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, ">> close_network, param %hd", (FMT__H, param));

  if (param)
  {
    permit_joining(param, 0U);
  }
  else
  {
    zb_buf_get_out_delayed(close_network);
  }

  TRACE_MSG(TRACE_APP1, "<< close_network", (FMT__0));
}

static void permit_joining(zb_uint8_t param, zb_uint8_t permit_duration)
{
  zb_zdo_mgmt_permit_joining_req_param_t *req_param;

  TRACE_MSG(TRACE_APP1, ">> permit_joining, param %hd, permit_duration %hd", (FMT__H_H, param, permit_duration));

  req_param = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_permit_joining_req_param_t);
  req_param->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
  req_param->permit_duration = permit_duration;
  req_param->tc_significance = 1;

  zb_zdo_mgmt_permit_joining_req(param, permit_joining_cb);

  TRACE_MSG(TRACE_APP1, "<< permit_joining", (FMT__0));
}

static void permit_joining_cb(zb_uint8_t param)
{
  TRACE_MSG(TRACE_ERROR, "permit joining done", (FMT__0));
  zb_buf_free(param);
}

/* Callback to handle the stack events */
void zboss_signal_handler(zb_uint8_t param)
{
  /* Get application signal from the buffer */
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
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
#ifdef ZB_MAC_CONFIGURABLE_TX_POWER
        request_tx_power(param);
        param = 0;
#endif
#ifdef ZB_USE_BUTTONS
        bulb_hal_set_connect(ZB_TRUE);
#endif
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
#ifndef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
        /* Close network to force ZED to join to ZC for the 1st time */
        close_network(0U);
        /* Open network after 10s to allow ZED to rejoin to us */
        ZB_SCHEDULE_APP_ALARM(open_network, 0U, 10 * ZB_TIME_ONE_SECOND);
#endif
        break;

      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        TRACE_MSG(TRACE_APP1, "Loading application production config", (FMT__0));
        break;

      case ZB_ZDO_SIGNAL_RECV_PERMIT_JOIN_REQ:
        {
          zb_zdo_signal_recv_permit_join_req_params_t *params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_recv_permit_join_req_params_t);
          WCS_TRACE_DBGREL("RECV_PERMIT_JOIN_REQ from %04x, duration %d", params->src_addr, params->duration);
#ifdef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
          permit_joining(param, params->duration);
          param = 0;
#endif
        }
        break;
      case ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
      {
        zb_uint8_t *permit_duration = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_uint8_t);
        TRACE_MSG(TRACE_APP1, "ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS, duration: %hd", (FMT__D, (*permit_duration)));
#ifndef ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK
        /* close the nwk */
        if (*permit_duration != 0)
        {
          zb_zdo_mgmt_permit_joining_req_param_t *request_param;
          request_param = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_permit_joining_req_param_t);
          ZB_BZERO(request_param, sizeof(zb_zdo_mgmt_permit_joining_req_param_t));
          request_param->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
          request_param->permit_duration = 0;
          request_param->tc_significance = 1;

          (void)zb_zdo_mgmt_permit_joining_req(param, NULL);
          param = 0;
        }
#endif
        break;
      } /* ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS */

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
#ifdef ZB_USE_BUTTONS
    bulb_hal_set_connect(ZB_FALSE);
#endif

    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, ZB_GET_APP_SIGNAL_STATUS(param)));
  }

  /* Free the buffer if it is not used */
  if (param)
  {
    zb_buf_free(param);
  }
}

zb_ret_t level_control_set_level(zb_uint8_t new_level)
{
  TRACE_MSG(TRACE_APP1, ">> level_control_set_level", (FMT__0));

  g_dev_ctx.level_control_attr.current_level = new_level;

#ifdef ZB_USE_BUTTONS
  bulb_hal_set_level(new_level);
#endif

  TRACE_MSG(TRACE_APP1, "New level is %i", (FMT__H, new_level));

  TRACE_MSG(TRACE_APP1, "<< level_control_set_level", (FMT__0));

  return RET_OK;
}

void zcl_reset_to_defaults_cb(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
  TRACE_MSG(TRACE_APP1, ">> zcl_reset_to_defaults_cb", (FMT__0));

  /* Reset zcl attrs to default values */
  zb_zcl_init_reporting_info();
  zb_zcl_reset_reporting_ctx();

/* [nvram_usage_example] */
  /* If we fail, trace is given and assertion is triggered */
  /* Write to NVRAM HA profile Zigbee data */
  (void)zb_nvram_write_dataset(ZB_NVRAM_HA_DATA);
  /* Write to NVRAM ZCL reporting data */
  (void)zb_nvram_write_dataset(ZB_NVRAM_ZCL_REPORTING_DATA);
/* [nvram_usage_example] */

  TRACE_MSG(TRACE_APP1, "<< zcl_reset_to_defaults_cb", (FMT__0));
}

/* [zb_zcl_set_report_attr_cb_example] */
void report_attribute_cb(zb_zcl_addr_t *addr, zb_uint8_t ep, zb_uint16_t cluster_id,
                              zb_uint16_t attr_id, zb_uint8_t attr_type, zb_uint8_t *value)
{
  ZVUNUSED(ep);
  ZVUNUSED(attr_type);
  ZVUNUSED(value);

  /* Unused without trace. */
  ZVUNUSED(addr);
  ZVUNUSED(cluster_id);
  ZVUNUSED(attr_id);

  TRACE_MSG(TRACE_APP1, ">> report_attribute_cb addr %d ep %hd, cluster 0x%x, attr %d",
            (FMT__D_H_D_D, addr->u.short_addr, ep, cluster_id, attr_id));
  TRACE_MSG(TRACE_APP1, "<< report_attribute_cb", (FMT__0));
}
/* [zb_zcl_set_report_attr_cb_example] */

void test_device_cb(zb_uint8_t param)
{
  zb_bufid_t buffer = param;
  zb_zcl_device_callback_param_t *device_cb_param =
    ZB_BUF_GET_PARAM(buffer, zb_zcl_device_callback_param_t);

  TRACE_MSG(TRACE_APP1, ">> test_device_cb param %hd id %hd", (FMT__H_H,
      param, device_cb_param->device_cb_id));

  device_cb_param->status = RET_OK;
  switch (device_cb_param->device_cb_id)
  {
    /* Modify ZCL Level Control cluster attribute */
    case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:

      WCS_TRACE_DBGREL("%s(ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID) %d", __FUNCTION__, device_cb_param->cb_param.level_control_set_value_param.new_value);
      TRACE_MSG(TRACE_APP1, "Level control setting", (FMT__0));
      device_cb_param->status =
        level_control_set_level(device_cb_param->cb_param.level_control_set_value_param.new_value);

#ifdef ZB_USE_NVRAM
/* [app_nvram_usage] */
      /* If we fail, trace is given and assertion is triggered */
      /* Persist application data into NVRAM */
      (void)zb_nvram_write_dataset(ZB_NVRAM_APP_DATA1);
/* [app_nvram_usage] */
#endif
      break;

    /* Handle attribute values modification */
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
      WCS_TRACE_DBGREL("%s(ZB_ZCL_SET_ATTR_VALUE_CB_ID) %d/%d", __FUNCTION__, device_cb_param->cb_param.set_attr_value_param.values.data8, device_cb_param->cb_param.set_attr_value_param.values.data16);
      TRACE_MSG(TRACE_APP1, "on/off setting to %hd", (FMT__H, device_cb_param->cb_param.set_attr_value_param.values.data8));

      if (device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
      {
        if (device_cb_param->cb_param.set_attr_value_param.attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
          g_dev_ctx.on_off_attr.on_off = (zb_bool_t)device_cb_param->cb_param.set_attr_value_param.values.data8;
        }
      }
      else if (device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
      {
        if (device_cb_param->cb_param.set_attr_value_param.attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
        {
          g_dev_ctx.level_control_attr.current_level = device_cb_param->cb_param.set_attr_value_param.values.data16;
        }
      }
      else
      {
        /* Other clusters can be processed here */
      }

      if (device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF ||
          device_cb_param->cb_param.set_attr_value_param.cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
      {
        if (g_dev_ctx.on_off_attr.on_off)
        {
#ifdef ZB_USE_BUTTONS
          bulb_hal_set_on_off(ZB_TRUE);
          bulb_hal_set_level(g_dev_ctx.level_control_attr.current_level);
#endif
        }
        else
        {
#ifdef ZB_USE_BUTTONS
          bulb_hal_set_on_off(ZB_FALSE);
#endif
        }
#ifdef ZB_USE_NVRAM
        /* If we fail, trace is given and assertion is triggered */
        (void)zb_nvram_write_dataset(ZB_NVRAM_APP_DATA1);
#endif
      }

      break;

#ifdef DIMMABLE_LIGHT_ENABLE_ZDD
    case ZB_ZCL_DIRECT_CONFIGURATION_CONFIGURE_INTERFACE_CB_ID:
    {
      const zb_zcl_direct_configuration_configure_interface_req_t *configure_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param,
        zb_zcl_direct_configuration_configure_interface_req_t);

      TRACE_MSG(TRACE_APP1, "Configure Interface Request: current_state_bitmask %hd, requested_state %hd",
        (FMT__H_H, g_zdd_ctx.zbd_configuration.interface_state, configure_req->interface_state));

      device_cb_param->status = RET_OK;

      switch (configure_req->interface_state)
      {
        case ZB_ZCL_DIRECT_CONFIGURATION_INTERFACE_STATE_ON:
          zbd_proxy_enable_ble_interface();
          break;

        case ZB_ZCL_DIRECT_CONFIGURATION_INTERFACE_STATE_OFF:
          /* Zigbee Direct spec. 13.3.5.4.2
           * When the Zigbee Direct Interface is disabled using this command, the following SHALL happen:
           * 1) any new connections to the services defined for Zigbee Direct via Bluetooth Low Energy
           *    ([Zigbee Direct Specification 7.7]) SHALL be ignored
           * 2) existing connections to the services defined for Zigbee Direct via Bluetooth Low Energy
           *    ([Zigbee Direct Specification 7.7]) SHALL be dropped
           * */
          zbd_proxy_disable_ble_interface(ZB_TRUE);
          break;

        default:
          device_cb_param->status = RET_ERROR;
          ZB_ASSERT(ZB_FALSE);
          break;
      }
    }
    break;
#endif /* DIMMABLE_LIGHT_ENABLE_ZDD */

    default:
      device_cb_param->status = RET_NOT_IMPLEMENTED; /* Use not implemented instead of error to have in ZCL Cmd Resp Status unsup. cluster instead of hw failure */
     break;
  }

  TRACE_MSG(TRACE_APP1, "<< test_device_cb %hd", (FMT__H, device_cb_param->status));
}

void bulb_identify_led(zb_uint8_t led_state)
{
  TRACE_MSG(TRACE_APP1, "bulb_identify_led %hd", (FMT__H, led_state));
#ifdef ZB_USE_BUTTONS
  bulb_hal_set_on_off((zb_bool_t)led_state);
  if (led_state)
  {
    bulb_hal_set_level(g_dev_ctx.level_control_attr.current_level);
  }
#endif
  ZB_SCHEDULE_APP_ALARM(bulb_identify_led, !led_state, ZB_TIME_ONE_SECOND / 2);
}

void bulb_do_identify(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, ">> bulb_do_identify %hd", (FMT__H, param));

  ZB_SCHEDULE_APP_ALARM_CANCEL(bulb_identify_led, ZB_ALARM_ANY_PARAM);

  if (param == 1)
  {
    /* start identifying */
    bulb_identify_led(!g_dev_ctx.on_off_attr.on_off);
  }
  else
  {
    /* stop identifying, return led to the previous mode */
#ifdef ZB_USE_BUTTONS
    bulb_hal_set_on_off(g_dev_ctx.on_off_attr.on_off);
    if (g_dev_ctx.on_off_attr.on_off)
    {
      bulb_hal_set_level(g_dev_ctx.level_control_attr.current_level);
    }
#endif
  }

  TRACE_MSG(TRACE_APP1, "<< bulb_do_identify", (FMT__0));
}

#ifdef ZB_USE_NVRAM
/* [app_nvram_cb_implementation] */
zb_uint16_t bulb_get_nvram_data_size(void)
{
  TRACE_MSG(TRACE_APP1, "bulb_get_nvram_data_size, ret %hd", (FMT__H, sizeof(bulb_device_nvram_dataset_t)));
  return sizeof(bulb_device_nvram_dataset_t);
}

void bulb_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length)
{
  bulb_device_nvram_dataset_t ds;
  zb_ret_t ret;
/* [trace_msg] */
  TRACE_MSG(TRACE_APP1, ">> bulb_nvram_read_app_data page %hd pos %d", (FMT__H_D, page, pos));
/* [trace_msg] */
  ZB_ASSERT(payload_length == sizeof(ds));

  ret = zb_nvram_read_data(page, pos, (zb_uint8_t*)&ds, sizeof(ds));

  if (ret == RET_OK)
  {
    zb_zcl_attr_t *attr_desc;

    attr_desc = zb_zcl_get_attr_desc_a(HA_DIMMABLE_LIGHT_ENDPOINT,ZB_ZCL_CLUSTER_ID_ON_OFF,
                                       ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID);

    ZB_ZCL_SET_DIRECTLY_ATTR_VAL8(attr_desc, ds.onoff_state);

    attr_desc = zb_zcl_get_attr_desc_a(HA_DIMMABLE_LIGHT_ENDPOINT, ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                                       ZB_ZCL_CLUSTER_SERVER_ROLE,ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID);

    ZB_ZCL_SET_DIRECTLY_ATTR_VAL16(attr_desc, ds.current_level);

    if (ds.onoff_state)
    {
#ifdef ZB_USE_BUTTONS
      bulb_hal_set_level(ds.current_level);
#endif
    }
    else
    {
#ifdef ZB_USE_BUTTONS
      bulb_hal_set_on_off(ZB_FALSE);
#endif
    }
  }

  TRACE_MSG(TRACE_APP1, "<< bulb_nvram_read_app_data ret %d", (FMT__D, ret));
}

zb_ret_t bulb_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos)
{
  zb_ret_t ret;
  bulb_device_nvram_dataset_t ds;
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_DIMMABLE_LIGHT_ENDPOINT,ZB_ZCL_CLUSTER_ID_ON_OFF,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE,ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID);

  TRACE_MSG(TRACE_APP1, ">> bulb_nvram_write_app_data, page %hd, pos %d", (FMT__H_D, page, pos));

  ds.onoff_state = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);

  attr_desc = zb_zcl_get_attr_desc_a(HA_DIMMABLE_LIGHT_ENDPOINT, ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                                     ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID);

  ds.current_level = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);

  ret = zb_nvram_write_data(page, pos, (zb_uint8_t*)&ds, sizeof(ds));

  TRACE_MSG(TRACE_APP1, "<< bulb_nvram_write_app_data, ret %d", (FMT__D, ret));

  return ret;
}
/* [app_nvram_cb_implementation] */
#endif /* ZB_USE_NVRAM */
