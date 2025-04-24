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


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER CUSTOM NXP
 *
 * ----------------------------------------------------------------------------------- */

#define MAX_PAYLOAD_SIZE 73 /* Maximal payload size authorized in order to send packet in one frame */

static zb_uint8_t custom_nxp_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

static zb_uint16_t cluster_revision_nxp_attr_list = ZB_ZCL_BASIC_CLUSTER_REVISION_DEFAULT;

static zb_zcl_attr_t cluster_attr_fc02[] = {
  /* Mandatory attributes */
  { 0xfffd,      ZB_ZCL_ATTR_TYPE_U16,            ACC_READ_ONLY,  ZB_MANUFACTURER_CODE_NXP, (void*)&cluster_revision_nxp_attr_list  },
  { 0x0000,      ZB_ZCL_ATTR_TYPE_U8,             ACC_READ_ONLY,  ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.zcl_version   },
  { 0x0001,      ZB_ZCL_ATTR_TYPE_U8,             ACC_READ_ONLY,  ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.power_source  },
  { 0x0003,      ZB_ZCL_ATTR_TYPE_64BIT,          ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_64       },
  { 0x0004,      ZB_ZCL_ATTR_TYPE_BOOL,           ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_bool     },
  { 0x0005,      ZB_ZCL_ATTR_TYPE_32BITMAP,       ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_32_bm    },
  { 0x0006,      ZB_ZCL_ATTR_TYPE_U8,             ACC_READ_ONLY,  ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u8_ro    },
  { 0x0007,      ZB_ZCL_ATTR_TYPE_U8,             ACC_WRITE_ONLY, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u8_wo    },
  { 0x0008,      ZB_ZCL_ATTR_TYPE_U8,             ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u8_rw    },
  { 0x0009,      ZB_ZCL_ATTR_TYPE_U8,             ACC_MANUF_SPEC, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u8_ms    },
  { 0x000a,      ZB_ZCL_ATTR_TYPE_U16,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u16      },
  { 0x000b,      ZB_ZCL_ATTR_TYPE_U24,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u24      },
  { 0x000c,      ZB_ZCL_ATTR_TYPE_U32,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u32      },
  { 0x000d,      ZB_ZCL_ATTR_TYPE_U40,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u40      },
  { 0x000e,      ZB_ZCL_ATTR_TYPE_U48,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u48      },
  { 0x000f,      ZB_ZCL_ATTR_TYPE_U56,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u56      },
  { 0x0010,      ZB_ZCL_ATTR_TYPE_U64,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_u64      },
  { 0x0011,      ZB_ZCL_ATTR_TYPE_S16,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_s16      },
  { 0x0012,      ZB_ZCL_ATTR_TYPE_8BIT_ENUM,       ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_8_enum   },
  { 0x0013,      ZB_ZCL_ATTR_TYPE_16BIT_ENUM,     ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_16_enum  },
  { 0x0014,      ZB_ZCL_ATTR_TYPE_SEMI,           ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_semi     },
  { 0x0015,      ZB_ZCL_ATTR_TYPE_SINGLE,         ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_single   },
  { 0x0016,      ZB_ZCL_ATTR_TYPE_DOUBLE,         ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_double   },
  { 0x0017,      ZB_ZCL_ATTR_TYPE_OCTET_STRING,   ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_os       },
  { 0x0018,      ZB_ZCL_ATTR_TYPE_CHAR_STRING,    ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.attr_cs       },
  { 0x0019,      ZB_ZCL_ATTR_TYPE_ARRAY,          ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_array  },
  { 0x001a,      ZB_ZCL_ATTR_TYPE_STRUCTURE,      ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_struct },
  { 0x001b,      ZB_ZCL_ATTR_TYPE_SET,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_set    },
  { 0x001c,      ZB_ZCL_ATTR_TYPE_BAG,            ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_bag    },
  { 0x001d,      ZB_ZCL_ATTR_TYPE_TIME_OF_DAY,    ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_time   },
  { 0x001e,      ZB_ZCL_ATTR_TYPE_DATE,           ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_date   },
  { 0x001f,      ZB_ZCL_ATTR_TYPE_UTC_TIME,       ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_utc    },
  { 0x0020,      ZB_ZCL_ATTR_TYPE_CLUSTER_ID,     ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_clid   },
  { 0x0021,      ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID,   ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_id     },
  { 0x0022,      ZB_ZCL_ATTR_TYPE_IEEE_ADDR,      ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_ieee   },
  { 0x0023,      ZB_ZCL_ATTR_TYPE_128_BIT_KEY,    ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_128    },
  { 0x0024,      ZB_ZCL_ATTR_TYPE_CUSTOM_32ARRAY, ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_32ar   },
  { 0x0025,      ZB_ZCL_ATTR_TYPE_INVALID,        ACC_READ_WRITE, ZB_MANUFACTURER_CODE_NXP, (void*)&g_custom_nxp_attr.g_attr_inval  },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             ZB_MANUFACTURER_CODE_NXP, NULL }
};

