/*
 * Copyright 2025 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Materials") are owned by NXP ( NXP ), its
 * suppliers and/or its licensors. Title to the Materials remains with NXP,
 * its suppliers and/or its licensors. The Materials contain
 * trade secrets and proprietary and confidential information of NXP, its
 * suppliers and/or its licensors. The Materials are protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Materials may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without NXP's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by NXP in writing.
 */

#define ZB_TRACE_FILE_ID 33619
#include <ctype.h>
#include <stdlib.h>
#include "zboss_api.h"
#include "cli_nvram.h"
#include "cli_config.h"
#include "cli_menu.h"
#include "cli_tools.h"

/* Commands config handlers */
/*static zb_ret_t nvram_erase_at_start(int argc, char *argv[]);*/     static zb_ret_t help_erase_at_start(void);
#ifdef ZB_USE_NVRAM
static zb_ret_t nvram_app_register(int argc, char *argv[]);       static zb_ret_t help_app_register(void);
static zb_ret_t nvram_app_write(int argc, char *argv[]);
static zb_ret_t nvram_app_read_local(int argc, char *argv[]);
#endif /* ZB_USE_NVRAM */
static zb_ret_t nvram_buf_alloc(int argc, char *argv[]);
static zb_ret_t nvram_print(int argc, char *argv[]);

/* Menu installcode */
cli_menu_cmd menu_nvram[] = {
  /* name, args,                                 align, function,                help,                description */
  { "erase_at_start", " [erase]", "                  ", nvram_erase_at_start,    help_erase_at_start, "erase nvram at start if erase [0|1] is 1, replace config factory_reset" },
#ifdef ZB_USE_NVRAM
  { "app_register", " [dataset] [size]", "           ", nvram_app_register,      help_app_register,   "register nvram application dataset [1-4] for read/write, buffer size [size]" },
  { "app_write", " [dataset] [offset] [payload]", "  ", nvram_app_write,         help_empty,          "write to nvram application dataset [1-4] from data index [offset] payload [DD:...:DD]" },
  { "app_read", " [dataset] [offset] [len]", "       ", nvram_app_read_local,    help_empty,          "read local variable from nvram application dataset [1-4] from data index [offset] size [len]" },
#endif /* ZB_USE_NVRAM */
  { "buf_alloc", " [pool] [size] [method]", "        ", nvram_buf_alloc,         help_empty,          "test allocation io buffer from pool [out|any], size [0-max], method [get|init]" },
  { "print", " ", "                                  ", nvram_print,             help_empty,          "print nvram & buffer information" },
  /* Add new commands above here */
  { NULL, NULL,                    NULL, NULL,           NULL,        NULL }
};


/* Static command nvram
 * command erase_at_start
 *
 * nvram erase_at_start [erase]
 */
/*static */zb_ret_t nvram_erase_at_start(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_erase = 0;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [erase] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_erase);
  if(new_erase > 1)
    return RET_INVALID_PARAMETER_1;

  if(config_is_started())
    return RET_UNAUTHORIZED;

  zb_set_nvram_erase_at_start(new_erase != 0);

  return RET_OK;
}
static zb_ret_t help_erase_at_start(void)
{
  menu_printf("pre-start MANDATORY nvram: ");
  menu_printf("- 0: restart with previous configuration");
  menu_printf("- 0: restart with previous configuration");
  menu_printf("- 1: restart from factory reset");
  return RET_OK;
}


#ifdef ZB_USE_NVRAM
typedef struct {
  zb_uint16_t size;
  zb_uint8_t *data;
} app_data_info_t;
app_data_info_t app_data_info[4] = {0};

static void menu_printf_app_data(zb_uint8_t *data, zb_uint16_t offset, zb_uint16_t len, char *msg)
{
  char dataStr[128];
  int  dataLen;
  zb_uint16_t i;

  dataLen = wcs_snprintf(dataStr, sizeof(dataStr)-1, "%s[%04x] ", msg, (offset/16)*16);
  for(i=0; i<(offset%16); i++)
    dataLen += wcs_snprintf(dataStr+dataLen, sizeof(dataStr)-dataLen-1, "   ");
  for(i=offset; i<offset+len; i++)
  {
    if(i%16==0)
      dataLen = wcs_snprintf(dataStr, sizeof(dataStr)-1, "%s[%04x] ", msg, i);
    dataLen += wcs_snprintf(dataStr+dataLen, sizeof(dataStr)-dataLen-1, "%02X", data[i]);
    if(i%16 == 15 || i+1 == offset+len)
      menu_printf("\t%s", dataStr);
    else
      dataLen += wcs_snprintf(dataStr+dataLen, sizeof(dataStr)-dataLen-1, ":");
  }
}

