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

#define ZB_TRACE_FILE_ID 40566

#include "zboss_api.h"
#include "r23_applications_common.h"

/**
 * Global functions definitions
*/
static void app_start_device_interview(zb_uint8_t param, zb_uint16_t short_addr);
static void app_do_next_test_step(zb_uint8_t param);
static void app_send_configuration_req(zb_uint8_t param, zb_uint16_t short_addr);
static void app_send_beacon_survey(zb_uint8_t param, zb_uint16_t short_addr);
static void app_send_bind_req(zb_uint8_t param, zb_uint16_t short_addr);
static void app_sec_get_auth_lvl(zb_uint8_t param, zb_ieee_addr_t target_ieee_addr);
static void app_nwk_mgmt_change_channel(zb_uint8_t param, zb_channel_page_t channel_mask);
static void app_nwk_mgmt_change_panid(zb_uint8_t param);

/**
 * Global variables definitions
 */

/* IEEE address of the devices */
static zb_ieee_addr_t g_r23_zc_ieee_addr = IEEE_ADDRESS_R23_ZC;
static zb_ieee_addr_t g_r23_zed_ieee_addr = IEEE_ADDRESS_R23_ZED;
static zb_ieee_addr_t g_r22_zr_ieee_addr = IEEE_ADDRESS_R22_ZR;
static zb_ieee_addr_t g_r23_zr_ieee_addr = IEEE_ADDRESS_R23_ZR;

/* IC and passcode */
static zb_uint8_t g_r23_zed_passcode[ZB_PAKE_PASSCODE_LENGTH] = APPLICATION_R23_ZED_PASSCODE;
static zb_char_t g_r22_zr_installcode_str[] = APPLICATION_R22_ZR_INSTALL_CODE;

/* */
enum test_steps_e
{
  STEP_START = 0u,
  STEP_SET_CONFIGURATION,
  STEP_BEACON_SURVEY,
  STEP_DECOMMISSION,
  STEP_UPDATE_TCLK,
  STEP_SEC_LEVEL,
  STEP_CHANNEL_CHANGE,
  STEP_PANID_CHANGE
};
static zb_uint8_t g_current_test_step = STEP_START;

