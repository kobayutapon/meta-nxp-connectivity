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

#define ZB_TRACE_FILE_ID 33615
#include <ctype.h>
#include <stdlib.h>
#include "zboss_api.h"
#include "cli_config.h"
#include "cli_menu.h"
#include "cli_tools.h"

#define TRACE_FORMAT_IC_48 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx CRC %02hx:%02hx"
#define TRACE_FORMAT_IC_64 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx CRC %02hx:%02hx"
#define TRACE_FORMAT_IC_96 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx CRC %02hx:%02hx"
#define TRACE_FORMAT_IC_128 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx CRC %02hx:%02hx"

#define TRACE_ARG_IC_48(a) (zb_uint8_t)((a)[0]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[5]), (zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[7])
#define TRACE_ARG_IC_64(a) (zb_uint8_t)((a)[0]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[5]),(zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[7]),(zb_uint8_t)((a)[8]),(zb_uint8_t)((a)[9])
#define TRACE_ARG_IC_96(a) (zb_uint8_t)((a)[0]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[5]),(zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[7]),(zb_uint8_t)((a)[8]),(zb_uint8_t)((a)[9]),(zb_uint8_t)((a)[10]),(zb_uint8_t)((a)[11]),(zb_uint8_t)((a)[12]),(zb_uint8_t)((a)[13])
#define TRACE_ARG_IC_128(a) (zb_uint8_t)((a)[0]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[5]),(zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[7]),(zb_uint8_t)((a)[8]),(zb_uint8_t)((a)[9]),(zb_uint8_t)((a)[10]),(zb_uint8_t)((a)[11]),(zb_uint8_t)((a)[12]),(zb_uint8_t)((a)[13]),(zb_uint8_t)((a)[14]),(zb_uint8_t)((a)[15]),(zb_uint8_t)((a)[16]),(zb_uint8_t)((a)[17])


/* Commands config handlers */
#if defined(ZB_COORDINATOR_ROLE)
static zb_ret_t ic_policy(int argc, char *argv[]);
static zb_ret_t ic_add(int argc, char *argv[]);           static zb_ret_t help_add(void);
static zb_ret_t ic_remove(int argc, char *argv[]);
static zb_ret_t ic_list(int argc, char *argv[]);
#endif
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
static zb_ret_t ic_set(int argc, char *argv[]);           static zb_ret_t help_set(void);
#endif

/* Menu installcode */
cli_menu_cmd menu_installcode[] = {
  /* name, args,                align, function,       help,         description */
#if defined(ZB_COORDINATOR_ROLE)
  { "policy", " [check]", "           ", ic_policy,      help_empty,  "set installcode policy check: [enable|disable], ZC only" },
  { "add", " [type] [code] [ieee]", " ", ic_add,         help_add,    "add installcode type: [48|64|96|128|passcode|hashed] code [IC:...:IC] for device addr [AA:AA:AA:AA:AA:AA:AA:AA], ZC only" },
  { "remove", " [ieee]", "            ", ic_remove,      help_empty,  "remove installcode for device addr [ieee|all], ZC only" },
  { "list", "", "                     ", ic_list,        help_empty,  "list all installcode, ZC only" },
#endif
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
  { "set", " [type] [code]", "        ", ic_set,         help_set,    "set installcode type: [48|64|96|128|passcode|hashed] code [IC:...:IC], ZR ZED only" },
#endif
  /* Add new commands above here */
  { NULL, NULL,                    NULL, NULL,           NULL,        NULL }
};


#if defined(ZB_COORDINATOR_ROLE)
/* Static command installcode
 * command policy
 *
 * installcode policy [check]
 */
static zb_ret_t ic_policy(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_policy;
  cli_tools_strval able_table[] = {
    { "enable",  ZB_TRUE  },
    { "disable", ZB_FALSE }
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [check] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, able_table, &new_policy);


  if(!config_is_coordinator())
   return RET_UNAUTHORIZED;

  if(config_get_state() >= STATE_RUNNING)
    return RET_UNAUTHORIZED;

  zb_set_installcode_policy(new_policy);
  
  return RET_OK;
}