void zb_zcl_test_init_server()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                              ZB_ZCL_CLUSTER_SERVER_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

void zb_zcl_test_init_client()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                              ZB_ZCL_CLUSTER_CLIENT_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

static zb_cluster_def cluster_fc02 = {
  cluster_attr_fc02,
  sizeof(cluster_attr_fc02)/sizeof(zb_zcl_attr_t),
  zb_zcl_test_init_server,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_test_init_client,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  custom_nxp_commands_handler,
};

/* -------------------------------- Cli commands custom NXP --------------------------- */

/* Prototype of custom NXP sub commands */
static zb_ret_t cluster_custnxp_cmd_payload(int argc, char *argv[]);

/* Static command cluster
 * command custom_nxp
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_custnxp_cmds(void);
#endif
static zb_ret_t help_custnxp_cmds_detailed(char *subcommand);
static zb_ret_t cluster_custnxp_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "payload", ZB_ZCL_CMD_NXP_PAYLOAD }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
   return help_custnxp_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_NXP_PAYLOAD:
        return cluster_custnxp_cmd_payload(argc-1, &argv[1]);
    }
  }

  /* ... */

  /* not found, print help */
  menu_printf("cluster custnxp %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_custnxp_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_custnxp_cmds(void) { return help_custnxp_cmds_detailed(NULL); }
static zb_ret_t help_custnxp_cmds_detailed(char *subcommand)
{
  /* SUBMENU CUSTOM NXP Commands */
  if(!subcommand || !strcmp(subcommand, "payload"))
  {
    menu_printf("cluster custom_nxp_cmd payload [endpoint] [dest_addr] [dest_ep] [number] [size] [interval]:");
    menu_printf("\tsend ZCL Command PAYLOAD from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] repeat number [0-255] payload size [0-%d] delay between cmds in ms [0-65535]", MAX_PAYLOAD_SIZE);
  }
  return RET_OK;
}


enum zb_zcl_custom_nxp_cmd_e
{
  ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_REQ              = 0x00,
  ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_RES              = 0x01
};

typedef struct zb_zcl_custom_nxp_payload_hdr_s{
  zb_uint8_t  repeat_number;  /* Number of pings to be sent */
  zb_uint8_t  payload_size;   /* ping payload size without ping header (4 bytes) */
  zb_uint16_t interval;       /*in ms */
} zb_zcl_custom_nxp_payload_hdr_t;

typedef struct custnxp_cmd_payload_params_s
{
  zb_af_endpoint_desc_t *this_ep;
  zb_zcl_custom_nxp_payload_hdr_t hdr;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
} custnxp_cmd_payload_params_t;

/* Static stack callback function
 * response for custnxp_cmd_payload
 */
static void custnxp_cmd_payload_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("custnxp_cmd_payload_cb() %d: %s", cmd_send_status->status, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}

/**
 * Command core, schedule itself until ping number has been reached
 */
static void send_cmd_payload_req(zb_uint8_t param)
{
  static zb_uint8_t pkt_num = 1;
  zb_bool_t is_manuf_specific = ZB_TRUE;

  zb_bufid_t buffer = zb_buf_get_out();
  zb_uint8_t* ptr = zb_buf_begin(buffer);

  custnxp_cmd_payload_params_t *dev_params = (custnxp_cmd_payload_params_t *)zb_buf_begin(param);

  /* Send ping */
  menu_printf("Packet number: %d", pkt_num);

  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, is_manuf_specific, ZB_ZCL_DISABLE_DEFAULT_RESPONSE);
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), is_manuf_specific, ZB_MANUFACTURER_CODE_NXP, ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_REQ);
  ZB_ZCL_PACKET_PUT_DATA8(ptr, dev_params->hdr.repeat_number);
  ZB_ZCL_PACKET_PUT_DATA8(ptr, dev_params->hdr.payload_size);
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, dev_params->hdr.interval);
  for(zb_uint8_t i=0; i < dev_params->hdr.payload_size; i++)
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (zb_uint8_t)ZB_RANDOM_VALUE(255));
  ZB_ZCL_FINISH_PACKET(buffer, ptr) \
    ZB_ZCL_SEND_COMMAND_SHORT(buffer,
                              dev_params->dest_short_addr,
                              ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                              dev_params->dest_ep_id,
                              dev_params->this_ep->ep_id,
                              dev_params->this_ep->profile_id,
                              ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                              custnxp_cmd_payload_cb);
  pkt_num++;

  /* Schedule next ping */
  if( dev_params->hdr.repeat_number > 1 )
  {
    ZB_SCHEDULE_APP_ALARM(send_cmd_payload_req, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(dev_params->hdr.interval));

    dev_params->hdr.repeat_number--;
  }
  else
  {
    pkt_num = 1;
    zb_buf_free(param);

    /* TODO: Add statistics like time, % of success ... */
  }
}