MAIN()
{
  ARGV_UNUSED;

  /* Global ZBOSS initialization */
  ZB_INIT("r23_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_r23_zc_ieee_addr);

  /* Set device role */
#ifndef ZB_APP_ENABLE_SUBGHZ_MODE
  {
    zb_set_network_coordinator_role(APPLICATION_CHANNEL_MASK);
  }
#else
  {
    zb_channel_list_t channel_list;
    zb_channel_list_init(channel_list);

    zb_channel_page_list_set_mask(channel_list, APPLICATION_CHANNEL_PAGE, APPLICATION_CHANNEL_MASK);
    zb_set_network_coordinator_role_ext(channel_list);
  }
#endif

  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* [zboss_use_r23_behavior_snippet] */

  /* zboss_use_r23_behavior() sets all key negotiation methods and PSK secrets
   * by default. That call is here just for a demo. ZBOSS in r23 mode by
   * default. */
  zboss_use_r23_behavior();

  /* [zboss_use_r23_behavior_snippet] */


  /* [zb_disable_key_neg_method_snippet] */

  /* The application can also change key negotiation methods and PSK secrets.
   * For example, we disable ZB_TLV_KEY_ECDHE_KEY_REQUEST_ZB_30.
   * It means that ZC will not accept r23+ devices without DLK support */
  zb_disable_key_neg_method(ZB_TLV_KEY_ECDHE_KEY_REQUEST_ZB_30);
  /* That call is just for a demo here. That method is already enabled by
   * zboss_use_r23_behavior() call.  */
  zb_enable_key_neg_method(ZB_TLV_KEY_ECDHE_CURVE_25519_HASH_SHA256);

  /* [zb_disable_key_neg_method_snippet] */

  /* [zb_set_supported_psk_secrets_snippet] */

  /* Rewrite supported PSK secrets bit mask. Just to demonstrate the API */
  {
    zb_uint8_t supported_secrets_mask = 0;
    ZB_TLV_PSK_SECRET_ENABLE(supported_secrets_mask, ZB_TLV_PSK_SECRET_AUTH_TOKEN);
    ZB_TLV_PSK_SECRET_ENABLE(supported_secrets_mask, ZB_TLV_PSK_SECRET_INSTALL_CODE);
    ZB_TLV_PSK_SECRET_ENABLE(supported_secrets_mask, ZB_TLV_PSK_SECRET_PAKE_PASSCODE);
    zb_set_supported_psk_secrets(supported_secrets_mask);
  }

  /* [zb_set_supported_psk_secrets_snippet] */

  /* [device_interview1_snippet] */

  /* Switch ON device interview. Now ZC app receives
   * ZB_ZDO_SIGNAL_DEVICE_READY_FOR_INTERVIEW signal and must explicitly finish
   * the interview by calling zb_tc_auth_device_after_interview() */
  zb_tc_enable_device_interview(ZB_TRUE);

  /* Initiate the stack start with starting the commissioning */
  if (zboss_start_no_autostart() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "ERROR zboss_start failed", (FMT__0));
  }
  else
  {
    /* Call the main loop */
    zboss_main_loop();
  }
  /* [device_interview1_snippet] */

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
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
      /* Set install-codes and pass-codes for joiners */
      zb_secur_ic_str_add(g_r22_zr_ieee_addr, g_r22_zr_installcode_str, NULL);
      zb_secur_ic_add(g_r23_zed_ieee_addr, ZB_IC_TYPE_PASSCODE, g_r23_zed_passcode, NULL);
      zboss_start_continue();
    break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
    {
      if (status == RET_OK)
      {
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        /* Open network for join */
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
      }
      else
      {
        TRACE_MSG(TRACE_ERROR, "Device FAILED status %d",
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

    /* [debug_broadcast_key_snippet] */
#ifdef DEBUG
    /* This signal is called when TC and Joiner established the TC link key*/
    case ZB_DEBUG_SIGNAL_TCLK_READY:
    {
      /* Broadcast APS key to decode packets in Wireshark. Only for debugging */
      zb_debug_signal_tclk_ready_params_t *params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_debug_signal_tclk_ready_params_t);
      (void)zb_debug_broadcast_aps_key(params->long_addr);
    }
    break;
#endif /* DEBUG */
    /* [debug_broadcast_key_snippet] */

    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
    {
      zb_zdo_signal_device_annce_params_t *dev_annce_params =
        ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_device_annce_params_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_DEVICE_ANNCE, short_addr 0x%x, ieee_addr " TRACE_FORMAT_64,
                (FMT__D_A, dev_annce_params->device_short_addr, TRACE_ARG_64(dev_annce_params->ieee_addr)));

      if (ZB_IEEE_ADDR_CMP(dev_annce_params->ieee_addr, g_r23_zed_ieee_addr))
      {
        /* All devices joined, let's do test procedure */
        app_do_next_test_step(param);
        param = ZB_UNDEFINED_BUFFER;
      }

      break;
    } /* ZB_ZDO_SIGNAL_DEVICE_ANNCE */

    case ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
    {
      zb_uint8_t *permit_duration = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_uint8_t);
      TRACE_MSG(TRACE_APP1, "ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS, duration: %hd", (FMT__D, (*permit_duration)));
      break;
    } /* ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS */

      /* [device_interview2_snippet] */

    case ZB_ZDO_SIGNAL_DEVICE_READY_FOR_INTERVIEW:
    {
      zb_zdo_signal_device_ready_for_interview_params_t *dev_interview_params =
        ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_device_ready_for_interview_params_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_DEVICE_READY_FOR_INTERVIEW, short_addr 0x%x, ieee_addr " TRACE_FORMAT_64,
                (FMT__D_A, dev_interview_params->short_addr, TRACE_ARG_64(dev_interview_params->long_addr)));

      if (ZB_IEEE_ADDR_CMP(dev_interview_params->long_addr, g_r23_zed_ieee_addr))
      {
        TRACE_MSG(TRACE_APP1, "Start device interview for 0x%x", (FMT__D, dev_interview_params->short_addr));

        app_start_device_interview(param, dev_interview_params->short_addr);
        param = ZB_UNDEFINED_BUFFER;
      }
      else
      {
        TRACE_MSG(TRACE_APP1, "Authenticate device 0x%x without interview", (FMT__D, dev_interview_params->short_addr));
        zb_tc_auth_device_after_interview(dev_interview_params->long_addr);
        param = ZB_UNDEFINED_BUFFER;
      }

      break;
    } /* ZB_ZDO_SIGNAL_DEVICE_READY_FOR_INTERVIEW */


    case ZB_ZDO_SIGNAL_DEVICE_INTERVIEW_FINISHED:
    {
      TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_DEVICE_INTERVIEW_FINISHED, status %hd", (FMT__H, status));
      break;
    } /* ZB_ZDO_SIGNAL_DEVICE_INTERVIEW_FINISHED */

    /* [device_interview2_snippet] */
  }

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}


