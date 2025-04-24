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
/* PURPOSE: Level Control Client
*/

#define ZB_TRACE_FILE_ID 40062

#include "level_control_zed.h"

#if ! defined ZB_ED_FUNC
#error define ZB_ED_ROLE to compile ze sample
#endif

#define DST_ADDR g_dst_addr
#define ZB_LEVEL_CONTROL_SERVER_ENDPOINT 5
#define ZB_LEVEL_CONTROL_CLIENT_ENDPOINT 10

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ed_addr = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};

zb_uint16_t g_dst_addr = 0;

zb_int8_t g_test_step = LEVEL_CONTROL_SWITCH_SEND_MOVE_TO_LEVEL;

zb_uint16_t g_peer_revision = ZB_ZCL_CLUSTER_REV_MIN;

/* Handler for specific zcl commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

/** [COMMON_DECLARATION] */
/**
 * Declaring attributes for each cluster
 */

/* [COMMON_DECLARATION] */
/* Identify cluster attributes data */
zb_uint16_t g_attr_identify_identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

/* [BASIC_CLUSTER_COMMON] */
/* Basic cluster attributes data */
zb_uint8_t g_attr_basic_zcl_version = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_basic_zcl_version, &g_attr_basic_power_source);
/* [BASIC_CLUSTER_COMMON] */

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &g_attr_identify_identify_time);
ZB_ZCL_DECLARE_LEVEL_CONTROL_CLI_ATTRIB_LIST(level_control_cli_attr_list);
/********************* Declare device **************************/
ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_CLUSTER_LIST(level_control_switch_clusters, basic_attr_list, identify_attr_list, level_control_cli_attr_list);
ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_EP(level_control_switch_ep, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, level_control_switch_clusters);
ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_CTX(level_control_zed_ctx, level_control_switch_ep);

MAIN()
{
  ARGV_UNUSED;

  /* Trace enable */
  ZB_SET_TRACE_ON();
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("level_control_zed_auto_mode");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_ed_addr);
  zb_set_network_ed_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* Set end-device configuration parameters */
  zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
  zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));

  /** [REGISTER] */
  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&level_control_zed_ctx);
  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, zcl_specific_cluster_cmd_handler);
  /** [REGISTER] */

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

/* [zb_bdb_finding_binding_initiator_cb_example] */
static zb_bool_t finding_binding_cb(zb_int16_t status,
                                    zb_ieee_addr_t addr,
                                    zb_uint8_t ep,
                                    zb_uint16_t cluster)
{
  TRACE_MSG(TRACE_APP1, "finding_binding_cb status %d addr " TRACE_FORMAT_64 " ep %hd cluster %d",
            (FMT__D_A_H_D, status, TRACE_ARG_64(addr), ep, cluster));

  return ZB_TRUE;
}
/* [zb_bdb_finding_binding_initiator_cb_example] */

void send_read_attr(zb_uint8_t param);

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_zcl_parsed_hdr_t cmd_info;

  TRACE_MSG(TRACE_APP1, "> zcl_specific_cluster_cmd_handler", (FMT__0));

  ZB_ZCL_COPY_PARSED_HEADER(param, &cmd_info);

  ZB_ZCL_DEBUG_DUMP_HEADER(&cmd_info);
  TRACE_MSG(TRACE_APP3, "payload size: %i", (FMT__D, zb_buf_len(param)));

  TRACE_MSG(TRACE_APP1, "< zcl_specific_cluster_cmd_handler", (FMT__0));
  return ZB_FALSE;
}

void send_lvl_ctrl_cmd_cb(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control command callback", (FMT__0));
  ZB_SCHEDULE_APP_ALARM(send_read_attr, param, ZB_TIME_ONE_SECOND * 4);
}

void send_step_up(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Step command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x00, 0x32, 0x000A);
}

void send_step_down(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Step command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x01, 0x32, 0x000A);
}

void send_stop(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Stop command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb);
}

void send_move(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Move command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_REQ(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x00, 0x8);
}

void send_move_to_level(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Move to Level command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_REQ(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0xE6, 0x000A);
}

void send_step_up_zcl8(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Step command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ_ZCL8(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x00, 0x32, 0x000A, 0x00, 0x00);
}

void send_step_down_zcl8(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Step command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ_ZCL8(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x01, 0x32, 0x000A, 0x00, 0x00);
}

void send_stop_zcl8(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Stop command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ_ZCL8(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x00, 0x00);
}

