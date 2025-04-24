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
/* PURPOSE: Trust Center Swap Out ZED
*/

#define ZB_TRACE_FILE_ID 40569

#include "zboss_api.h"
#include "tcso_applications_common.h"

/**
 * Global variables definitions
 */

/* IEEE address of the device */
static zb_ieee_addr_t g_zed_ieee_addr = IEEE_ADDRESS_ZED;

static zb_char_t g_zed_installcode_str[] = APPLICATION_ZED_INSTALL_CODE;

static zb_bool_t s_tc_swapped = ZB_FALSE;

MAIN()
{
  ARGV_UNUSED;

  /* Global ZBOSS initialization */
  ZB_INIT("tcso_zed");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zed_ieee_addr);

  /* Set device role */
#ifndef ZB_APP_ENABLE_SUBGHZ_MODE
  {
    zb_set_network_ed_role(APPLICATION_CHANNEL_MASK);
  }
#else
  {
    zb_channel_list_t channel_list;
    zb_channel_list_init(channel_list);

    zb_channel_page_list_set_mask(channel_list, APPLICATION_CHANNEL_PAGE, APPLICATION_CHANNEL_MASK);
    zb_set_network_ed_role_ext(channel_list);
  }
#endif

    /* Act as Sleepy End Device */
    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    zb_set_rx_on_when_idle(ZB_FALSE);


  zb_set_nvram_erase_at_start(ZB_FALSE);

  zboss_use_r23_behavior();

  /* Set install-code */
  zb_secur_ic_str_set(g_zed_installcode_str);

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

      /* [join_done_snippet] */
      case ZB_SIGNAL_JOIN_DONE:
        if (status == 0)
        {
          TRACE_MSG(TRACE_APP1, "Joined/rejoined ok", (FMT__0));
        }
        break;
      /* [join_done_snippet] */

      /* [tclk_updated_snippet] */
      case ZB_TCLK_UPDATED_SIGNAL:
      {
        TRACE_MSG(TRACE_APP1, "ZB_TCLK_UPDATED_SIGNAL, status %hd", (FMT__H, status));
        if (s_tc_swapped)
        {
          TRACE_MSG(TRACE_APP1, "TCLK updated after TC swap", (FMT__0));
        }
        break;
      }
      /* [tclk_updated_snippet] */

      /* [tcsw_cli_sig_snippet] */
      case ZB_TC_SWAPPED_SIGNAL:
      {
        TRACE_MSG(TRACE_APP1, "ZB_TC_SWAPPED_SIGNAL, status %hd", (FMT__H, status));
        s_tc_swapped = ZB_TRUE;
      }
      break;
      /* [tcsw_cli_sig_snippet] */
  }

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}