/* Static stack callback function
 * response for zb_secur_ic_add
 */
static void installcode_add_cb(zb_ret_t status)
{
  menu_printf("installcode_add_cb(): %s", wcs_get_error_str(status));

  menu_cb_occured();
}
/* Static command installcode
 * command add
 *
 * installcode add [type] [code] [ieee]
 */
static zb_ret_t ic_add(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t  new_ic_type;
  zb_uint_t  data_len;
  zb_uint8_t  new_ic_len;
  zb_uint8_t new_ic_val[16+2];
  zb_ieee_addr_t new_ieee_addr;
  cli_tools_strval type_table[] = {
    { "48",       ZB_IC_TYPE_48 },
    { "64",       ZB_IC_TYPE_64 },
    { "96",       ZB_IC_TYPE_96 },
    { "128",      ZB_IC_TYPE_128 },
    { "passcode", ZB_IC_TYPE_PASSCODE },
    { "hashed",   ZB_IC_TYPE_128_HASHED },
  };

  if(argc != 3)
    return RET_INVALID_PARAMETER;

  /* get [type] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, type_table, &new_ic_type);
  switch(new_ic_type) {
    case ZB_IC_TYPE_48:         new_ic_len =  6+2; break;
    case ZB_IC_TYPE_64:         new_ic_len =  8+2; break;
    case ZB_IC_TYPE_96:         new_ic_len = 12+2; break;
    case ZB_IC_TYPE_128:        new_ic_len = 16+2; break;
    case ZB_IC_TYPE_PASSCODE:   new_ic_len =  4+0; break;
    case ZB_IC_TYPE_128_HASHED: new_ic_len = 16+0; break;
    default:       return RET_INVALID_PARAMETER_1; break;
  }

  /* get [code len] */
  TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 1, &data_len);
  if(data_len != new_ic_len)
    return RET_INVALID_PARAMETER_2;
  /* get [code data] */
  TOOLS_GET_ARG_HEX_ARRAY_DATA(ret, argv, 1, data_len, new_ic_val);

  /* get [ieee] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 2, new_ieee_addr);

  /* check that we are coordinator */
  if(!config_is_coordinator())
   return RET_UNAUTHORIZED;

  /* Add install code for joiners at runtime */
  if(config_get_state() < STATE_RUNNING)
    return RET_UNAUTHORIZED;

  /* Configure install code for device ieee_addr */
  zb_secur_ic_add(new_ieee_addr, new_ic_type, new_ic_val, installcode_add_cb);

  return RET_OK;
}
static zb_ret_t help_add(void)
{
  menu_printf("type 48:       install code  6 bytes + 2 CRC");
  menu_printf("type 64:       install code  8 bytes + 2 CRC");
  menu_printf("type 96:       install code 12 bytes + 2 CRC");
  menu_printf("type 128:      install code 16 bytes + 2 CRC");
  menu_printf("type passcode: install code  4 bytes + 0 CRC");
  menu_printf("type hashed:   install code 16 bytes + 0 CRC");
  return RET_OK;
}


/* Static stack callback function
 * response for zb_secur_ic_remove_all
 */
static void ic_remove_all_cb(zb_uint8_t param)
{
  zb_secur_ic_remove_all_resp_t *resp = ZB_BUF_GET_PARAM(param, zb_secur_ic_remove_all_resp_t);

  if(resp->status != RET_OK)
  {
    menu_printf("installcode_remove_all_cb() Error: %s", wcs_get_error_str(resp->status));
  }
  else
  {
    menu_printf("installcode_remove_all_cb() %s", wcs_get_error_str(resp->status));
  }

  zb_buf_free(param);
  menu_cb_occured();
}
/* Static stack callback function
 * response for zb_secur_ic_remove
 */