void send_move_zcl8(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Move command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_REQ_ZCL8(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0x00, 0x8, 0x00, 0x00);
}

void send_move_to_level_zcl8(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APP1, "Level Control Move to Level command", (FMT__0));
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_REQ_ZCL8(param, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, send_lvl_ctrl_cmd_cb, 0xE6, 0x000A, 0x00, 0x00);
}

void read_attr_cb(zb_uint8_t param)
{
  switch(g_test_step)
  {
    case LEVEL_CONTROL_SWITCH_SEND_MOVE_TO_LEVEL:
      ZB_SCHEDULE_APP_ALARM(send_move_to_level, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_MOVE:
      ZB_SCHEDULE_APP_ALARM(send_move, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STOP:
      ZB_SCHEDULE_APP_ALARM(send_stop, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STEP_DOWN:
      ZB_SCHEDULE_APP_ALARM(send_step_down, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STEP_UP:
      ZB_SCHEDULE_APP_ALARM(send_step_up, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_MOVE_TO_LEVEL_ZCL8:
      ZB_SCHEDULE_APP_ALARM(send_move_to_level_zcl8, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_MOVE_ZCL8:
      ZB_SCHEDULE_APP_ALARM(send_move_zcl8, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STOP_ZCL8:
      ZB_SCHEDULE_APP_ALARM(send_stop_zcl8, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STEP_DOWN_ZCL8:
      ZB_SCHEDULE_APP_ALARM(send_step_down_zcl8, param, ZB_TIME_ONE_SECOND);
      break;
    case LEVEL_CONTROL_SWITCH_SEND_STEP_UP_ZCL8:
      ZB_SCHEDULE_APP_ALARM(send_step_up_zcl8, param, ZB_TIME_ONE_SECOND);
      break;
    default:
      zb_buf_free(param);
      break;
  }
  g_test_step++;
}

void send_read_attr(zb_uint8_t param)
{
  zb_uint8_t *cmd_ptr;
  ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ((param), cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
  ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID);
  ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ(
      (param), cmd_ptr, DST_ADDR, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ZB_LEVEL_CONTROL_SERVER_ENDPOINT, ZB_LEVEL_CONTROL_CLIENT_ENDPOINT,
       ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, read_attr_cb);
}

void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch(sig)
    {
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

/* [signal_first] */
      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        TRACE_MSG(TRACE_APP1, "FIRST_START: start steering", (FMT__0));
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        break;
/* [signal_first] */
/* [signal_reboot] */
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        TRACE_MSG(TRACE_APP1, "Device RESTARTED OK", (FMT__0));
        /* Do not have buttons in simulator - just start periodic on/off sending */
        //cmd_in_progress = ZB_FALSE;
        break;
/* [signal_reboot] */
/* [zb_bdb_finding_binding_initiator] */
      case ZB_BDB_SIGNAL_STEERING:
      {
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));

        TRACE_MSG(TRACE_APP1, "Successful steering, start f&b initiator", (FMT__0));
        zb_bdb_finding_binding_initiator(ZB_LEVEL_CONTROL_CLIENT_ENDPOINT, finding_binding_cb);
        break;
      }
/* [zb_bdb_finding_binding_initiator] */

      case ZB_BDB_SIGNAL_FINDING_AND_BINDING_INITIATOR_FINISHED:
      {
        zb_bufid_t buf = zb_buf_get_out();
        TRACE_MSG(TRACE_APP1, "Finding&binding done", (FMT__0));
        zb_zcl_set_backward_comp_mode(ZB_ZCL_AUTO_MODE);
        ZB_SCHEDULE_APP_CALLBACK(send_read_attr, buf);
      }
      break;

/* [signal_can_sleep] */
      case ZB_COMMON_SIGNAL_CAN_SLEEP:
      {
        /* zb_zdo_signal_can_sleep_params_t *can_sleep_params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_can_sleep_params_t); */
        break;
      }
/* [signal_can_sleep] */
      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
      {
        TRACE_MSG(TRACE_APP1, "Production config is ready", (FMT__0));
        break;
      }

      case ZB_BDB_SIGNAL_TC_REJOIN_DONE:
        TRACE_MSG(TRACE_APP1, "TC rejoin is completed successfully", (FMT__0));
        break; /* ZB_BDB_SIGNAL_TC_REJOIN_DONE */

      default:
        TRACE_MSG(TRACE_APP1, "Unknown signal %hd, do nothing", (FMT__H, sig));
    }
  }
  if (param)
  {
    zb_buf_free(param);
  }
}
