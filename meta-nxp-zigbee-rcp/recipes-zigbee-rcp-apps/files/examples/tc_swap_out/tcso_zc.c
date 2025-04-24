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
 /* PURPOSE: Trust Center Swap Out ZC
*/

#define ZB_TRACE_FILE_ID 40568

#include "zboss_api.h"
#include "zboss_tcswap.h"
#include "tcso_applications_common.h"


#define TCSW_NVRAM_DATASET_SIZE 1024
#define TCSW_PORTION_SIZE 32

/**
 * Global functions definitions
*/
static zb_ret_t app_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos);
static void app_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);
static zb_uint16_t app_get_nvram_data_size(void);
static void app_restore_tc_db(void);
static void app_backup_tc_db(void);
static void app_simulate_zc_change(void);
/**
 * Global variables definitions
 */
static zb_uint8_t g_nvram_buf[TCSW_NVRAM_DATASET_SIZE] = {0};
static zb_bool_t  g_tc_db_loaded = ZB_FALSE;


/* IEEE address of the device */
static zb_ieee_addr_t g_zc_ieee_addr = IEEE_ADDRESS_ZC;
static zb_ieee_addr_t g_zed_ieee_addr = IEEE_ADDRESS_ZED;
static zb_char_t g_zed_installcode_str[] = APPLICATION_ZED_INSTALL_CODE;


MAIN()
{
  ARGV_UNUSED;

  /* Global ZBOSS initialization */
  ZB_INIT("tcso_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_ieee_addr);

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

  zboss_use_r23_behavior();

  /* We use NVRAM for TC database backup during TC swap out procedure.
   * It's only fir debug and demonstrate purposes, in production TC DB must be backed up elsewhere */
  zb_nvram_register_app1_read_cb(app_nvram_read_app_data);
  zb_nvram_register_app1_write_cb(app_nvram_write_app_data, app_get_nvram_data_size);

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
      if (g_tc_db_loaded)
      {
        app_restore_tc_db();
        app_simulate_zc_change();
      }
      zboss_start_continue();
    break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
    {
      if (status == RET_OK)
      {
        if (g_tc_db_loaded)
        {
          g_tc_db_loaded = ZB_FALSE;
#ifdef DEBUG
          /* Broadcast nwk and hashed tclk to decode packets in Wireshark. Only for debugging */
          zb_debug_broadcast_nwk_key();
          zb_debug_broadcast_aps_key(g_zed_ieee_addr);
#endif
        }
        /* Set install-codes for ZED */
        zb_secur_ic_str_add(g_zed_ieee_addr, g_zed_installcode_str, NULL);
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
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

#ifdef DEBUG
    case ZB_DEBUG_SIGNAL_TCLK_READY:
    {
      zb_debug_signal_tclk_ready_params_t *params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_debug_signal_tclk_ready_params_t);
      zb_debug_broadcast_aps_key(params->long_addr);
    }
    break;
#endif /* DEBUG */

    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
    {
      zb_zdo_signal_device_annce_params_t *dev_annce_params =
        ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_device_annce_params_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_DEVICE_ANNCE, short_addr 0x%x, ieee_addr " TRACE_FORMAT_64,
                (FMT__D_A, dev_annce_params->device_short_addr, TRACE_ARG_64(dev_annce_params->ieee_addr)));
      break;
    } /* ZB_ZDO_SIGNAL_DEVICE_ANNCE */

    case ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
    {
      zb_uint8_t *permit_duration = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_uint8_t);
      TRACE_MSG(TRACE_APP1, "ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS, duration: %hd", (FMT__D, (*permit_duration)));
      break;
    } /* ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS */

    /* [tcsw_backup1_snippet] */
    case ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL:
      TRACE_MSG(TRACE_APP1, "Signal ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL, status %d", (FMT__D, status));
      app_backup_tc_db();
    break;
    /* [tcsw_backup1_snippet] */
  }

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}


static zb_ret_t app_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos)
{
  zb_ret_t ret;

  TRACE_MSG(TRACE_APP1, ">> app_nvram_write_app_data", (FMT__0));

  ret = zb_nvram_write_data(page, pos, g_nvram_buf, TCSW_NVRAM_DATASET_SIZE);

  TRACE_MSG(TRACE_APP1, "<< app_nvram_write_app_data, ret %hd", (FMT__H, ret));

  return ret;
}


static void app_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length)
{
  TRACE_MSG(TRACE_APP1, ">> app_nvram_read_app_data", (FMT__0));
  ZVUNUSED(payload_length);
  zb_nvram_read_data(page, pos, g_nvram_buf, TCSW_NVRAM_DATASET_SIZE);
  g_tc_db_loaded = ZB_TRUE;

  TRACE_MSG(TRACE_APP1, "<< app_nvram_read_app_data", (FMT__0));
}


static zb_uint16_t app_get_nvram_data_size(void)
{
  TRACE_MSG(TRACE_APP1, "app_get_nvram_data_size", (FMT__0));
  return TCSW_NVRAM_DATASET_SIZE;
}

/* [tcsw_restore_snippet] */
static void app_restore_tc_db(void)
{
  zb_ret_t ret;
  zb_uint_t buf_offset = 0U;

  ret = zb_tcsw_start_restore_db(ZB_FALSE);

  TRACE_MSG(TRACE_APP1, ">> app_restore_tc_db", (FMT__0));

  do
  {
    ret = zc_tcsw_restore_portion((zb_uint8_t*)g_nvram_buf + buf_offset, TCSW_PORTION_SIZE);
    buf_offset += TCSW_PORTION_SIZE;
  }
  while (ret == RET_AGAIN);

  ZB_ASSERT(ret == RET_OK);

  ret = zc_tcsw_restore_finish();

  TRACE_MSG(TRACE_APP1, "<< app_restore_tc_db, ret %hd", (FMT__H, ret));
}
/* [tcsw_restore_snippet] */

static void app_simulate_zc_change(void)
{
  static zb_uint8_t new_nwk_key[16] = {0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
                                       0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab};
  static zb_ieee_addr_t new_zc_ieee_addr = IEEE_ADDRESS_ZC_SWAPPED;

  TRACE_MSG(TRACE_APP1, "app_simulate_zc_change", (FMT__0));

  ZB_PIBCACHE_PAN_ID() = 0xABAB;
  zb_set_long_address(new_zc_ieee_addr);
  zb_secur_setup_nwk_key(new_nwk_key, 0);
}

/* [tcsw_backup2_snippet] */
static void app_backup_tc_db(void)
{
  zb_uint_t portion_len;
  zb_uint_t nvram_buf_pos = 0U;
  zb_ret_t ret = zb_tcsw_start_backup_db();

  TRACE_MSG(TRACE_APP1, "app_backup_tc_db, ret %x, need_backup %hd",
    (FMT__D_H, ret, zb_tcsw_need_backup()));

  do
  {
    portion_len = zb_tcsw_get_portion((zb_uint8_t*)g_nvram_buf + nvram_buf_pos, TCSW_PORTION_SIZE);
    nvram_buf_pos += portion_len;
  }
  while (portion_len != 0);

  zb_nvram_write_dataset(ZB_NVRAM_APP_DATA1);
}
/* [tcsw_backup2_snippet] */