/* Test procedure */
static void app_do_next_test_step(zb_uint8_t param)
{
  if (param == 0u)
  {
    zb_buf_get_out_delayed(app_do_next_test_step);
  }
  else
  {
    g_current_test_step += 1u;

    TRACE_MSG(TRACE_APP1, "app_do_next_test_step, param %hd, step %hd", (FMT__H_H, param, g_current_test_step));

    switch (g_current_test_step)
    {
      zb_uint16_t short_addr;

      case STEP_SET_CONFIGURATION:
      {
        WCS_TRACE_INFO("app_do_next_test_step : STEP_SET_CONFIGURATION");
        /* Set global configuration parameters for ZED.
         * Can be used for initial setup of the R23 device */
        short_addr = zb_address_short_by_ieee(g_r23_zed_ieee_addr);
        app_send_configuration_req(param, short_addr);
      }
      break;

      case STEP_BEACON_SURVEY:
      {
        WCS_TRACE_INFO("app_do_next_test_step : STEP_BEACON_SURVEY");
        /* Send beacon survey req to ZED.
         * Beacon survey req uses to determine how many potential parents ZED has */
        short_addr = zb_address_short_by_ieee(g_r23_zed_ieee_addr);
        app_send_beacon_survey(param, short_addr);
      }
      break;

      case STEP_DECOMMISSION:
      {
        /* Send bind req to R23 ZR.
         * Check binding with mgmt_bind_req.
         * Clear binding via decommission req.
         * Check binding with mgmt_bind_req again */
        WCS_TRACE_INFO("app_do_next_test_step : STEP_DECOMMISSION");
        short_addr = zb_address_short_by_ieee(g_r23_zr_ieee_addr);
        app_send_bind_req(param, short_addr);
      }
      break;

      case STEP_UPDATE_TCLK:
      {
        /* Update tclk for R23 ZR.
         * Authentication token will be used as PSK secret */
        WCS_TRACE_INFO("app_do_next_test_step : STEP_UPDATE_TCLK");
        if (RET_OK != zb_zdo_secur_update_device_tclk(param, g_r23_zr_ieee_addr))
        {
          TRACE_MSG(TRACE_ERROR, "zb_zdo_secur_update_device_tclk: unexpected error", (FMT__0));
        }

        /* Schedule next step */
        ZB_SCHEDULE_APP_ALARM(app_do_next_test_step, 0, 5 * ZB_TIME_ONE_SECOND);
      }
      break;

      case STEP_SEC_LEVEL:
      {
        WCS_TRACE_INFO("app_do_next_test_step : STEP_SEC_LEVEL");
        /* Get Authentication Level of R23 ZR */
        app_sec_get_auth_lvl(param, g_r23_zr_ieee_addr);
      }
      break;

      case STEP_CHANNEL_CHANGE:
      {
        zb_uint8_t channel = zb_get_current_channel();
        /* Prepare network for channel change
         * Change active channel */
        channel ++;
        if(channel == 27)
        {
          channel = 11;
        }
        WCS_TRACE_INFO("app_do_next_test_step STEP_CHANNEL_CHANGE - new channel: %d", channel);
        app_nwk_mgmt_change_channel(param, (1ul << channel));
      }
      break;

      case STEP_PANID_CHANGE:
        WCS_TRACE_INFO("app_do_next_test_step : STEP_PANID_CHANGE");
        /* Prepare network for channel change
         * Change active channel */
        app_nwk_mgmt_change_panid(param);
      break;

      default:
        zb_buf_free(param);
    }
  }
}

