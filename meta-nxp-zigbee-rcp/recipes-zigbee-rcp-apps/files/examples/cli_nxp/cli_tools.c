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

#define ZB_TRACE_FILE_ID 33620
#include "zboss_api.h"
#include "cli_tools.h"
#include "cli_endpoint.h"
#include <ctype.h>

#define RET_INVALID_PARAM(argnb)       ERROR_CODE(ERROR_CATEGORY_GENERIC, ERROR_GET_CODE(RET_INVALID_PARAMETER_1)+argnb)



#define read_long_signed_int(argv, argnb, val, min, max, format)                         \
  if(argv[argnb] == NULL)                                                                \
    return RET_EOF;                                                                      \
  if ((format & ARG_HEX) && !strncasecmp(argv[argnb], "0x", 2))                          \
  {                                                                                      \
    if(1 != sscanf(argv[argnb]+2, "%lx", val))                                           \
      return RET_INVALID_PARAM(argnb);                                                   \
  }                                                                                      \
  else  if(format & ARG_DEC)                                                             \
  {                                                                                      \
    if(1 != sscanf(argv[argnb], "%ld", val))                                             \
      return RET_INVALID_PARAM(argnb);                                                   \
  }                                                                                      \
  else                                                                                   \
    return RET_INVALID_FORMAT;                                                           \
  if(*val < min || *val > max)                                                           \
  {                                                                                      \
      menu_printf("Parsed argument %s is out of range [%d, %d]", argv[argnb], min, max); \
      return RET_INVALID_PARAM(argnb);                                                   \
  }

#define read_long_unsigned_int(argv, argnb, val, max, format)                            \
  if(argv[argnb] == NULL)                                                                \
    return RET_EOF;                                                                      \
  if ((format & ARG_HEX) && !strncasecmp(argv[argnb], "0x", 2))                          \
  {                                                                                      \
    if(1 != sscanf(argv[argnb]+2, "%lx", val))                                           \
      return RET_INVALID_PARAM(argnb);                                                   \
  }                                                                                      \
  else if(format & ARG_DEC)                                                              \
  {                                                                                      \
    if(1 != sscanf(argv[argnb], "%lu", val))                                             \
      return RET_INVALID_PARAM(argnb);                                                   \
  }                                                                                      \
  else                                                                                   \
    return RET_INVALID_FORMAT;                                                           \
  if(*val > max)                                                                         \
    {                                                                                    \
        menu_printf("Parsed argument %s is out of range [%u, %u]", argv[argnb], 0, max); \
        return RET_INVALID_PARAM(argnb);                                                 \
    }