/* Static command cluster
 * command custom_nxp_cmd
 *
 * cluster custom_nxp_cmd payload [endpoint] [dest_addr] [dest_ep] [number] [size] [interval]
 */
static zb_ret_t cluster_custnxp_cmd_payload(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_zcl_custom_nxp_payload_hdr_t hdr;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster custom_nxp client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_CUSTOM_NXP, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [number] */
  TOOLS_GET_ARG(ret, uint8,  argv, 3, &hdr.repeat_number);

  /* get [size] */
  TOOLS_GET_ARG(ret, uint8,  argv, 4, &hdr.payload_size);
  if(hdr.payload_size > MAX_PAYLOAD_SIZE)
    return RET_INVALID_PARAMETER_5;

  /* get [interval] */
  TOOLS_GET_ARG(ret, uint16,  argv, 5, &hdr.interval);

  /* Do it */
  {
    zb_bufid_t param = zb_buf_get_out();
    custnxp_cmd_payload_params_t *dev_params;

    dev_params = zb_buf_initial_alloc(param, sizeof(custnxp_cmd_payload_params_t));
    dev_params->this_ep = this_ep;
    dev_params->hdr = hdr;
    dev_params->dest_short_addr = dest_short_addr;
    dev_params->dest_ep_id = dest_ep_id;
    ZB_SCHEDULE_APP_CALLBACK(send_cmd_payload_req, param);
  }
  return RET_OK;
}


/* -------------------------------- Handler commands custom NXP --------------------------- */

