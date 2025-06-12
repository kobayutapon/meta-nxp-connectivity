/*
 * Copyright 2024-2025 NXP
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


static void menu_print_cluster_attributes(zb_zcl_device_callback_param_t *device_cb_params, zb_af_endpoint_desc_t *this_ep)
{
  menu_printf("Callback endpoint %u(%d clusters), attr %d, %s",
    device_cb_params->endpoint,
    (this_ep)?(this_ep->cluster_count):(0),
    device_cb_params->attr_type,
    get_device_cb_id_str(device_cb_params->device_cb_id));

}

static zb_bool_t is_cluster_created(zb_af_endpoint_desc_t *this_ep, zb_uint16_t cluster_id, zb_uint8_t role)
{
  if(!this_ep)
    return ZB_FALSE;

  for(int i=0; i<this_ep->cluster_count; i++)
    if(this_ep->cluster_desc_list[i].cluster_id == cluster_id &&
       this_ep->cluster_desc_list[i].role_mask & role)
      return ZB_TRUE;

  return ZB_FALSE;
}

void cluster_attributes_cb(zb_uint8_t param)
{
  zb_zcl_device_callback_param_t *device_cb_params = ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);
  zb_af_endpoint_desc_t *this_ep = endpoint_get_by_id(device_cb_params->endpoint);

  device_cb_params->status = RET_NOT_IMPLEMENTED; //Will return ZCL status ZB_ZCL_STATUS_UNSUP_CMD
  switch(device_cb_params->device_cb_id)
  {
  case ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID:
    // done inside the handler... menu_print_cluster_attributes(device_cb_params, this_ep);
    /* Make sure we have created the ota cluster on client side */
    if(is_cluster_created(this_ep, ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_CLIENT_ROLE))
    {
      device_cb_params->status = cluster_ota_clt_device_value_cb(device_cb_params);
    }
    else
    {
      device_cb_params->status = RET_INVALID_PARAMETER;
    }
    break;

  case ZB_ZCL_OTA_UPGRADE_SRV_QUERY_IMAGE_CB_ID:
    menu_print_cluster_attributes(device_cb_params, this_ep);
    /* Make sure we have created the ota cluster on client side */
    if(is_cluster_created(this_ep, ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_SERVER_ROLE))
    {
      /* Well we have already inserted files to the stack, let it manage it */
      device_cb_params->status = RET_NOT_FOUND;
    }
    else
    {
      device_cb_params->status = RET_INVALID_PARAMETER;
    }
    break;

  case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
    /* This case has been added to avoid going into the default case which returns an error code, when we have a set attribute command. */
    device_cb_params->status = RET_OK;
    break;

  case ZB_ZCL_SCENES_ADD_SCENE_CB_ID:
  case ZB_ZCL_SCENES_STORE_SCENE_CB_ID:
  case ZB_ZCL_SCENES_VIEW_SCENE_CB_ID:
  case ZB_ZCL_SCENES_REMOVE_SCENE_CB_ID:
  case ZB_ZCL_SCENES_REMOVE_ALL_SCENES_CB_ID:
  case ZB_ZCL_SCENES_RECALL_SCENE_CB_ID:
  case ZB_ZCL_SCENES_GET_SCENE_MEMBERSHIP_CB_ID:
#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
  case ZB_ZCL_SCENES_ENHANCED_ADD_SCENE_CB_ID:
  case ZB_ZCL_SCENES_ENHANCED_VIEW_SCENE_CB_ID:
  case ZB_ZCL_SCENES_COPY_SCENE_CB_ID:
