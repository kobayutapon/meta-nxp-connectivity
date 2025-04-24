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
/* PURPOSE: OTA upgrade client application
*/

#include <stdio.h>
#include <limits.h> /* For PATH_MAX */
#include <time.h>

#define ZB_TRACE_FILE_ID 63256
#include "zboss_api.h"

#include "ota_client.h"
#include "ota_nxp_definitions.h"

/* Default test config, overwritten by ota-client.cfg */
/* file content:
 * # comment, ignored
 * <%04X: manufacturer>-<%04X: image_type>-<%08X: version> # any comments/description
 */
#define OTA_UPGRADE_CONFIG_FILE                "ota-client.cfg"

#define OTA_UPGRADE_DEFAULT_MANUFACTURER       ZB_MANUFACTURER_CODE_NXP
#define OTA_UPGRADE_DEFAULT_IMAGE_TYPE         OTA_UPGRADE_HEADER_IMAGE_TYPE_NXP_SCRIPT_MANUAL_TEST /* 0x1070: imx-dualpan.sh */


#define OTA_UPGRADE_DATA_SIZE 64 /* Max value accepted by the stack, refer to ZB_ZCL_OTA_UPGRADE_QUERY_IMAGE_BLOCK_DATA_SIZE_MAX */


#if ! defined ZB_ROUTER_ROLE
#error define ZB_ROUTER_ROLE to compile zr tests
#endif

/* Used endpoint */
#define ENDPOINT  10

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zr_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

typedef struct zb_zcl_ota_file_s
{
  /* OTA-FILE-<date>-<time>-<Name> */
#define OTA_NAME_MAX      9+6+1+4+1+32
  char filename[PATH_MAX+OTA_NAME_MAX+1];
  size_t written;
  size_t size;
  FILE *fp;
  struct timespec start;
  zb_zcl_ota_upgrade_file_header_t header;
  zb_zcl_ota_upgrade_file_header_optional_t optional;
} zb_zcl_ota_file_t;

zb_zcl_ota_file_t ota_rx_file;

/**
 * Declaring attributes for each cluster
 */

/* Basic cluster attributes */
/* ZCL version attribute */
zb_uint8_t g_attr_zcl_version  = ZB_ZCL_VERSION;
/* Power source attribute */
zb_uint8_t g_attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_zcl_version, &g_attr_power_source);

/* OTA Upgrade client cluster attributes */
/* UpgradeServerID attribute */
zb_ieee_addr_t upgrade_server = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE;
/* FileOffset attribute */
zb_uint32_t file_offset = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE;
/* CurrentFileVersion attribute (custom data) */
zb_uint32_t file_version = 0x00000000; /* Make sure own version is different from the server to get the file */
/* CurrentZigbeeStackVersion attribute */
zb_uint16_t stack_version = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO;
/* DownloadedFileVersion attribute */
zb_uint32_t downloaded_file_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE;
/* DownloadedZigbeeStackVersion attribute */
zb_uint16_t downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE;
/* ImageUpgradeStatus attribute */
zb_uint8_t image_status = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE;
/* Manufacturer ID attribute (custom data) */
zb_uint16_t manufacturer = OTA_UPGRADE_DEFAULT_MANUFACTURER;
/* Image Type ID attribute (custom data) */
zb_uint16_t image_type = OTA_UPGRADE_DEFAULT_IMAGE_TYPE;
/* MinimumBlockReque attribute */
zb_uint16_t min_block_reque = 0;
/* Image Stamp attribute */
zb_uint16_t image_stamp = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE;
/* Server short address attribute */
zb_uint16_t server_addr;
/* Server endpoint attribute */
zb_uint8_t server_ep;

ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST(ota_upgrade_attr_list,
    &upgrade_server, &file_offset, &file_version, &stack_version, &downloaded_file_ver,
    &downloaded_stack_ver, &image_status, &manufacturer, &image_type, &min_block_reque, &image_stamp,
    &server_addr, &server_ep, 0x0101, OTA_UPGRADE_DATA_SIZE, ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF);