static zb_uint8_t custom_nxp_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manage it */
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_REQ:
      {
        zb_zcl_custom_nxp_payload_hdr_t *payload_req;
        zb_uint8_t *payload_data;
        zb_uint8_t param_res;
        zb_uint8_t* ptr;
        zb_bool_t is_manuf_specific = ZB_TRUE;

        if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_payload_hdr_t))
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, recv buffer too short",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            "CMD_CUSTOM_NXP_PAYLOAD_REQ");
          break;
        }

        payload_req = (zb_zcl_custom_nxp_payload_hdr_t *)zb_buf_begin(param);

        if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_payload_hdr_t) + payload_req->payload_size)
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, recv payload too short",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            "CMD_CUSTOM_NXP_PAYLOAD_REQ");
          break;
        }

        payload_data = (zb_uint8_t *)payload_req;
        payload_data += sizeof(zb_zcl_custom_nxp_payload_hdr_t);

        /* Request correct (shall we check as well the payload ?) */
        menu_printf("%s() recv from 0x%04x endpoint %u: %s OK, send response",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          "CMD_CUSTOM_NXP_PAYLOAD_REQ");

        ret = ZB_TRUE;
        param_res = zb_buf_get_out();
        ptr = zb_buf_begin(param_res);
        ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr, ZB_ZCL_FRAME_DIRECTION_TO_CLI, is_manuf_specific, ZB_ZCL_DISABLE_DEFAULT_RESPONSE);
        ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), is_manuf_specific, ZB_MANUFACTURER_CODE_NXP, ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_RES);
        ZB_ZCL_PACKET_PUT_DATA8(ptr, payload_req->repeat_number);
        ZB_ZCL_PACKET_PUT_DATA8(ptr, payload_req->payload_size);
        ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, payload_req->interval);
        for(zb_uint8_t i=0; i < payload_req->payload_size; i++)
          ZB_ZCL_PACKET_PUT_DATA8(ptr, payload_data[i]);
        ZB_ZCL_FINISH_PACKET(param_res, ptr)                                                        \
          ZB_ZCL_SEND_COMMAND_SHORT(param_res,
                                    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
                                    ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                                    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
                                    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
                                    cmd_info->profile_id,
                                    cmd_info->cluster_id,
                                    custnxp_cmd_payload_cb);
        // Need to free param ?
        //zb_buf_free(param);
      }

      break;

    default:
      menu_printf("%s() recv from 0x%04x endpoint %u to %u:%s: unknown cmd 0x%04x, call dummy handler",
        __FUNCTION__,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
        (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
        cmd_info->cmd_id);
        dummy_commands_handler(cmd_info, param);
      break;

  }

  //Free param ?
  zb_buf_free(param);
  return ret;
}

static zb_uint8_t custom_nxp_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_CUSTOM_NXP_PAYLOAD_RES:
      {
        zb_zcl_custom_nxp_payload_hdr_t *payload_res;

        if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_payload_hdr_t))
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, recv buffer too short",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            "CMD_CUSTOM_NXP_PAYLOAD_RES");
          break;
        }

        payload_res = (zb_zcl_custom_nxp_payload_hdr_t *)zb_buf_begin(param);

        if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_payload_hdr_t) + payload_res->payload_size)
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, recv payload too short",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            "CMD_CUSTOM_NXP_PAYLOAD_RES");
          break;
        }

        /* Response correct (shall we check as well the payload ?) */
        ret = ZB_TRUE; /* /!\ Free param manually as the stack will not do it */
        zb_buf_free(param);

        menu_printf("%s() recv from 0x%04x endpoint %u: %s OK",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          "CMD_CUSTOM_NXP_PAYLOAD_RES");
      }
      break;


    default:
      menu_printf("%s() recv from 0x%04x endpoint %u to %u:%s: unknown cmd 0x%04x, call dummy handler",
        __FUNCTION__,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
        (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
        cmd_info->cmd_id);
        dummy_commands_handler(cmd_info, param);
      break;
  }

  return ret;
}

static zb_uint8_t custom_nxp_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(custom_nxp_client_commands_handler(cmd_info, param)):(custom_nxp_server_commands_handler(cmd_info, param));
}
