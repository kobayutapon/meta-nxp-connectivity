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
/* PURPOSE: OTA upgrade server application
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ZB_TRACE_FILE_ID 63255
#include "zboss_api.h"

#include "ota_server.h"
#include "ota_nxp_definitions.h"

/* Use fake time in case the IMX is not synchronized by NTP */
#define OTA_UPGRADE_TEST_CURRENT_TIME       0x12345678  /* indicates the current time of the OTA server */
#define OTA_UPGRADE_TEST_UPGRADE_TIME       OTA_UPGRADE_TEST_CURRENT_TIME+1  /* indicates the time that the client SHALL upgrade to running new image */


#if ! defined ZB_COORDINATOR_ROLE
#error define ZB_COORDINATOR_ROLE to compile zc tests
#endif

/* Used endpoint */
#define ENDPOINT  5

#define MAX_OTA_CLIENTS 64
#define MAX_OTA_FILES 20

typedef struct zb_zcl_ota_file_s
{
  /* <manufacture code>-<Image Type>-<Version>-<Name>.zigbee */
#define OTA_NAME_MAX      4+1+4+1+8+1+32+1+6
  char name[PATH_MAX+OTA_NAME_MAX+1];
  int  size;
  zb_uint8_t *bufptr;
  zb_uint8_t bufsize;
  struct timespec start;
  zb_zcl_ota_upgrade_file_header_t   header;
  zb_zcl_ota_upgrade_file_header_optional_t optional;
  /* ... */
} zb_zcl_ota_file_t;

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

zb_uint16_t dst_addr;



zb_zcl_ota_file_t ota_files[MAX_OTA_FILES] = {0};
zb_uint8_t ota_nb_files = 0;


/**
 * Declaring attributes for each cluster
 */

/* Basic cluster attributes */
zb_uint8_t g_attr_zcl_version  = ZB_ZCL_VERSION;                     /* ZCL version attribute */
zb_uint8_t g_attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN;  /* Power source attribute */

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_zcl_version, &g_attr_power_source);

/* OTA Upgrade server cluster attributes */
zb_uint8_t query_jitter = ZB_ZCL_OTA_UPGRADE_QUERY_JITTER_MAX_VALUE; /* QueryJitter value */
zb_uint32_t current_time = OTA_UPGRADE_TEST_CURRENT_TIME;            /* CurrentTime */

ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST_SERVER(ota_upgrade_attr_list, &query_jitter, &current_time, MAX_OTA_FILES);

/* Declare cluster list for a device */
ZB_HA_DECLARE_OTA_UPGRADE_SERVER_CLUSTER_LIST(ota_upgrade_server_clusters,
          basic_attr_list, ota_upgrade_attr_list);

/* Declare endpoint */
ZB_HA_DECLARE_OTA_UPGRADE_SERVER_EP(ota_upgrade_server_ep, ENDPOINT, ota_upgrade_server_clusters);

/* Declare application's device context for single-endpoint device */
ZBOSS_DECLARE_DEVICE_CTX_1_EP(ota_upgrade_server_ctx, ota_upgrade_server_ep);