/* Declare cluster list for a device */
ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_CLUSTER_LIST( ota_upgrade_client_clusters,
          basic_attr_list, ota_upgrade_attr_list);

/* Declare endpoint */
ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_EP(ota_upgrade_client_ep, ENDPOINT, ota_upgrade_client_clusters);

/* Declare application's device context for single-endpoint device */
ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_CTX(ota_upgrade_client_ctx, ota_upgrade_client_ep);

void create_ota_file_name(zb_zcl_ota_file_t *ota_file)
{
  struct timespec ts = {0};
  struct tm rtm = {0};

  clock_gettime(CLOCK_REALTIME, &ts);
  localtime_r(&ts.tv_sec, &rtm);

  snprintf(ota_file->filename, sizeof(ota_file->filename),"OTA-FILE-%02d%02d%02d-%02d%02d-%s",
    rtm.tm_mon+1,
    rtm.tm_mday,
    rtm.tm_year-100,
    rtm.tm_hour,
    rtm.tm_min,
    ota_file->header.header_string);
}

void test_device_cb(zb_uint8_t param)
{
  zb_uint32_t file_offset  = 0;
  size_t      written_size = 0;
  zb_uint8_t *file_ptr     = NULL;

  zb_zcl_device_callback_param_t *device_cb_param =
    ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);

  TRACE_MSG(TRACE_APP1, ">> test_device_cb param %hd id %hd", (FMT__H_H,
      param, device_cb_param->device_cb_id));

  device_cb_param->status = RET_OK;
  switch (device_cb_param->device_cb_id)
  {
    /* It's OTA Upgrade command */
    case ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID:
    {
      zb_zcl_ota_upgrade_value_param_t *ota_upgrade_value = &(device_cb_param->cb_param.ota_value_param);

      switch (ota_upgrade_value->upgrade_status)
      {
        case ZB_ZCL_OTA_UPGRADE_STATUS_START:

          WCS_TRACE_INFO("OTA Upgrade Start: manufacturer %04x, type %04x, version %08x, len %d",
            ota_upgrade_value->upgrade.start.manufacturer,
            ota_upgrade_value->upgrade.start.image_type,
            ota_upgrade_value->upgrade.start.file_version,
            ota_upgrade_value->upgrade.start.file_length
            );
          /* Start OTA upgrade. */
          if (image_status == ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL)
          {
            /* Accept image */
            ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
          }
          else
          {
            /* Another download is in progress, deny new image */
            ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_BUSY;
          }
          break;
        case ZB_ZCL_OTA_UPGRADE_STATUS_RECEIVE:
          /* If packet size < header size : Receive header only */
          if(ota_upgrade_value->upgrade.receive.file_offset + ota_upgrade_value->upgrade.receive.data_length < sizeof(zb_zcl_ota_upgrade_file_header_t))
          {
            zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;
            memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_upgrade_value->upgrade.receive.data_length);

            WCS_TRACE_INFO("OTA Upgrade Recv: header @%d (remains %d)",
              ota_upgrade_value->upgrade.receive.file_offset,
              ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
          }
          /* If packet size > header size : Receive header and beginnning of data */
          else if(ota_upgrade_value->upgrade.receive.file_offset == 0 && 
                  ota_upgrade_value->upgrade.receive.file_offset + ota_upgrade_value->upgrade.receive.data_length > sizeof(zb_zcl_ota_upgrade_file_header_t))
          {
            zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;
            
            memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_upgrade_value->upgrade.receive.data_length);

            create_ota_file_name(&ota_rx_file);
            ota_rx_file.fp = fopen(ota_rx_file.filename,"w+b");
            
            ota_rx_file.written = 0;
            file_offset         = 0;
            ota_rx_file.size    = ota_rx_file.header.total_image_size - ota_rx_file.header.header_length;
            written_size        = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
            file_ptr            = ota_upgrade_value->upgrade.receive.block_data + (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);            

            WCS_TRACE_INFO("OTA Upgrade Recv: header @%d (remains %d), %s @%d (remains %d)",
              ota_upgrade_value->upgrade.receive.file_offset,
              ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset,
              ota_rx_file.header.header_string,
              file_offset,
              ota_rx_file.size - file_offset);
          }
          /* If packet size < header size : Receive remaining header and beginnning of data */
          else if(ota_upgrade_value->upgrade.receive.file_offset < ota_rx_file.header.header_length)
          {
            zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;

            memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
            file_offset   = 0;
            written_size  = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
            file_ptr      = ota_upgrade_value->upgrade.receive.block_data + (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);

            create_ota_file_name(&ota_rx_file);
            ota_rx_file.fp      = fopen(ota_rx_file.filename,"w+b");
            ota_rx_file.written = 0;

            WCS_TRACE_INFO("OTA Upgrade Recv: header @%d (remains %d), %s @%d (remains %d)",
              ota_upgrade_value->upgrade.receive.file_offset,
              ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset,
              ota_rx_file.header.header_string,
              file_offset,
              ota_rx_file.size - file_offset);
          }
          /* Receive remaining of data */
          else
          {
            static size_t lastlog = 0;
            size_t percentage, step;

            file_offset   = ota_upgrade_value->upgrade.receive.file_offset - ota_rx_file.header.header_length;
            written_size  = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
            file_ptr      = ota_upgrade_value->upgrade.receive.block_data;

            percentage = (file_offset * 100) / ota_rx_file.size;
            if(ota_rx_file.size > 1024*1024) step = 1;
            else if(ota_rx_file.size > 1024) step = 10;
            else                             step = 20;

            if( percentage / step != lastlog || ota_rx_file.size - file_offset - ota_upgrade_value->upgrade.receive.data_length == 0)
            {
              struct timespec now, elapsed;
              size_t baud_rate;
              time_t eta;
              zb_uint8_t lqi = ZB_MAC_LQI_UNDEFINED;
              zb_int8_t rssi = ZB_MAC_RSSI_UNDEFINED;

              zb_zdo_get_diag_data(server_addr, &lqi, &rssi);
              clock_gettime(CLOCK_BOOTTIME, &now);
              elapsed.tv_sec = now.tv_sec - ota_rx_file.start.tv_sec;
              elapsed.tv_nsec = now.tv_nsec - ota_rx_file.start.tv_nsec;
              if(elapsed.tv_nsec < 0)
              {
                elapsed.tv_sec -= 1;
                elapsed.tv_nsec += 1000000000;
              }
              baud_rate = file_offset / elapsed.tv_sec;
              eta = (ota_rx_file.size - file_offset)/baud_rate;

              WCS_TRACE_INFO("OTA Upgrade recv: %s @%d (remains %d) %d%%  %dB/s  %02d:%02d, lqi: %hd, rssi: %hd",
                ota_rx_file.header.header_string,
                file_offset,
                ota_rx_file.size - file_offset - ota_upgrade_value->upgrade.receive.data_length, 
                percentage,
                baud_rate,
                eta/60, eta%60,
                lqi,
                rssi);

              lastlog = percentage / step;
            }
          }
          if(written_size)
          {
            if(file_offset == 0)
              clock_gettime(CLOCK_BOOTTIME, &ota_rx_file.start);

            if(file_offset != ota_rx_file.written)
            {
              WCS_TRACE_WARNING("Bad offset %s, should be %d", file_offset, ota_rx_file.written);
            }
            ota_rx_file.written += fwrite(file_ptr, 1, written_size-file_offset, ota_rx_file.fp);
            if(ota_rx_file.written != written_size)
            {
              WCS_TRACE_WARNING("Partial write: %d/%d", ota_rx_file.written, written_size);
            }
          }
          /* Process image block. */
          ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
          break;
        case ZB_ZCL_OTA_UPGRADE_STATUS_CHECK:
          /* Downloading is finished, do additional checks if needed etc before Upgrade End Request. */
          fclose(ota_rx_file.fp);
          if(ota_rx_file.written == ota_rx_file.size)
            WCS_TRACE_INFO("OTA Upgrade Check: file %s OK", ota_rx_file.filename);
          else
            WCS_TRACE_WARNING("OTA Upgrade Check: file %s is incomplete, missing %d bytes", ota_rx_file.filename, ota_rx_file.size - ota_rx_file.written);

          ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
          break;
        case ZB_ZCL_OTA_UPGRADE_STATUS_APPLY:
          WCS_TRACE_INFO("OTA Upgrade Apply");
          /* Upgrade End Resp is ok, ZCL checks for manufacturer, image type etc are ok.
             Last step before actual upgrade. */
          ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
          break;
        case ZB_ZCL_OTA_UPGRADE_STATUS_FINISH:
          WCS_TRACE_INFO("OTA Upgrade Finish");
          /* It is time to upgrade FW. */
          break;
        case ZB_ZCL_OTA_UPGRADE_STATUS_SERVER_NOT_FOUND:
          WCS_TRACE_INFO("OTA Upgrade Server not found");
          break;
        default:
          WCS_TRACE_WARNING("OTA Upgrade unknown device_cb_id %d", ota_upgrade_value->upgrade_status);
          break;
      }
    }
    break;

    default:
      device_cb_param->status = RET_NOT_IMPLEMENTED; /* Use not implemented instead of error to have in ZCL Cmd Resp Status unsup. cluster instead of hw failure */
      break;
  }

  TRACE_MSG(TRACE_APP1, "<< test_device_cb %hd", (FMT__H, device_cb_param->status));
}