/* Static stack callback function
 * query from the stack
 */
static zb_uint16_t get_app_size_cb(zb_uint8_t bank_idx)
{
  if(bank_idx > 3)
    return 0;

  menu_printf("nvram_get_app%u_data_size(): %u", bank_idx+1, app_data_info[bank_idx].size);

  return app_data_info[bank_idx].size;
}
static zb_uint16_t get_app1_size_cb(void) { return get_app_size_cb(0); }
static zb_uint16_t get_app2_size_cb(void) { return get_app_size_cb(1); }
static zb_uint16_t get_app3_size_cb(void) { return get_app_size_cb(2); }
static zb_uint16_t get_app4_size_cb(void) { return get_app_size_cb(3); }

static void read_app_cb(zb_uint8_t bank_idx, zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length)
{
  if(bank_idx > 3)
    return;

  if(payload_length != app_data_info[bank_idx].size)
  {
    menu_printf("nvram_read_app%d_cb(): invalid payload_len: %u, should be %u", bank_idx+1, payload_length, app_data_info[bank_idx].size);
  }
  else
  {
    zb_ret_t ret;

    ret = zb_nvram_read_data(page, pos, app_data_info[bank_idx].data, payload_length);
    if(ret != RET_OK)
    {
      menu_printf("nvram_read_app%d_cb(): read failed: %s", bank_idx+1, wcs_get_error_str(ret));
    }
    else
    {
      menu_printf("nvram_read_app%d_cb(): read OK %u bytes at page 0x%x pos 0x%x ", bank_idx+1, payload_length, page, pos);
      menu_printf_app_data(app_data_info[bank_idx].data, 0, payload_length, "READ");
    }
  }
}
static void read_app1_cb(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length) { read_app_cb(0, page, pos, payload_length); }
static void read_app2_cb(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length) { read_app_cb(1, page, pos, payload_length); }
static void read_app3_cb(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length) { read_app_cb(2, page, pos, payload_length); }
static void read_app4_cb(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length) { read_app_cb(3, page, pos, payload_length); }

static zb_ret_t write_app_cb(zb_uint8_t bank_idx, zb_uint8_t page, zb_uint32_t pos)
{
  zb_ret_t ret;

  if(bank_idx > 3)
    return RET_INVALID_PARAMETER;

  ret = zb_nvram_write_data(page, pos, app_data_info[bank_idx].data, app_data_info[bank_idx].size);
  if(ret != RET_OK)
  {
      menu_printf("nvram_write_app%d_cb(): write failed: %s", bank_idx+1, wcs_get_error_str(ret));
  }
  else
  {
      menu_printf("nvram_write_app%d_cb(): write OK %u bytes at page 0x%x pos 0x%x ", bank_idx+1, app_data_info[bank_idx].size, page, pos);
      menu_printf_app_data(app_data_info[bank_idx].data, 0, app_data_info[bank_idx].size, "WRITE");
  }

  return ret;
}
static zb_ret_t write_app1_cb(zb_uint8_t page, zb_uint32_t pos) { return write_app_cb(0, page, pos); }
static zb_ret_t write_app2_cb(zb_uint8_t page, zb_uint32_t pos) { return write_app_cb(1, page, pos); }
static zb_ret_t write_app3_cb(zb_uint8_t page, zb_uint32_t pos) { return write_app_cb(2, page, pos); }
static zb_ret_t write_app4_cb(zb_uint8_t page, zb_uint32_t pos) { return write_app_cb(3, page, pos); }


/* Static command nvram
 * command app_register
 *
 * nvram app_register [dataset] [size]
 */
