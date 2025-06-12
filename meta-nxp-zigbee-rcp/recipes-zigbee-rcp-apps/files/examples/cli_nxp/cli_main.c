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
/* PURPOSE: Simple app
*/

#define ZB_TRACE_FILE_ID 33616
#include <pthread.h>
#include "zboss_api.h"
#include "cli_config.h"
#include "cli_endpoint.h"
#include "cli_menu.h"

zb_ieee_addr_t g_zr_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

static void *cli_main(void *arg)
{
  menu_run();

  return arg;
}

MAIN()
{
  pthread_t cli_thread;
  zb_bool_t running = ZB_TRUE;
  zb_ret_t ret;

  ZVUNUSED(argc);

  ZB_SET_TRAF_DUMP_ON();


  ret = menu_init();
  if(ret != RET_OK)
  {
    MAIN_RETURN(1);
  }

  pthread_create(&cli_thread, NULL, cli_main, NULL);

  while(running) {
    switch(config_get_state()) {
      case STATE_INIT:
          {
            char *name = argv[0];
            char *tmp;

            tmp = strstr(name, "/");
            while(tmp)
            {
              tmp++;
              name = tmp;
              tmp = strstr(tmp, "/");
            }

            ZB_INIT(name);
            config_init_default();
          }
        break;
      case STATE_INITTING:
        sleep(1);
        if(osif_is_term_sig_received())
          running = ZB_FALSE;
        break;
      case STATE_RUN:
        ret = zboss_start_no_autostart();
        if (ret == RET_OK) {
          config_set_state(STATE_RUNNING);
        } else {
          menu_printf("FAILED to start %s", wcs_get_error_str(ret));
          config_set_state(STATE_INIT);
        }
        break;
      case STATE_RUNNING:
#ifdef ZB_ZBOSS_DEINIT
      case STATE_STOPPING:
#endif
        if(!ZB_SCHEDULER_IS_STOP())
          zboss_main_loop_iteration();
        else
          running = ZB_FALSE;
        break;
#ifdef ZB_ZBOSS_DEINIT
      case STATE_STOP:
        {
          zb_bufid_t param = zb_buf_get_out();
          if (param != ZB_BUF_INVALID)
          {
            zboss_start_shut(param);
            config_set_state(STATE_STOPPING);
          }
          else
          {
            menu_printf("FAILED to stop, no buffer available");
            config_set_state(STATE_RUNNING);
          }
        }
        break;
#endif
    }
  }

  pthread_join(cli_thread, NULL);
  menu_shutdown();

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


/**
   ZDO start/commissioning state change callback

   @param - buffer with event
 */
void zboss_signal_handler(zb_uint8_t param)
{
  /* Get application signal from the buffer */
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

  TRACE_MSG(TRACE_APP1, "> zboss_signal_handler %h", (FMT__H, param));

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    config_got_signal(sig, sg_p);

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
        break;

      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        TRACE_MSG(TRACE_APP1, "Loading application production config", (FMT__0));
        break;

#ifdef ZB_ZBOSS_DEINIT
      case ZB_SIGNAL_READY_TO_SHUT:
        /* The stask is down don't access anymore to zb_buf_xxx APIs */
        param = 0;
        break;
#endif
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

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "< zboss_signal_handler", (FMT__0));
}
