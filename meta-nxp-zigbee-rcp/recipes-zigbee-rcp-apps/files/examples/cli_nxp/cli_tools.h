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

#ifndef CLI_NXP_TOOLS_H
#define CLI_NXP_TOOLS_H 1

#include "zboss_api.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"


/* Define this if you need to debug your command arg parsing */
#undef CLI_DEBUG_ARG

typedef enum {
  ARG_DEC=0x01,
  ARG_HEX=0x02,
  ARG_ANY=0x03
} zb_arg_format_t;

typedef struct {
 char *string;
 zb_uint8_t value;
} cli_tools_strval;

#define TOOLS_GET_ARG(ret, type, argv, argnb, dest_ptr)                    \
  ret = tools_arg_get_##type(argv, argnb, dest_ptr, ARG_ANY);              \
  if(ret != RET_OK)                                                        \
    return ret

#define TOOLS_GET_ARG_DECIMAL(ret, type, argv, argnb, dest_ptr)            \
  ret = tools_arg_get_##type(argv, argnb, dest_ptr, ARG_DEC);              \
  if(ret != RET_OK)                                                        \
    return ret

#define TOOLS_GET_ARG_HEXA(ret, type, argv, argnb, dest_ptr)               \
  ret = tools_arg_get_##type(argv, argnb, dest_ptr, ARG_HEX);              \
  if(ret != RET_OK)                                                        \
    return ret


#define TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, argnb, dest_ptr)            \
  ret = tools_arg_get_hex_array_len(argv, argnb, dest_ptr);                \
  if(ret != RET_OK)                                                        \
    return ret

#define TOOLS_GET_ARG_HEX_ARRAY_DATA(ret, argv, argnb, len, dest_ptr)      \
  ret = tools_arg_get_hex_array_data(argv, argnb, len, dest_ptr);          \
  if(ret != RET_OK)                                                        \
    return ret

#define TOOLS_GET_ARG_STRVAL(ret, argv, argnb, table, dest_ptr)                              \
  ret = tools_arg_get_strval(argv, argnb, table, sizeof(table)/sizeof(table[0]), dest_ptr);  \
  if(ret != RET_OK)                                                                          \
    return ret

#define TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, argnb, dest_ptr, cl_id, cl_role)          \
  ret = tools_arg_get_endpoint_with_cluster_role(argv, argnb, dest_ptr, cl_id, cl_role); \
  if(ret != RET_OK)                                                                      \
    return ret

#define TOOLS_GET_ARG_CLUSTER(ret, argv, argnb, dest_ptr, impl)            \
  ret = tools_arg_cluster(argv, argnb, dest_ptr, impl);                    \
  if(ret != RET_OK)                                                        \
    return ret

zb_ret_t tools_arg_get_int(char *argv[], int argnb, zb_int_t *val, zb_arg_format_t format);

zb_ret_t tools_arg_get_int8(char *argv[], int argnb, zb_int8_t *val, zb_arg_format_t format);
zb_ret_t tools_arg_get_int16(char *argv[], int argnb, zb_int16_t *val, zb_arg_format_t format);
zb_ret_t tools_arg_get_int32(char *argv[], int argnb, zb_int32_t *val, zb_arg_format_t format);

zb_ret_t tools_arg_get_uint(char *argv[], int argnb, zb_uint_t *val, zb_arg_format_t format);

zb_ret_t tools_arg_get_uint8(char *argv[], int argnb, zb_uint8_t *val, zb_arg_format_t format);
zb_ret_t tools_arg_get_uint16(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format);
zb_ret_t tools_arg_get_uint32(char *argv[], int argnb, zb_uint32_t *val, zb_arg_format_t format);

zb_ret_t tools_arg_get_hex_array_len(char *argv[], int argnb, zb_uint_t *val);
zb_ret_t tools_arg_get_hex_array_data(char *argv[], int argnb, zb_uint_t len, zb_uint8_t *val);

zb_ret_t tools_arg_get_strval(char *argv[], int argnb, cli_tools_strval *strval_table, zb_uint_t strval_size, zb_uint8_t *val);

zb_ret_t tools_arg_get_ieee(char *argv[], int argnb, zb_ieee_addr_t val, zb_arg_format_t format);
zb_ret_t tools_arg_get_nwk_key(char *argv[], int argnb, zb_uint8_t val[ZB_CCM_KEY_SIZE], zb_arg_format_t format);

zb_ret_t tools_arg_get_profile(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format);
zb_ret_t tools_arg_get_device_type(char *argv[], int argnb, zb_uint16_t *val, zb_arg_format_t format);

zb_ret_t tools_arg_get_endpoint_with_cluster_role(char *argv[], int argnb, zb_af_endpoint_desc_t **val_ep, zb_uint16_t cluster_id, zb_uint8_t role_mask);

zb_ret_t tools_arg_cluster(char *argv[], int argnb, zb_cluster_entry **val_cl, zb_bool_t check_implementation);

zb_ret_t tools_arg_help(int argc, char *argv[], int argnb);

#endif /* CLI_NXP_TOOLS_H */