/* [r23_config_req_snippet] */

/* Functions for demonstrate the set configuration request API */
static void app_send_configuration_req_cb(zb_uint8_t param)
{
  zb_zdo_set_configuration_resp_t *resp = (zb_zdo_set_configuration_resp_t*)zb_buf_begin(param);

  TRACE_MSG(TRACE_APP1, "app_send_configuration_req_cb, param %hd, tsn %hd, status %hd",
              (FMT__H_H_H, param, resp->tsn, resp->status));

  ZB_SCHEDULE_APP_CALLBACK(app_do_next_test_step, param);
}


static void app_send_configuration_req(zb_uint8_t param, zb_uint16_t short_addr)
{
  zb_zdo_configuration_parameters_t *req_param;

  TRACE_MSG(TRACE_APP1, "app_send_configuration_req, param %hd, short_addr 0x%x",
    (FMT__H_D, param, short_addr));

  req_param = ZB_BUF_GET_PARAM(param, zb_zdo_configuration_parameters_t);
  req_param->dst_addr = short_addr;
  req_param->restricted_mode_enabled = 1u; /* Certain ZDO commands will not be accepted unless sent
                                              by Trust Center with APS encryption */
  req_param->require_link_key_encr_for_tk = 1u; /* Device will require that the APS Transport key command
                                                   shall be APS encrypted with the device's unique Trust Center Link Key */
  req_param->leave_req_allowed = 1u;
  req_param->disable_channel_change = 1u;  /* Forbid channel change for device */

  req_param->disable_panid_change = 1u; /* Forbid pan id change for device */

  (void)zb_zdo_send_configuration_parameters(param, app_send_configuration_req_cb);
}

/* [r23_config_req_snippet] */

/*
 * Functions for demonstrate the device interview
 */
static void app_zdo_read_node_desc_cb(zb_uint8_t param)
{
  zb_zdo_node_desc_resp_t *resp = (zb_zdo_node_desc_resp_t*)zb_buf_begin(param);

  TRACE_MSG(TRACE_APP1, ">> app_zdo_read_node_desc_cb, param %hd, short_addr 0x%x, status %hd",
    (FMT__H_D_H, param, resp->hdr.nwk_addr, resp->hdr.status));

  /* [device_interview3_snippet] */

  /* To continue the device interview, the TC can send another requests.
   * To close the device interview and reject the device do nothing - after apsSecurityTimeOutPeriod (10s by default)
   * the device will be removed automatically.
   * To close the device interview and authorize the device call zb_tc_auth_device_after_interview() .
   */
  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    zb_ieee_addr_t ieee_addr;
    if (RET_OK == zb_address_ieee_by_short(resp->hdr.nwk_addr, ieee_addr))
    {
      if (RET_OK == zb_tc_auth_device_after_interview(ieee_addr))
      {
        TRACE_MSG(TRACE_APP1, "device authorized " TRACE_FORMAT_64, (FMT__A, TRACE_ARG_64(ieee_addr)));
      }
    }
  }
  /* [device_interview3_snippet] */
  zb_buf_free(param);

  TRACE_MSG(TRACE_APP1, "<< app_zdo_read_node_desc_cb", (FMT__0));
}


static void app_zdo_read_node_desc_req(zb_uint8_t param, zb_uint16_t short_addr)
{
  zb_zdo_node_desc_req_t* req;

  TRACE_MSG(TRACE_APP1, ">> app_zdo_read_node_desc_req, param %hd, short_addr 0x%x",
    (FMT__H_D, param, short_addr));

  req = zb_buf_initial_alloc(param, sizeof(zb_zdo_node_desc_req_t));
  ZB_BZERO(req, sizeof(zb_zdo_node_desc_req_t));

  req->nwk_addr = short_addr;

  if (zb_zdo_node_desc_req(param, app_zdo_read_node_desc_cb) == ZB_ZDO_INVALID_TSN)
  {
    TRACE_MSG(TRACE_ERROR, "Can not send ZDO Simple Descriptor Request", (FMT__0));
    ZB_ASSERT(ZB_FALSE);
  }

  TRACE_MSG(TRACE_APP1, "<< app_zdo_read_node_desc_req", (FMT__0));
}