static zb_ret_t nvram_app_register(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_bank = 0;
  zb_uint16_t new_size = 0;
  zb_uint8_t bank_idx;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [dataset] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_bank);
  if(new_bank < 1 || new_bank > 4)
    return RET_INVALID_PARAMETER_1;
  bank_idx = new_bank-1;

  /* get [size] */
  TOOLS_GET_ARG(ret, uint16, argv, 1, &new_size);

  if(new_size != 0 && app_data_info[bank_idx].size != 0)
    return RET_ALREADY_EXISTS;
  if(new_size == 0 && app_data_info[bank_idx].size == 0)
    return RET_OK;

  if(config_is_started())
    return RET_UNAUTHORIZED;

  if(new_size)
  {
    /* Allocate buffer */
    app_data_info[bank_idx].data = malloc(new_size);
    if(!app_data_info[bank_idx].data)
      return RET_NO_MEMORY;
    memset(app_data_info[bank_idx].data, 0xFF, new_size);
    app_data_info[bank_idx].size = new_size;

    /* Register application callback for reading/writing application data from/to NVRAM */
    switch(new_bank) {
      case 1:
        zb_nvram_register_app1_read_cb(read_app1_cb);
        zb_nvram_register_app1_write_cb(write_app1_cb, get_app1_size_cb);
        break;
      case 2:
        zb_nvram_register_app2_read_cb(read_app2_cb);
        zb_nvram_register_app2_write_cb(write_app2_cb, get_app2_size_cb);
        break;
      case 3:
        zb_nvram_register_app3_read_cb(read_app3_cb);
        zb_nvram_register_app3_write_cb(write_app3_cb, get_app3_size_cb);
        break;
      case 4:
        zb_nvram_register_app4_read_cb(read_app4_cb);
        zb_nvram_register_app4_write_cb(write_app4_cb, get_app4_size_cb);
        break;
      default:
        return RET_INVALID_PARAMETER_1;
    }
  }
  else
  {
    /* Un-Register application callback for reading/writing application data from/to NVRAM */
    switch(new_bank) {
      case 1:
        zb_nvram_register_app1_read_cb(NULL);
        zb_nvram_register_app1_write_cb(NULL, NULL);
        break;
      case 2:
        zb_nvram_register_app2_read_cb(NULL);
        zb_nvram_register_app2_write_cb(NULL, NULL);
        break;
      case 3:
        zb_nvram_register_app3_read_cb(NULL);
        zb_nvram_register_app3_write_cb(NULL, NULL);
        break;
      case 4:
        zb_nvram_register_app4_read_cb(NULL);
        zb_nvram_register_app4_write_cb(NULL, NULL);
        break;
      default:
        return RET_INVALID_PARAMETER_1;
    }

    free(app_data_info[bank_idx].data);
    app_data_info[bank_idx].size = 0;
  }

  return RET_OK;
}
static zb_ret_t help_app_register(void)
{
  menu_printf("if size is 0, it will unregister the dataset");
  return RET_OK;
}


/* Static command nvram
 * command app_write
 *
 * nvram app_write [dataset] [offset] [payload]
 */
static zb_ret_t nvram_app_write(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_bank = 0;
  zb_uint16_t new_offset = 0;
  zb_uint_t new_payload_len = 0;
  zb_uint8_t bank_idx;

  if(argc != 3)
    return RET_INVALID_PARAMETER;

  /* get [dataset] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_bank);
  if(new_bank < 1 || new_bank > 4)
    return RET_INVALID_PARAMETER_1;
  bank_idx = new_bank-1;

  /* get [offset] */
  TOOLS_GET_ARG(ret, uint16, argv, 1, &new_offset);

  /* get [payload len] */
  TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 2, &new_payload_len);
  if(new_offset + new_payload_len > app_data_info[bank_idx].size)
    return RET_INVALID_PARAMETER_2;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* get [payload data] */
  TOOLS_GET_ARG_HEX_ARRAY_DATA(ret, argv, 2, new_payload_len, &app_data_info[bank_idx].data[new_offset]);

  switch(new_bank) {
    case 1:
      /* Persist application data into NVRAM */
      ret = zb_nvram_write_dataset(ZB_NVRAM_APP_DATA1);
      break;
    case 2:
      /* Persist application data into NVRAM */
      ret = zb_nvram_write_dataset(ZB_NVRAM_APP_DATA2);
      break;
    case 3:
      /* Persist application data into NVRAM */
      ret = zb_nvram_write_dataset(ZB_NVRAM_APP_DATA3);
      break;
    case 4:
      /* Persist application data into NVRAM */
      ret = zb_nvram_write_dataset(ZB_NVRAM_APP_DATA4);
      break;
    default:
      return RET_INVALID_PARAMETER_1;
  }

  return ret;
}


/* Static command nvram
 * command app_read_local
 *
 * nvram app_read [dataset] [offset] [len]
 */