static void dump_ota_file(zb_zcl_ota_file_t *ota_file, char *msg)
{
  if(ota_file->header.manufacturer_code == ZB_MANUFACTURER_CODE_NXP)
  {
    if(ota_file->header.image_type  >= IMAGE_TYPE_NXP_ZBOSS_OFFSET && ota_file->header.image_type  <= IMAGE_TYPE_NXP_ZBOSS_MAX_OFFSET )
    {
      char *type = "unknown";

           if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_LIB_ZC_OFFSET)  type = "module";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_LIB_ZR_OFFSET)  type = "lib zc";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_LIB_ED_OFFSET)  type = "lib zr";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_SCRIPT_OFFSET)  type = "lib zed";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_APPLI_OFFSET)   type = "script";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_EXAMPLE_OFFSET) type = "application";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_ZBOSS_MAX_OFFSET)     type = "example";

      WCS_TRACE_INFO("%s Host %s: %s version zboss%x.%x %x.%x, size %d",
        msg,
        type,
        ota_file->header.header_string, 
        (ota_file->header.file_version>>8)&0xFF,
        (ota_file->header.file_version>>0)&0xFF,
        (ota_file->header.file_version>>24)&0xFF,
        (ota_file->header.file_version>>16)&0xFF,
        ota_file->size);
    }
    else if(ota_file->header.image_type  >= IMAGE_TYPE_NXP_IW612_OFFSET && ota_file->header.image_type  <= IMAGE_TYPE_NXP_IW612_MAX_OFFSET)
    {
      char *type = "unknown";
      char *signature = "unknown";

           if(ota_file->header.image_type < IMAGE_TYPE_NXP_IW612_SD_UART_OFFSET)   type = "WiFi";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_IW612_UART_UART_OFFSET) type = "Wifi&Bt";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_IW612_UART_SPI_OFFSET)  type = "Bt:Uart&15.4:Uart";
      else if(ota_file->header.image_type < IMAGE_TYPE_NXP_IW612_MAX_OFFSET)       type = "Bt:Uart&15.4:Spi";

      switch(ota_file->header.image_type & IMAGE_TYPE_NXP_IW612_SIGNATURE_MASK)
      {
        case IMAGE_TYPE_NXP_IW612_SIGNED_PROD_OFFSET: signature = "prod"; break;
        case IMAGE_TYPE_NXP_IW612_SIGNED_ENG_OFFSET:  signature = "dev";  break;
        case IMAGE_TYPE_NXP_IW612_UNSIGNED_OFFSET:    signature = "none"; break;
      }
 
      WCS_TRACE_INFO("%s IW612 %s, signature %s, %s firmware version %x.%x.%xp%x, size %d",
        msg,
        type,
        signature,
        ota_file->header.header_string, 
        (ota_file->header.file_version>>8)&0xFF,
        (ota_file->header.file_version>>0)&0xFF,
        (ota_file->header.file_version>>24)&0xFF,
        (ota_file->header.file_version>>16)&0xFF,
        ota_file->size);
    }
    else
    {
      WCS_TRACE_WARNING("%s NXP Unknown: type %04X, version %08X, name %s, size: %d",
        msg,
        ota_file->header.image_type, 
        ota_file->header.file_version, 
        ota_file->header.header_string,
        ota_file->size);
    }
  }
  else
  {
    WCS_TRACE_WARNING("%s Unknown manuf %04X, type %04X, version %08X, name %s, size: %d",
      msg,
      ota_file->header.manufacturer_code, 
      ota_file->header.image_type, 
      ota_file->header.file_version, 
      ota_file->header.header_string,
      ota_file->size);
  }
}