zb_ret_t tools_arg_get_int(char *argv[], int argnb, zb_int_t *val, zb_arg_format_t format)
{
  long int lsint_val;

  read_long_signed_int(argv, argnb, &lsint_val, ZB_INT_MIN, ZB_INT_MAX, format);

  *val = (zb_int_t)lsint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d int 0x%x:%d ", argnb, *val, *val);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_int8(char *argv[], int argnb, zb_int8_t *val, zb_arg_format_t format)
{
  long int lsint_val;

  read_long_signed_int(argv, argnb, &lsint_val, ZB_INT8_MIN, ZB_INT8_MAX, format);

  *val = (zb_int8_t)lsint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d int8 0x%02x:%d ", argnb, *val, *val);
#endif

  return RET_OK;
}


zb_ret_t tools_arg_get_int16(char *argv[], int argnb, zb_int16_t *val, zb_arg_format_t format)
{
  long int lsint_val;

  read_long_signed_int(argv, argnb, &lsint_val, ZB_INT16_MIN, ZB_INT16_MAX, format);

  *val = (zb_int16_t)lsint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d int16 0x%04x:%d ", argnb, *val, *val);
#endif

  return RET_OK;
}


zb_ret_t tools_arg_get_int32(char *argv[], int argnb, zb_int32_t *val, zb_arg_format_t format)
{
  long int lsint_val;

  read_long_signed_int(argv, argnb, &lsint_val, ZB_INT32_MIN, ZB_INT32_MAX, format);

  *val = (zb_int32_t)lsint_val;

#ifdef CLI_DEBUG_ARG
    WCS_TRACE_DEBUG("get arg %d int32 0x%08x:%d ", argnb, *val, *val);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_uint(char *argv[], int argnb, zb_uint_t *val, zb_arg_format_t format)
{
  long unsigned int luint_val;

  read_long_unsigned_int(argv, argnb, &luint_val, ZB_UINT_MAX, format);

  *val = (zb_uint_t)luint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d int 0x%x:%d ", argnb, *val, *val);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_uint8(char *argv[], int argnb, zb_uint8_t *val, zb_arg_format_t format)
{
  long unsigned int luint_val;

  read_long_unsigned_int(argv, argnb, &luint_val, ZB_UINT8_MAX, format);

  *val = (zb_uint8_t)luint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d uint8 0x%02x:%u ", argnb, *val, *val);
#endif

  return RET_OK;
}


zb_ret_t tools_arg_get_uint16(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format)
{
  long unsigned int luint_val;

  read_long_unsigned_int(argv, argnb, &luint_val, ZB_UINT16_MAX, format);

  *val = (zb_uint16_t)luint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d uint16 0x%04x:%u ", argnb, *val, *val);
#endif

  return RET_OK;
}


zb_ret_t tools_arg_get_uint32(char *argv[], int argnb, zb_uint32_t *val, zb_arg_format_t format)
{
  long unsigned int luint_val;

  read_long_unsigned_int(argv, argnb, &luint_val, ZB_UINT32_MAX, format);

  *val = (zb_uint32_t)luint_val;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d uint32 0x%08x:%u ", argnb, *val, *val);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_hex_array_len(char *argv[], int argnb, zb_uint_t *val)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  /* data string: xx:xx...xx:xx so string len is 2*data_len + (data_len-1) */
  if((strlen(argv[argnb])+1)%3 != 0 )
    return RET_INVALID_PARAM(argnb);

  *val = (strlen(argv[argnb])+1)/3;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d hex array len %u...", argnb, *val);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_hex_array_data(char *argv[], int argnb, zb_uint_t len, zb_uint8_t *val)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  for(zb_uint_t i=0; i<len; i++)
  {
    /* Force to delimit the 2 characters to be hexa data: sscanf( "%02x" ) would return ok if chars are 7Z => value 07 */
    char hexData[6] = ":xx:0";
    int dummy;
    hexData[1] =  argv[argnb][3*i+0];
    hexData[2] =  argv[argnb][3*i+1];
    if(2 != sscanf(hexData, ":%hhx:%d", &val[i], &dummy))
      return RET_INVALID_PARAM(argnb);

#ifdef CLI_DEBUG_ARG
    WCS_TRACE_DEBUG("get arg %d hex data[%u] 0x%02x", argnb, i, val[i]);
#endif
  }

  return RET_OK;
}

zb_ret_t tools_arg_get_strval(char *argv[], int argnb, cli_tools_strval *strval_table, zb_uint_t strval_size, zb_uint8_t *val)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  for(zb_uint_t i=0; i<strval_size; i++)
  {
    if(!strcasecmp(argv[argnb], strval_table[i].string))
    {
#ifdef CLI_DEBUG_ARG
      WCS_TRACE_DEBUG("get arg %d strval %s: 0x%x", argnb, argv[argnb], strval_table[i].value);
#endif

      *val = strval_table[i].value;

      return RET_OK;
    }
  }

  return RET_INVALID_PARAM(argnb);
}

zb_ret_t tools_arg_get_ieee(char *argv[], int argnb, zb_ieee_addr_t val, zb_arg_format_t format)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  if(format != ARG_HEX)
    return RET_INVALID_FORMAT;

  for(int i = 0; argv[argnb][i]; i++)
    argv[argnb][i] = tolower(argv[argnb][i]);
  if(8 != sscanf(argv[argnb], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
  &val[7], &val[6], &val[5], &val[4], &val[3], &val[2], &val[1], &val[0]))
  {
    return RET_INVALID_PARAM(argnb);
  }

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d ieee %hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    argnb,
    val[0], val[1], val[2],  val[3],  val[4],  val[5],  val[6],  val[7]);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_nwk_key(char *argv[], int argnb, zb_uint8_t val[ZB_CCM_KEY_SIZE], zb_arg_format_t format)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  if(format != ARG_HEX)
    return RET_INVALID_FORMAT;

  for(int i = 0; argv[argnb][i]; i++)
    argv[argnb][i] = tolower(argv[argnb][i]);
  if(ZB_CCM_KEY_SIZE != sscanf(argv[argnb], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
  &val[0], &val[1], &val[2], &val[3], &val[4], &val[5], &val[6], &val[7], &val[8], &val[9], &val[10], &val[11], &val[12], &val[13], &val[14], &val[15]))
  {
    return RET_INVALID_PARAM(argnb);
  }

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d nwk key %hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
    argnb,
    val[0], val[1], val[2],  val[3],  val[4],  val[5],  val[6],  val[7],
    val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
#endif

  return RET_OK;
}