#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */
    if(is_cluster_created(this_ep, ZB_ZCL_CLUSTER_ID_SCENES, ZB_ZCL_CLUSTER_SERVER_ROLE))
    {
      device_cb_params->status = cluster_scenes_srv_device_value_cb(device_cb_params, param);
    }
    else
    {
      zb_uint8_t *add_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      *add_scene_status = ZB_ZCL_STATUS_UNSUP_CLUST_CMD;
      device_cb_params->status = RET_NOT_IMPLEMENTED;
    }
    break;
  case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_ALL_GROUPS_CB_ID:
  case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_CB_ID:
    {
      /* Internal: indicate to application that it is needed to remove all scenes on all
       * endpoints for XXX groups (full reset of scenes table if XXX is all).
       */
      for(int i=0; i<256; i++)
      {
        this_ep = endpoint_get_by_id(i);
        if(!this_ep)
          continue;

        if(is_cluster_created(this_ep, ZB_ZCL_CLUSTER_ID_SCENES, ZB_ZCL_CLUSTER_SERVER_ROLE))
        {
          device_cb_params->endpoint = i;
          cluster_scenes_srv_device_value_cb(device_cb_params, param);
        }
      }
      device_cb_params->status = RET_OK;
    }
    break;

  default:
    menu_print_cluster_attributes(device_cb_params, this_ep);
    break;
  }

  menu_cb_occured();
}


int wcs_snprintf_attr_raw(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value)
{
  int len = 0;
  zb_uint8_t attr_len = zb_zcl_get_attribute_size(type, value);

  if(!str || !attr_len)
    return len;

  for(zb_uint8_t i=0; i<attr_len; i++)
    len += wcs_snprintf(str+len, size-len, "%02x:", value[i]);

  /* Remove last ":" */
  str[len-1] = '\0';

  return len-1;
}


typedef union {
  zb_bool_t   boolean;
  zb_uint8_t  u8;
  zb_uint16_t u16;
  zb_uint32_t u32;
  zb_uint64_t u64;
  zb_int8_t   s8;
  zb_int16_t  s16;
  zb_int32_t  s32;
  zb_int64_t  s64;
} cast_t;
#ifdef ZB_LITTLE_ENDIAN

  #define ENDIAN_24_TO_32(val8,val32) val32 = 0;                                                                            \
    val32 =              val8[3]; val32 = (val32<<8) + val8[2]; val32 = (val32<<8) + val8[1]; val32 = (val32<<8) + val8[0];

  #define ENDIAN_40_TO_64(val8,val64) val64 = 0;                                                                            \
                                                                                              val64 = (val64<<8) + val8[4]; \
    val64 = (val64<<8) + val8[3]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[0];

  #define ENDIAN_48_TO_64(val8,val64) val64 = 0;                                                                            \
                                                                val64 = (val64<<8) + val8[5]; val64 = (val64<<8) + val8[4]; \
    val64 = (val64<<8) + val8[3]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[0];

  #define ENDIAN_56_TO_64(val8,val64) val64 = 0;                                                                            \
                                  val64 = (val64<<8) + val8[6]; val64 = (val64<<8) + val8[5]; val64 = (val64<<8) + val8[4]; \
    val64 = (val64<<8) + val8[3]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[0];

#else

  #define ENDIAN_24_TO_32(val8,val32) val32 = 0;                                                                            \
    val32 = (val32<<8) + val8[0]; val32 = (val32<<8) + val8[1]; val32 = (val32<<8) + val8[2];

  #define ENDIAN_40_TO_64(val8,val64) val64 = 0;                                                                            \
    val64 = (val64<<8) + val8[0]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[3]; \
    val64 = (val64<<8) + val8[4];

  #define ENDIAN_48_TO_64(val8,val64) val64 = 0;                                                                            \
    val64 = (val64<<8) + val8[0]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[3]; \
    val64 = (val64<<8) + val8[4]; val64 = (val64<<8) + val8[5];

  #define ENDIAN_56_TO_64(val8,val64) val64 = 0;                                                                            \
    val64 = (val64<<8) + val8[0]; val64 = (val64<<8) + val8[1]; val64 = (val64<<8) + val8[2]; val64 = (val64<<8) + val8[3]; \
    val64 = (val64<<8) + val8[4]; val64 = (val64<<8) + val8[5]; val64 = (val64<<8) + val8[6];

#endif