/* Create the image table */
static int create_ota_file(void)
{
  char *path2files = getenv(ZBOSS_OTA_SERVER_DIR);
  DIR *pDir;
  struct dirent *pDirent;
  
  if(!path2files)
  {
    WCS_TRACE_WARNING("%s is not defined, try %s", ZBOSS_OTA_SERVER_DIR, ZBOSS_OTA_SERVER_DEFAULT_DIR);
    path2files = ZBOSS_OTA_SERVER_DEFAULT_DIR;
  }
    
  pDir = opendir(path2files);
  if(!pDir)
  {
    WCS_TRACE_ERROR("%s is not a folder (%s), no upgrade file will be available", path2files, strerror(errno));
    return -1;
  }
  
  WCS_TRACE_INFO("search *.zigbee files in %s", path2files);
  while((pDirent = readdir(pDir)) != NULL && ota_nb_files < MAX_OTA_FILES)
  {
    char *tmp;
    zb_zcl_ota_file_t *ota_file = &ota_files[ota_nb_files];
    int ret;
    FILE *fp;

    tmp = strstr(pDirent->d_name, ".zigbee");
    if(!tmp || strlen(tmp) > strlen(".zigbee"))
      continue;

    memset(&ota_file->header, 0, sizeof(ota_file->header));
    ret = sscanf(pDirent->d_name, "%04hX-%04hX-%08X-%s.zigbee", &ota_file->header.manufacturer_code, &ota_file->header.image_type, &ota_file->header.file_version, ota_file->header.header_string);
    if(ret != 4)
    {
      WCS_TRACE_WARNING("skip %s: invalid name format", pDirent->d_name);
      continue;
    }

    if(strlen(pDirent->d_name)>= OTA_NAME_MAX)
    {
      WCS_TRACE_WARNING("skip %s: header_string too long", pDirent->d_name);
      continue;
    }
    
    snprintf(ota_file->name, sizeof(ota_file->name), "%s/%s", path2files, pDirent->d_name);

    /* Get file size */
    fp = fopen(ota_file->name, "r");
    if(!fp)
    {
      WCS_TRACE_WARNING("skip %s, cannot open it: %s", pDirent->d_name, strerror(errno));
      continue;
    }
    fseek(fp, 0L, SEEK_END);
    ota_file->size = ftell(fp);
    fclose(fp);

    /* remove .zigbee in string header */
    tmp = strstr(ota_file->header.header_string, ".zigbee");
    if(tmp) *tmp = '\0';


    /* fill out remain ota header */
    ota_file->header.file_id           = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_ID;
    ota_file->header.header_version    = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_VERSION;
    ota_file->header.header_length     = sizeof(zb_zcl_ota_upgrade_file_header_t);
    ota_file->header.fc                = 0x0000;

    /* Fill here header optional, if any */

    /* bit0: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER)
    {
      zb_uint8_t *credential_version = (zb_uint8_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);

      *credential_version = 0;// Fill here the real value
      ota_file->header.header_length += sizeof(zb_uint8_t);
    }
    /* bit1: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC)
    {
      zb_ieee_addr_t *dst_addr = (zb_ieee_addr_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);

      memset(dst_addr, 0, sizeof(zb_ieee_addr_t));// Fill here the real value
      ota_file->header.header_length += sizeof(zb_ieee_addr_t);
    }
    /* bit2: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER)
    {
      zb_uint16_t *min_hw_version = (zb_uint16_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);
      zb_uint16_t *max_hw_version = (zb_uint16_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length + sizeof(zb_uint16_t));

      *min_hw_version = 0;// Fill here the real value
      *max_hw_version = 0;// Fill here the real value
      ota_file->header.header_length += 2*sizeof(zb_uint16_t);
    }

    ota_file->header.total_image_size  = ota_file->header.header_length + ota_file->size;

    WCS_TRACE_INFO("found %s: %s, size: %d", ota_file->name, ota_file->header.header_string, ota_file->size);

    ota_nb_files++;
  }
  closedir(pDir);

  if(ota_nb_files ==0)
      WCS_TRACE_WARNING("not valid file found in %s", path2files);
  
  return ota_nb_files;
}

/* Insert upgrade OTA file into Server upgrade table */
void insert_ota_file(zb_uint8_t param)
{
  zb_ret_t ret;
  zb_uint8_t index;

  TRACE_MSG(TRACE_APP1, ">> insert_ota_file %hd", (FMT__H, param));

  for(index=0; index < ota_nb_files; index++)
  {
    zb_uint8_t bufid = zb_buf_get_any();
    ZB_ZCL_OTA_UPGRADE_INSERT_FILE(bufid, ENDPOINT, index, (zb_uint8_t*)(&ota_files[index].header), OTA_UPGRADE_TEST_UPGRADE_TIME, ZB_TRUE, ret);
    dump_ota_file(&ota_files[index], "add");
    ZB_ASSERT(ret == RET_OK);
  }
  
  /* Free the buffer since it was not used */
  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< insert_ota_file", (FMT__0));
}

char *get_src_addr(zb_zcl_addr_t *source)
{
  static char addrStr[128] = "unknown";

  switch(source->addr_type)
  {
  case ZB_ZCL_ADDR_TYPE_SHORT:      snprintf(addrStr, sizeof(addrStr), "%04X", source->u.short_addr);                       break;
  case ZB_ZCL_ADDR_TYPE_SRC_ID_GPD: snprintf(addrStr, sizeof(addrStr), "%08X", source->u.src_id);                           break;
  case ZB_ZCL_ADDR_TYPE_IEEE_GPD:   snprintf(addrStr, sizeof(addrStr), TRACE_FORMAT_64, TRACE_ARG_64(source->u.ieee_addr)); break;
  case ZB_ZCL_ADDR_TYPE_IEEE:       snprintf(addrStr, sizeof(addrStr), TRACE_FORMAT_64, TRACE_ARG_64(source->u.ieee_addr)); break;
  }

  return addrStr;
}

/* This cb is called on next image block request
   Parameters:
   - index - index of the requested image in the image table
   - zcl_hdr - ZCL hdr of the request packet
   - offset - requested offset for the file
   - size - requested block size
   - data - OUT param, pointer to the requested block

   Possible return values:
   - RET_OK - if data set to the requested block
   - RET_ERROR - if the requested block is unavailable
   - RET_BLOCKED - if the application can't serve the request on the spot, but will send
   a response later. See zb_zcl_ota_upgrade_send_image_block_response() for further details.
*/
zb_ret_t next_data_ind_cb(zb_uint8_t index,
                          zb_zcl_parsed_hdr_t* zcl_hdr,
                          zb_uint32_t offset,
                          zb_uint8_t size,
                          zb_uint8_t** data)
{
  zb_zcl_ota_file_t *ota_file;
  FILE *fp;
  zb_uint32_t file_offset = 0;
  size_t      size_to_read = 0;
  zb_uint8_t *file_ptr = NULL;
  size_t read;

  if(index >= ota_nb_files)
  {
    WCS_TRACE_ERROR("next_data_ind_cb(%d, ...) invalid index (max %d)", index, ota_nb_files);
    return RET_OUT_OF_RANGE;
  }

  ota_file = &ota_files[index];
  
  /* Buffer to be freed after ZB_ZCL_OTA_UPGRADE_REMOVE_FILE */
  if(!ota_file->bufptr)
  {
    ota_file->bufptr  = malloc(size);
    ota_file->bufsize = size;
  }
  else if(ota_file->bufsize < size)
  {
    ota_file->bufptr  = realloc(ota_file->bufptr, size);
    ota_file->bufsize = size;
  }
  
  if(!ota_file->bufptr)
  {
    WCS_TRACE_ERROR("next_data_ind_cb(%d, ...) cannot allocate %d bytes", index, size);
    return RET_NO_MEMORY;
  }

  /* Get header only */
  if(offset+size <= ota_file->header.header_length )
  {
    zb_uint8_t *src = (zb_uint8_t *)&ota_file->header;

    memcpy(ota_file->bufptr, src+offset, size);

    WCS_TRACE_INFO("OTA Upgrade file %d: deliver to %s header @%d (remains %d)",
      index,
      get_src_addr(&zcl_hdr->addr_data.common_data.source),
      offset,
      ota_file->header.header_length - offset - size);
  }
  /* Get remaining header and beginnning of data */
  else if(offset <= ota_file->header.header_length)
  {
    zb_uint8_t *src = (zb_uint8_t *)&ota_file->header;

    memcpy(ota_file->bufptr, src+offset, ota_file->header.header_length - offset);

    file_offset    = 0;
    size_to_read   = size - (ota_file->header.header_length - offset);
    file_ptr       = ota_file->bufptr + (ota_file->header.header_length - offset);

    WCS_TRACE_INFO("OTA Upgrade file %d: deliver to %s header @%d (remains %d), %s @%d (remains %d)",
      index,
      get_src_addr(&zcl_hdr->addr_data.common_data.source),
      offset,
      ota_file->header.header_length - offset - size,
      ota_file->header.header_string,
      file_offset,
      ota_file->size - file_offset - size_to_read);
  }
  /* Get remaining of data */
  else
  {
    static size_t lastlog = 0;
    size_t percentage, step;
    file_offset    = offset - ota_file->header.header_length;
    size_to_read   = size;
    file_ptr       = ota_file->bufptr;

    percentage = (file_offset * 100) / ota_file->size;
    if(ota_file->size > 1024*1024) step = 1;
    else if(ota_file->size > 1024) step = 10;
    else                           step = 20;

    if( percentage / step != lastlog || ota_file->size - file_offset - size_to_read == 0)
    {
      struct timespec now, elapsed;
      size_t baud_rate;
      time_t eta;
      zb_uint8_t lqi = ZB_MAC_LQI_UNDEFINED;
      zb_int8_t rssi = ZB_MAC_RSSI_UNDEFINED;
      zb_uint16_t short_addr= zcl_hdr->addr_data.common_data.source.u.short_addr;

      clock_gettime(CLOCK_BOOTTIME, &now);
      elapsed.tv_sec = now.tv_sec - ota_file->start.tv_sec;
      elapsed.tv_nsec = now.tv_nsec - ota_file->start.tv_nsec;
      if(elapsed.tv_nsec < 0)
      {
        elapsed.tv_sec -= 1;
        elapsed.tv_nsec += 1000000000;
      }
      baud_rate = file_offset / elapsed.tv_sec;
      eta = (ota_file->size - file_offset)/baud_rate;

      zb_zdo_get_diag_data(short_addr, &lqi, &rssi);

      WCS_TRACE_INFO("OTA Upgrade file %d: deliver to %s %s @%d (remains %d) %d%%  %dB/s  %02d:%02d, lqi: %hd, rssi: %hd",
        index,
        get_src_addr(&zcl_hdr->addr_data.common_data.source),
        ota_file->header.header_string,
        file_offset,
        ota_file->size - file_offset - size_to_read, 
        percentage,
        baud_rate,
        eta/60, eta%60,
        lqi,
        rssi);

      lastlog = percentage / step;
    }
  }

  if(size_to_read > 0)
  {
    if(file_offset == 0)
      clock_gettime(CLOCK_BOOTTIME, &ota_file->start);

    fp = fopen(ota_file->name, "r");
    if(!fp)
      return RET_NOT_FOUND;

    fseek(fp, file_offset, SEEK_SET);
    read = fread(file_ptr, 1, size_to_read, fp);
    fclose(fp);
    if(read != size_to_read)
    {
      WCS_TRACE_ERROR("next_data_ind_cb(%d, ...) cannot read %d bytes (%d)", index, size_to_read, read);
      return RET_NO_MEMORY;
    }
  }

  *data = ota_file->bufptr;

  return RET_OK;
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
  /* ZB_SET_TRACE_OFF(); */
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("ota_server_zc");

  if(create_ota_file()<=0)
    return 0;

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_addr);
  zb_set_network_coordinator_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);
  zb_nwk_set_max_ed_capacity(MAX_OTA_CLIENTS);
  zb_secur_setup_nwk_key(g_key, 0);

  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&ota_upgrade_server_ctx);

  /* Initialize OTA Upgrade cluster */
  zb_zcl_ota_upgrade_init_server(ENDPOINT, next_data_ind_cb);

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
  /* Get application signal from the buffer */
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

  TRACE_MSG(TRACE_APP1, ">> zboss_signal_handler %h", (FMT__H, param));

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
        request_tx_power(zb_buf_get_any());
#endif
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        /* Start steering */
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        /* Register upgrade OTA file */
        ZB_SCHEDULE_APP_ALARM(insert_ota_file, param, ZB_TIME_ONE_SECOND*15);
        param = 0;
        break;

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

  /* Free the buffer if it is not used */
  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<< zboss_signal_handler", (FMT__0));
}