static void ic_remove_cb(zb_uint8_t param)
{
  zb_secur_ic_remove_resp_t *resp = ZB_BUF_GET_PARAM(param, zb_secur_ic_remove_resp_t);

  if(resp->status != RET_OK)
  {
    menu_printf("installcode_remove_cb() Error: %s", wcs_get_error_str(resp->status));
  }
  else
  {
    menu_printf("installcode_remove_cb() %s", wcs_get_error_str(resp->status));
  }

  zb_buf_free(param);
  menu_cb_occured();
}
/* Static command installcode
 * command remove
 *
 * installcode remove [ieee]
 */
static zb_ret_t ic_remove(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_ieee_addr_t new_ieee_addr;
  zb_uint8_t all = ZB_FALSE;
  cli_tools_strval all_table[] = {
    { "all", ZB_TRUE },
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [ieee], can be 'all' in case all devices are targeted */
  if((ret = tools_arg_get_ieee(argv, 0, new_ieee_addr, ARG_HEX)) != RET_OK &&
     (ret = tools_arg_get_strval(argv, 0, all_table, 1, &all) != RET_OK))
  {
    return ret;
  }

  /* check we are coordinator */
  if(!config_is_coordinator())
   return RET_UNAUTHORIZED;

  /* Remove install code for joiners at runtime */
  if(config_get_state() < STATE_RUNNING)
    return RET_UNAUTHORIZED;

  if(all)
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_secur_ic_remove_all_req_t *req;

    buffer = zb_buf_get_out();

    req = ZB_BUF_GET_PARAM(buffer, zb_secur_ic_remove_all_req_t);
    req->response_cb = ic_remove_all_cb;
    zb_secur_ic_remove_all_req(buffer);
  }
  else
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_secur_ic_remove_req_t *req;

    buffer = zb_buf_get_out();

    req = ZB_BUF_GET_PARAM(buffer, zb_secur_ic_remove_req_t);
    req->response_cb = ic_remove_cb;
    ZB_IEEE_ADDR_COPY(req->device_address, new_ieee_addr);
    zb_secur_ic_remove_req(buffer);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_secur_ic_add
 */
static void ic_list_cb(zb_uint8_t param)
{
  zb_secur_ic_get_list_resp_t *resp = ZB_BUF_GET_PARAM(param, zb_secur_ic_get_list_resp_t);

  if(resp->status != RET_OK)
  {
    menu_printf("installcode_list_cb() Error: %s", wcs_get_error_str(resp->status));
  }
  else
  {
    zb_secur_ic_entry_t *ic_table = (zb_secur_ic_entry_t*)zb_buf_begin(param);

    menu_printf("installcode_list_cb() Ok: %hhu entries starting at %hhu, total %hhu", resp->ic_table_entries, resp->start_index, resp->ic_table_list_count);
    for(zb_uint8_t i=0; i<resp->ic_table_entries; i++)
    {
      zb_secur_ic_entry_t *ic = &ic_table[i];
      char type_val_str[128] = "???";

      switch(ic->options & 0x3)
      {
      case 0: snprintf(type_val_str, sizeof(type_val_str), " 48: "TRACE_FORMAT_IC_48,  TRACE_ARG_IC_48(ic->installcode));  break;
      case 1: snprintf(type_val_str, sizeof(type_val_str), " 64: "TRACE_FORMAT_IC_64,  TRACE_ARG_IC_64(ic->installcode));  break;
      case 2: snprintf(type_val_str, sizeof(type_val_str), " 96: "TRACE_FORMAT_IC_96,  TRACE_ARG_IC_96(ic->installcode));  break;
      case 3: snprintf(type_val_str, sizeof(type_val_str), "128: "TRACE_FORMAT_IC_128, TRACE_ARG_IC_128(ic->installcode)); break;
      }

      menu_printf("\tIC[%hhu] ieee_addr: "TRACE_FORMAT_64" type %s", resp->start_index+i, TRACE_ARG_64(ic->device_address), type_val_str);
    }

    if(resp->start_index + resp->ic_table_entries < resp->ic_table_list_count)
    {
      zb_bufid_t buffer = ZB_BUF_INVALID;
      zb_secur_ic_get_list_req_t *req;

      buffer = zb_buf_get_out();

      req = ZB_BUF_GET_PARAM(buffer, zb_secur_ic_get_list_req_t);
      req->response_cb = ic_list_cb;
      req->start_index = resp->start_index + resp->ic_table_entries;

      zb_secur_ic_get_list_req(buffer);
    }
  }

  zb_buf_free(param);
  menu_cb_occured();
}
/* Static command installcode
 * command list
 *
 * installcode list
 */
static zb_ret_t ic_list(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_secur_ic_get_list_req_t *req;

    buffer = zb_buf_get_out();
    req = ZB_BUF_GET_PARAM(buffer, zb_secur_ic_get_list_req_t);
    req->response_cb = ic_list_cb;
    req->start_index = 0;

    zb_secur_ic_get_list_req(buffer);
  }

  return RET_OK;
}
#endif