static zb_bool_t read_test_param(char *config_file)
{
  zb_bool_t found = ZB_FALSE;
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  
  fp = fopen(config_file, "r");
  if(!fp)
    return found;

  while((read = getline(&line, &len, fp)) != -1)
  {
    zb_uint16_t m;
    zb_uint16_t it;
    int ret;
    
    WCS_TRACE_DEBUG("%s: %s", config_file, line);

    ret = sscanf(line, "%04hX-%04hX", &m, &it);
    if(ret == 2)
    {
      manufacturer = m;
      image_type   = it;

      WCS_TRACE_INFO("test params from %s: manufacturer: %04hX, image_type: %04hX", config_file, manufacturer, image_type);

      found = ZB_TRUE;
      break;
    }
  }

  fclose(fp);

  return found;
}

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

  /* Trace disable */
  /*  ZB_SET_TRACE_OFF(); */
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("ota_client_zr");

  if(ZB_FALSE == read_test_param(OTA_UPGRADE_CONFIG_FILE))
  WCS_TRACE_INFO("test default params: manufacturer: %04hX, image_type: %04hX", manufacturer, image_type);

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zr_addr);
  zb_set_network_router_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&ota_upgrade_client_ctx);
  /* Set Device user application callback */
  ZB_ZCL_REGISTER_DEVICE_CB(test_device_cb);

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
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, NULL);

  TRACE_MSG(TRACE_APP1, ">> zboss_signal_handler %h", (FMT__H, param));

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch(sig)
    {
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        break;

      case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        TRACE_MSG(TRACE_APP1, "Production configuration block is ready", (FMT__0));
        break;

     case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
#ifdef ZB_MAC_CONFIGURABLE_TX_POWER
        request_tx_power(zb_buf_get_any());
#endif
        /* Initialize OTA Upgrade cluster */
        ZB_SCHEDULE_APP_ALARM(zb_zcl_ota_upgrade_init_client, param, ZB_TIME_ONE_SECOND*1);
        param = 0;
        break;

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
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, ZB_GET_APP_SIGNAL_STATUS(param)));
  }

  /* Free the buffer if it is not used */
  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}