int wcs_snprintf_attr_val(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value)
{
  int len = 0;
  zb_uint8_t attr_len = zb_zcl_get_attribute_size(type, value);
  cast_t *cast_val = (cast_t *)value;
  cast_t data_val;

  if(!str)
    return len;

  switch(type) {
  case ZB_ZCL_ATTR_TYPE_NULL:              /*!<  0x00U: Null data type */
    len += wcs_snprintf(str+len, size-len, "NULL ");
    break;
  case ZB_ZCL_ATTR_TYPE_BOOL:              /*!<  0x10U: Boolean data type */
    len += wcs_snprintf(str+len, size-len, "%s ", (cast_val->boolean)?("true"):("false"));
    break;
  case ZB_ZCL_ATTR_TYPE_8BIT:              /*!<  0x08U: 8-bit value data type */
  case ZB_ZCL_ATTR_TYPE_8BITMAP:           /*!<  0x18U: 8-bit bitmap data type */
    len += wcs_snprintf(str+len, size-len, "0x%02x ", cast_val->u8);
    break;
  case ZB_ZCL_ATTR_TYPE_U8:                /*!<  0x20U: Unsigned 8-bit value data type */
  case ZB_ZCL_ATTR_TYPE_8BIT_ENUM:         /*!<  0x30U: 8-bit enumeration (U8 discrete) data type */
    len += wcs_snprintf(str+len, size-len, "%hhu ", cast_val->u8);
    break;
  case ZB_ZCL_ATTR_TYPE_S8:                /*!<  0x28U: Signed 8-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%hhd ", cast_val->s8);
    break;
  case ZB_ZCL_ATTR_TYPE_16BIT:             /*!<  0x09U: 16-bit value data type */
  case ZB_ZCL_ATTR_TYPE_16BITMAP:          /*!<  0x19U: 16-bit bitmap data type */
    len += wcs_snprintf(str+len, size-len, "0x%04x ", cast_val->u16);
    break;
  case ZB_ZCL_ATTR_TYPE_U16:               /*!<  0x21U: Unsigned 16-bit value data type */
  case ZB_ZCL_ATTR_TYPE_16BIT_ENUM:        /*!<  0x31U: 16-bit enumeration (U16 discrete) data type */
    len += wcs_snprintf(str+len, size-len, "%hu ", cast_val->u16);
    break;
  case ZB_ZCL_ATTR_TYPE_S16:               /*!<  0x29U: Signed 16-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%hd ", cast_val->s16);
    break;
  case ZB_ZCL_ATTR_TYPE_24BIT:             /*!<  0x0aU: 24-bit value data type */
  case ZB_ZCL_ATTR_TYPE_24BITMAP:          /*!<  0x1aU: 24-bit bitmap data type */
    ENDIAN_24_TO_32(value, data_val.u32);
    len += wcs_snprintf(str+len, size-len, "0x%06x ", data_val.u32);
    break;
  case ZB_ZCL_ATTR_TYPE_U24:               /*!<  0x22U: Unsigned 24-bit value data type */
    ENDIAN_24_TO_32(value, data_val.u32);
    len += wcs_snprintf(str+len, size-len, "%hu ", data_val.u32);
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_S24:               /*!<  0x2aU: Signed 24-bit value data type */
    break;
  case ZB_ZCL_ATTR_TYPE_32BIT:             /*!<  0x0bU: 32-bit value data type */
  case ZB_ZCL_ATTR_TYPE_32BITMAP:          /*!<  0x1bU: 32-bit bitmap data type */
    len += wcs_snprintf(str+len, size-len, "0x%08x ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_U32:               /*!<  0x23U: Unsigned 32-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%u ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_S32:               /*!<  0x2bU: Signed 32-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%d ", cast_val->s32);
    break;
  case ZB_ZCL_ATTR_TYPE_40BIT:             /*!<  0x0cU: 40-bit value data type */
  case ZB_ZCL_ATTR_TYPE_40BITMAP:          /*!<  0x1cU: 40-bit bitmap data type */
    ENDIAN_40_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "0x%10lx ", data_val.u64);
    break;
  case ZB_ZCL_ATTR_TYPE_U40:               /*!<  0x24U: Unsigned 40-bit value data type */
    ENDIAN_40_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "%lu ", data_val.u64);
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_S40:               /*!<  0x2cU: Signed 40-bit value data type */
    break;
  case ZB_ZCL_ATTR_TYPE_48BIT:             /*!<  0x0dU: 48-bit value data type */
  case ZB_ZCL_ATTR_TYPE_48BITMAP:          /*!<  0x1dU: 48-bit bitmap data type */
    ENDIAN_48_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "0x%12lx ", data_val.u64);
    break;
  case ZB_ZCL_ATTR_TYPE_U48:               /*!<  0x25U: Unsigned 48-bit value data type */
    ENDIAN_48_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "%lu ", data_val.u64);
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_S48:               /*!<  0x2dU: Signed 48-bit value data type */
    break;
  case ZB_ZCL_ATTR_TYPE_56BIT:             /*!<  0x0eU: 56-bit value data type */
  case ZB_ZCL_ATTR_TYPE_56BITMAP:          /*!<  0x1eU: 56-bit bitmap data type */
    ENDIAN_56_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "0x%14lx ", data_val.u64);
    break;
  case ZB_ZCL_ATTR_TYPE_U56:               /*!<  0x26U: Unsigned 56-bit value data type */
    ENDIAN_56_TO_64(value, data_val.u64);
    len += wcs_snprintf(str+len, size-len, "%lu ", data_val.u64);
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_S56:               /*!<  0x2eU: Signed 56-bit value data type */
    break;
  case ZB_ZCL_ATTR_TYPE_64BIT:             /*!<  0x0fU: 64-bit value data type */
  case ZB_ZCL_ATTR_TYPE_64BITMAP:          /*!<  0x1fU: 64-bit bitmap data type */
    len += wcs_snprintf(str+len, size-len, "0x%16lx ", cast_val->u64);
    break;
  case ZB_ZCL_ATTR_TYPE_U64:               /*!<  0x27U: Unsigned 64-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%lu ", cast_val->u64);
    break;
  case ZB_ZCL_ATTR_TYPE_S64:               /*!<  0x2bU: Signed 32-bit value data type */
    len += wcs_snprintf(str+len, size-len, "%ld ", cast_val->s64);
    break;