#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
/* Static command installcode
 * command set
 *
 * installcode set [type] [code]
 */
static zb_ret_t ic_set(int argc, char *argv[])
{
  zb_uint8_t  new_ic_type;
  zb_uint_t   data_len;
  zb_uint8_t  new_ic_len;
  zb_uint8_t new_ic_val[16+2];
  zb_ret_t ret;
  cli_tools_strval type_table[] = {
    { "48",       ZB_IC_TYPE_48 },
    { "64",       ZB_IC_TYPE_64 },
    { "96",       ZB_IC_TYPE_96 },
    { "128",      ZB_IC_TYPE_128 },
    { "passcode", ZB_IC_TYPE_PASSCODE },
    { "hashed",   ZB_IC_TYPE_128_HASHED },
  };

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [type] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, type_table, &new_ic_type);
  switch(new_ic_type) {
    case ZB_IC_TYPE_48:         new_ic_len =  6+2; break;
    case ZB_IC_TYPE_64:         new_ic_len =  8+2; break;
    case ZB_IC_TYPE_96:         new_ic_len = 12+2; break;
    case ZB_IC_TYPE_128:        new_ic_len = 16+2; break;
    case ZB_IC_TYPE_PASSCODE:   new_ic_len =  4+0; break;
    case ZB_IC_TYPE_128_HASHED: new_ic_len = 16+0; break;
    default:       return RET_INVALID_PARAMETER_1; break;
  }

  /* get [code len] */
  TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 1, &data_len);
  if(data_len != new_ic_len)
    return RET_INVALID_PARAMETER_2;
  /* get [code data] */
  TOOLS_GET_ARG_HEX_ARRAY_DATA(ret, argv, 1, data_len, new_ic_val);

  /* Set my install code at configtime */
  if(config_get_state() >= STATE_RUNNING)
    return RET_UNAUTHORIZED;

  if(ZB_TRUE
#if defined(ZB_ROUTER_ROLE)
    && !config_is_router()
#endif
#if defined(ZB_ED_ROLE)
    && !config_is_end_device()
#endif
  )
    return RET_UNAUTHORIZED;

  /* Configure my install code */
  ret = zb_secur_ic_set(new_ic_type, new_ic_val);

  return ret;
}
static zb_ret_t help_set(void)
{
  menu_printf("type 48:       install code  6 bytes + 2 CRC");
  menu_printf("type 64:       install code  8 bytes + 2 CRC");
  menu_printf("type 96:       install code 12 bytes + 2 CRC");
  menu_printf("type 128:      install code 16 bytes + 2 CRC");
  menu_printf("type passcode: install code  4 bytes + 0 CRC");
  menu_printf("type hashed:   install code 16 bytes + 0 CRC");
  return RET_OK;
}
#endif