zb_ret_t tools_arg_get_profile(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format)
{
  zb_ret_t ret;
  zb_uint16_t my_profile_id;

  ZVUNUSED(format);

  ret = tools_arg_get_uint16(argv, argnb, &my_profile_id, format);
  if(ret != RET_OK)
  {
    /* Not an hexa, look for initials */
    for(int i=0; table_profiles[i].name != NULL; i++)
      if(!strcasecmp(argv[argnb], table_profiles[i].initials))
      {
        my_profile_id = table_profiles[i].id;
        break;
      }

    if(my_profile_id == 0xFFFF)
      return RET_INVALID_PARAM(argnb);
  }

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d profile %04x", argnb, my_profile_id);
#endif

  *val = my_profile_id;
  return RET_OK;
}

zb_ret_t tools_arg_get_device_type(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format)
{
  zb_ret_t ret;
  zb_uint16_t my_device_id;

  ZVUNUSED(format);

  ret = tools_arg_get_uint16(argv, argnb, &my_device_id, format);
  if(ret != RET_OK)
  {
    /* Not an hexa, look for initials */
    for(int i=0; endpoint_devices[i].id != 0xFFFF; i++)
      if(!strcasecmp(argv[argnb], endpoint_devices[i].initials))
      {
        my_device_id = endpoint_devices[i].id;
        break;
      }

    if(my_device_id == 0xFFFF)
      return RET_INVALID_PARAM(argnb);
  }

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d device %04x", argnb, my_device_id);
#endif

  *val = my_device_id;
  return RET_OK;
}

zb_ret_t tools_arg_get_endpoint_with_cluster_role(char *argv[], int argnb, zb_af_endpoint_desc_t **val_ep, zb_uint16_t cluster_id, zb_uint8_t role_mask)
{
  zb_ret_t ret;
  zb_uint8_t my_ep_id;
  zb_af_endpoint_desc_t *this_ep;

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,  argv, argnb, &my_ep_id);

  this_ep = endpoint_get_by_id(my_ep_id);
  if(!this_ep)
    return RET_NOT_FOUND;

  if(role_mask)
  {
    zb_zcl_cluster_desc_t *this_cluster = NULL;

    for(int i=0; i<this_ep->cluster_count;i++)
      if(this_ep->cluster_desc_list[i].cluster_id == cluster_id &&
         this_ep->cluster_desc_list[i].role_mask & role_mask)
      {
        this_cluster = &this_ep->cluster_desc_list[i];
        break;
      }
    if(!this_cluster)
      return RET_NOT_FOUND;

#ifdef CLI_DEBUG_ARG
    WCS_TRACE_DEBUG("get arg %d endpoint 0x%x:%d having cluster 0x%04x role %x", argnb, *val_ep, *val_ep, cluster_id, role_mask);
  }
  else
  {
    WCS_TRACE_DEBUG("get arg %d endpoint 0x%x:%d ", argnb, *val_ep, *val_ep);
#endif
  }

  *val_ep = this_ep;
  return RET_OK;
}

zb_ret_t tools_arg_cluster(char *argv[], int argnb, zb_cluster_entry **val_cl, zb_bool_t check_implementation)
{
  zb_ret_t ret;
  zb_uint16_t my_cl_id;
  zb_cluster_entry *this_cl = NULL;

  ret = tools_arg_get_uint16(argv, argnb, &my_cl_id, ARG_HEX);
  if(ret == RET_OK)
  {
    /* Check the cluster is correct */
    for(int i=0; table_clusters[i].id != 0xFFFF; i++)
      if(table_clusters[i].id == my_cl_id)
      {
        this_cl = &table_clusters[i];
        break;
      }
  }
  else
  {
    /* Not an hexa, look for initials */
    for(int i=0; table_clusters[i].id != 0xFFFF; i++)
      if(!strcasecmp(argv[argnb], table_clusters[i].initials))
      {
        this_cl = &table_clusters[i];
        break;
      }
  }
  if(!this_cl)
  {
    menu_printf("invalid cluster \'%s\', possible values (hexa or initials):", argv[argnb]);
    help_clusters();
    return RET_INVALID_PARAM(argnb);
  }
  else if(check_implementation && !this_cl->definition)
    return RET_NOT_IMPLEMENTED;

#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get arg %d cluster %04x: %s: %s-%s", argnb, this_cl->id, this_cl->initials, this_cl->domain, this_cl->name);
#endif

  *val_cl = this_cl;
  return RET_OK;
}

zb_ret_t tools_arg_help(int argc, char *argv[], int argnb)
{
  if(argv[argnb] == NULL)
    return RET_EOF;

  if(argc >= argnb+1 && !strcmp(argv[argnb], "help"))
    return RET_OK;
  return RET_INVALID_PARAM(argnb);
}