static void app_start_device_interview(zb_uint8_t param, zb_uint16_t short_addr)
{
  /* List of ZDO requests available during device interview:
     zdo network address request,
     zdo ieee address req,
     zdo node descriptor req,
     zdo power descriptor req,
     zdo simple descriptor req,
     zdo match descriptor req,
     zdo active ep req
   */
  TRACE_MSG(TRACE_APP1, ">> app_start_device_interview, param %hd, short_addr 0x%x",
    (FMT__H_D, param, short_addr));

  ZB_SCHEDULE_APP_CALLBACK2(app_zdo_read_node_desc_req, param, short_addr);

  TRACE_MSG(TRACE_APP1, "<< app_start_device_interview", (FMT__0));
}

  /* [beacon_survey_snippet] */

/*
 * Functions for demonstrate Beacon Survey
 */
static void app_beacon_survey_cb(zb_uint8_t param)
{
  zb_zdo_beacon_survey_resp_params_t *resp = (zb_zdo_beacon_survey_resp_params_t*)zb_buf_begin(param);
  zb_uindex_t i;

  TRACE_MSG(TRACE_APP1, ">> app_beacon_survey_cb param %hd status %hd panid_conflict_count %hd",
            (FMT__H_H_H, param, resp->status, resp->panid_conflict_count));

  TRACE_MSG(TRACE_APP1, "survey results: total_beacons_surveyed %hd num_cur_nwk_beacons %hd num_potential_parents_current_zbn %hd num_other_nwk_beacons %hd",
            (FMT__H_H_H_H,
             resp->results.total_beacons_surveyed,
             resp->results.num_cur_nwk_beacons,
             resp->results.num_potential_parents_current_zbn,
             resp->results.num_other_nwk_beacons));
  TRACE_MSG(TRACE_APP1, "current_parent 0x%x, lqa %hd parents_count %hd",
            (FMT__D_H_H, resp->parents.current_parent, resp->parents.current_parent_lqa, resp->parents.count_potential_parents));
  for (i = 0 ; i < resp->parents.count_potential_parents ; ++i)
  {
    TRACE_MSG(TRACE_APP1, "potential parent 0x%x, lqa %hd",
              (FMT__D_H, resp->parents.parent_list[i].device_short, resp->parents.parent_list[i].lqi));
  }

  ZB_SCHEDULE_APP_CALLBACK(app_do_next_test_step, param);
  TRACE_MSG(TRACE_APP1, "<< app_beacon_survey_cb", (FMT__0));
}


static void app_send_beacon_survey(zb_uint8_t param, zb_uint16_t short_addr)
{
  zb_zdo_mgmt_beacon_survey_param_t *req_param;
  zb_uint8_t idx;

  TRACE_MSG(TRACE_APP1, ">> app_send_beacon_survey_req, param %hd, short_addr 0x%x",
    (FMT__H_D, param, short_addr));

  req_param = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_beacon_survey_param_t);

  req_param->channel_page_cnt = ZB_SURVEY_BEACON_MAX_CHANNEL_PAGE_CNT;
  req_param->dst_addr  = short_addr;

  zb_channel_page_list_get_page_idx(APPLICATION_CHANNEL_PAGE, &idx);
  ZB_BZERO(req_param->channel_page_list, sizeof(zb_uint32_t) * ZB_SURVEY_BEACON_MAX_CHANNEL_PAGE_CNT);

  ZB_CHANNEL_PAGE_SET_PAGE(req_param->channel_page_list[idx], APPLICATION_CHANNEL_PAGE);
  ZB_CHANNEL_PAGE_SET_MASK(req_param->channel_page_list[idx], APPLICATION_CHANNEL_MASK);
  req_param->config_mask = 0x00;

  zdo_mgmt_beacon_survey_req(param, app_beacon_survey_cb);

  TRACE_MSG(TRACE_APP1, "<< app_send_beacon_survey_req", (FMT__0));
}

/* [beacon_survey_snippet] */

/*
 * Functions for demonstrate decommission API
 */
static void app_send_mgmt_bind_req2(zb_uint8_t param)
{
  zb_zdo_mgmt_bind_param_t *req = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_bind_param_t);

  TRACE_MSG(TRACE_APP1, "app_send_mgmt_bind_req2, param %hd", (FMT__H, param));

  req->start_index = 0x00;
  req->dst_addr = zb_address_short_by_ieee(g_r23_zr_ieee_addr);

  zb_zdo_mgmt_bind_req(param, app_do_next_test_step);
}