static zb_ret_t nvram_app_read_local(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_bank = 0;
  zb_uint16_t new_offset = 0;
  zb_uint16_t new_payload_len = 0;
  zb_uint8_t bank_idx;

  if(argc != 3)
    return RET_INVALID_PARAMETER;

  /* get [dataset] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_bank);
  if(new_bank < 1 || new_bank > 4)
    return RET_INVALID_PARAMETER_1;
  bank_idx = new_bank-1;

  /* get [offset] */
  TOOLS_GET_ARG(ret, uint16, argv, 1, &new_offset);

  /* get [len] */
  TOOLS_GET_ARG(ret, uint16, argv, 2, &new_payload_len);
  if(new_payload_len == 0 || new_offset + new_payload_len > app_data_info[bank_idx].size)
    return RET_INVALID_PARAMETER_3;

   menu_printf("nvram_read_app%d_local(): %u bytes at offset %u ", new_bank, new_payload_len, new_offset);
   menu_printf_app_data(app_data_info[bank_idx].data, new_offset, new_payload_len, "LOCAL");

  return RET_OK;
}
#endif /* ZB_USE_NVRAM */


/* Static command nvram
 * command buf_alloc
 *
 * nvram buf_alloc [pool] [size] [method]
 */
static zb_ret_t nvram_buf_alloc(int argc, char *argv[])
{
  zb_ret_t ret = ret;
#define TEST_BUF_POOL_OUT 0
#define TEST_BUF_POOL_ANY 1
  zb_uint8_t new_pool;
  zb_uint_t  new_size;
#define TEST_METHOD_INIT 0
#define TEST_METHOD_GET  1
  zb_uint8_t new_method;
  cli_tools_strval buf_pool_table[] = {
    { "out", TEST_BUF_POOL_OUT },
    { "any", TEST_BUF_POOL_ANY }
  };
  cli_tools_strval method_table[] = {
    { "init", TEST_METHOD_INIT },
    { "get", TEST_METHOD_GET   }
  };

  if(argc != 3)
    return RET_INVALID_PARAMETER;

  /* get [pool] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, buf_pool_table, &new_pool);

  /* get [size] */
  TOOLS_GET_ARG(ret, uint, argv, 1, &new_size);

  /* get [method] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 2, method_table, &new_method);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    void *ptr = NULL;

    switch(new_pool) {
      case TEST_BUF_POOL_OUT: buffer = zb_buf_get_out(); break;
      case TEST_BUF_POOL_ANY: buffer = zb_buf_get_any(); break;
    }
    if(buffer == ZB_BUF_INVALID)
    {
      ret = RET_NO_RESOURCES;
      menu_printf("buf_alloc(%u) ERROR: zb_buf_get_%s() failed", new_size, argv[0]);
    }
    else
    {
      menu_printf("buf_alloc(%u) buffer %u size: %u max: %u", new_size, buffer, zb_buf_len(buffer), zb_buf_get_max_size(buffer));
      switch(new_method) {
        case TEST_METHOD_INIT: ptr = zb_buf_initial_alloc(buffer, new_size);            break;
        case TEST_METHOD_GET:  ptr = zb_buf_get_tail_func(TRACE_CALL buffer, new_size); break; /* ZB_BUF_GET_PARAM */
      }
      if(ptr == NULL)
      {
        ret = RET_NO_MEMORY;
        menu_printf("buf_alloc(%u) buffer %u ERROR: zb_buf_%s() failed", new_size, buffer, argv[2]);
      }
      else
      {
        ret = RET_OK;
        menu_printf("buf_alloc(%u) buffer %u size: %u max: %u, ptr: %x", new_size, buffer, zb_buf_len(buffer), zb_buf_get_max_size(buffer), ptr);
      }
      zb_buf_free(buffer);
    }
  }

  return ret;
}


/* Static command nvram
 * command print
 *
 * nvram print
 */
static zb_ret_t nvram_print(int argc, char *argv[])
{
  ZVUNUSED(argv);
  
  if(argc != 0)
    return RET_INVALID_PARAMETER;

  menu_printf("NvRam size:       0x%x", zb_get_nvram_page_length());
  menu_printf("IoBuf hdr size:   %u", sizeof(zb_buf_hdr_t));
  menu_printf("IoBuf total size: %u", ZB_IO_BUF_SIZE);

  return RET_OK;
}