// TODO  case ZB_ZCL_ATTR_TYPE_SEMI:              /*!<  0x38U: 2 byte floating point */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_SINGLE:            /*!<  0x39U: 4 byte floating point */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_DOUBLE:            /*!<  0x3aU: 8 byte floating point */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_OCTET_STRING:      /*!<  0x41U: Octet string data type */
    break;
  case ZB_ZCL_ATTR_TYPE_CHAR_STRING:       /*!<  0x42U: Character string (array) data type */
    for(int i=1; i<value[0]; i++)
      len += wcs_snprintf(str+len, size-len, "%c", value[i]);
    len += wcs_snprintf(str+len, size-len, " ");
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING: /*!<  0x43U: Long octet string */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_LONG_CHAR_STRING:  /*!<  0x44U: Long character string */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_ARRAY:             /*!<  0x48U: Array data type 2 + sum of content len */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_STRUCTURE:         /*!<  0x4cU: Structure data type 2 + sum of content len */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_SET:               /*!<  0x50U: Collection:set, size = sum of len of content */
    break;
// TODO  case ZB_ZCL_ATTR_TYPE_BAG:               /*!<  0x51U: Collection:bag, size = sum of len of content */
    break;
  case ZB_ZCL_ATTR_TYPE_TIME_OF_DAY:       /*!<  0xe0U: Time of day, 4 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%08x ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_DATE:              /*!<  0xe1U: Date, 4 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%08x ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_UTC_TIME:          /*!<  0xe2U: UTC Time, 4 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%08x ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_CLUSTER_ID:        /*!<  0xe8U: Cluster ID, 2 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%04x ", cast_val->u16);
    break;
  case ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID:      /*!<  0xe9U: Attribute ID, 2 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%04x ", cast_val->u16);
    break;
  case ZB_ZCL_ATTR_TYPE_BACNET_OID:        /*!<  0xeaU: BACnet OID, 4 bytes */
    len += wcs_snprintf(str+len, size-len, "0x%08x ", cast_val->u32);
    break;
  case ZB_ZCL_ATTR_TYPE_IEEE_ADDR:
    len += wcs_snprintf(str+len, size-len, TRACE_FORMAT_64, TRACE_ARG_64(value));
    break;
  case ZB_ZCL_ATTR_TYPE_128_BIT_KEY:
    len += wcs_snprintf(str+len, size-len, TRACE_FORMAT_128, TRACE_ARG_128(value));
    break;

  default:
    len += wcs_snprintf(str+len, size-len, "Type:0x%02x,Len:%u", type, attr_len);
    break;
  }
  return len;
}