/* [decommis_snippet] */

static void app_send_decommission_req(zb_uint8_t param)
{
  zb_zdo_decommission_req_param_t *req_param;

  TRACE_MSG(TRACE_APP1, "app_send_decommission_req, param %hd", (FMT__H, param));

  /* clear buffer */
  (void)zb_buf_initial_alloc(param, 0);

  req_param = ZB_BUF_GET_PARAM(param, zb_zdo_decommission_req_param_t);
  req_param->dst_addr = zb_address_short_by_ieee(g_r23_zr_ieee_addr);
  req_param->eui64_count = 1;
  ZB_IEEE_ADDR_COPY(req_param->eui64_list[0], g_r22_zr_ieee_addr);

  /* zb_zdo_clear_all_bind_req() can be used instead of zb_zdo_decommission_req().
   * Both of these requests clear the bindings, but the ZDO Decommission request
   * also clears partner link keys for selected devices */
  if (zb_zdo_decommission_req(param, app_send_mgmt_bind_req2) == 0xffu)
  {
    zb_buf_free(param);
    TRACE_MSG(TRACE_ERROR, "unexpected error", (FMT__0));
    ZB_ASSERT(0);
  }
}

/* [decommis_snippet] */

static void app_send_mgmt_bind_req(zb_uint8_t param)
{
  zb_zdo_mgmt_bind_param_t *req = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_bind_param_t);

  TRACE_MSG(TRACE_APP1, "app_send_mgmt_bind_req, param %hd", (FMT__H, param));

  req->start_index = 0x00;
  req->dst_addr = zb_address_short_by_ieee(g_r23_zr_ieee_addr);

  zb_zdo_mgmt_bind_req(param, app_send_decommission_req);
}


static void app_send_bind_req(zb_uint8_t param, zb_uint16_t dest_addr)
{
  zb_zdo_bind_req_param_t *bind_param;
  bind_param = ZB_BUF_GET_PARAM(param, zb_zdo_bind_req_param_t);

  TRACE_MSG(TRACE_APP1, "app_send_bind_req, param %hd", (FMT__H, param));

  ZB_IEEE_ADDR_COPY(bind_param->src_address, g_r23_zr_ieee_addr);
  bind_param->cluster_id = 0x001C;
  bind_param->src_endp = 0x1;
  bind_param->dst_endp = 0x3;
  bind_param->dst_addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
  ZB_IEEE_ADDR_COPY(bind_param->dst_address.addr_long, g_r22_zr_ieee_addr);

  bind_param->req_dst_addr = dest_addr;

  zb_zdo_bind_req(param, app_send_mgmt_bind_req);
}


/* [sec_get_auth_lvl_snippet] */
/*
 * Functions for demonstrate how to get authentication level
 */
static void app_sec_get_auth_lvl_cb (zb_uint8_t param)
{
  zb_zdo_secur_get_auth_level_rsp_t *ptr;
  ptr = (zb_zdo_secur_get_auth_level_rsp_t *) zb_buf_begin(param);
  TRACE_MSG(TRACE_APP1, "app_send_get_auth_lvl_cb, param %hd:", (FMT__H, param));
  TRACE_MSG(TRACE_APP1, "# tsn = %hd, status 0x%x", (FMT__H_H, ptr->tsn, ptr->status));
  TRACE_MSG(TRACE_APP1, "# ieee_addr = "TRACE_FORMAT_64, (FMT__A, TRACE_ARG_64(ptr->target_ieee)));
  TRACE_MSG(TRACE_APP1, "# initial_join_auth = %hd, key_upd_method = %hd", (FMT__H_H, ptr->initial_join_auth, ptr->key_upd_method));

  ZB_SCHEDULE_APP_ALARM(app_do_next_test_step, param, ZB_TIME_ONE_SECOND);
}


static void app_sec_get_auth_lvl(zb_uint8_t param, zb_ieee_addr_t target_ieee_addr)
{
  zb_zdo_secur_get_auth_level_req_t *buf_params = ZB_BUF_GET_PARAM(param, zb_zdo_secur_get_auth_level_req_t);
  ZB_IEEE_ADDR_COPY(buf_params->target_ieee, target_ieee_addr);
  zb_zdo_get_auth_level_req(param, app_sec_get_auth_lvl_cb);
}
/* [sec_get_auth_lvl_snippet] */


/* [change_channel_snippet] */
/*
 * Functions for demonstrate active channel change
 */
static void app_nwk_mgmt_change_channel_cb(zb_uint8_t param)
{
  zb_channel_panid_change_preparation_t *params = ZB_BUF_GET_PARAM(param, zb_channel_panid_change_preparation_t);
  TRACE_MSG(TRACE_APP1, ">> app_nwk_mgmt_change_channel_cb, param, error cnt = %d", (FMT__D, params->error_cnt));
  if (params->error_cnt == 0)
  {
    TRACE_MSG(TRACE_APP1, "Preparation for channel change is successful, let's send channel change command", (FMT__0));
    if (RET_OK == zb_start_channel_change(param))
    {
      TRACE_MSG(TRACE_APP1, "Change channel procedure started successful", (FMT__0));
      ZB_SCHEDULE_APP_ALARM(app_do_next_test_step, 0, ZB_TIME_ONE_SECOND * 10u);
    }
  }
  TRACE_MSG(TRACE_APP1, "<< app_nwk_mgmt_change_channel_cb", (FMT__0));
}


static void app_nwk_mgmt_change_channel(zb_uint8_t param, zb_channel_page_t channel_mask)
{
  zb_channel_change_parameters_t *params = ZB_BUF_GET_PARAM(param, zb_channel_change_parameters_t);
  params->next_channel_change = channel_mask;

  /* Send set_configuration_req to all devices for allow channel change */
  if (RET_OK == zb_prepare_network_for_channel_change(param, app_nwk_mgmt_change_channel_cb))
  {
    TRACE_MSG(TRACE_APP1, "Prepare network for channel change, next channel mask: 0x%x", (FMT__D, channel_mask));
  }
}
/* [change_channel_snippet] */


/*
 * Functions for demonstrate PAN ID change
 */
static void app_broadcast_network_key (zb_uint8_t param)
{
  ZVUNUSED(param);
#ifdef DEBUG
  (void)zb_debug_broadcast_nwk_key();
#endif
}


/* [change_panid_snippet] */
static void app_nwk_mgmt_change_panid_cb(zb_uint8_t param)
{
  zb_channel_panid_change_preparation_t *params = ZB_BUF_GET_PARAM(param, zb_channel_panid_change_preparation_t);
  TRACE_MSG(TRACE_APP1, ">> app_nwk_mgmt_change_panid_cb, param, error cnt = %d", (FMT__D, params->error_cnt));
  if (params->error_cnt == 0)
  {
    TRACE_MSG(TRACE_APP1, "Preparation for channel change is successful, let's send channel change command", (FMT__0));
    if (RET_OK == zb_start_panid_change(param))
    {
      TRACE_MSG(TRACE_APP1, "Change panid procedure started successful", (FMT__0));
      /* Broadcast network key to decode packets in Wireshark after PAN ID change. Only for debugging */
      ZB_SCHEDULE_APP_ALARM(app_broadcast_network_key, 0, 10 * ZB_TIME_ONE_SECOND);
    }
  }
  TRACE_MSG(TRACE_APP1, "<< app_nwk_mgmt_change_panid_cb", (FMT__0));
}


static void app_nwk_mgmt_change_panid(zb_uint8_t param)
{
  if (param == 0u)
  {
    zb_buf_get_out_delayed(app_nwk_mgmt_change_panid);
  }
  else
  {
    zb_panid_change_parameters_t *params = ZB_BUF_GET_PARAM(param, zb_panid_change_parameters_t);
    params->next_panid_change = 0xffffu; /* The next panid value will be randomly generated */

    /* Send set_configuration_req to all devices for allow PAN ID change */
    if (RET_OK == zb_prepare_network_for_panid_change(param, app_nwk_mgmt_change_panid_cb))
    {
      TRACE_MSG(TRACE_APP1, "Prepare network for panid change", (FMT__0));
    }
  }
}

/* [change_panid_snippet] */
