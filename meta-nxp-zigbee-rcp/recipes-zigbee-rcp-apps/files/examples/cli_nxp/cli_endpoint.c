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

#define ZB_TRACE_FILE_ID 33614
#include <ctype.h>
#include "zboss_api.h"
#include "cli_menu.h"
#include "cli_config.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"
#include "cli_tools.h"


#define MAX_ENDPOINTS 256

#define MAX_CLUSTERS 256

/* Declare zb_af_simple_desc_##MAX_CLUSTERS_SERVERS##_##MAX_CLUSTERS_SERVERS##_t => zb_af_simple_desc_256_0_t */
//ZB_DECLARE_SIMPLE_DESC(MAX_CLUSTERS_SERVERS,MAX_CLUSTERS_CLIENTS);
ZB_DECLARE_SIMPLE_DESC(256,0);
#define STRUCT_SIMPLE_DESC_NAME zb_af_simple_desc_256_0_t

zb_af_endpoint_desc_t *cli_endpoints[MAX_ENDPOINTS] = { 0 };

/* Context containing endpoints table, each endpoint containing clusters table */
zb_af_device_ctx_t cli_ctx = {
  .ep_count = 0,
  cli_endpoints,
};


zb_endpoint_profile table_profiles[] = {
  { 0x0000, "ZigBee Device Object",           "ZDO", "  " }, /* ZDO profile ID */
  { 0x0101, "Industrial Plant Monitoring",    "IPM", "  " }, /* Legacy profile */
  { 0x0104, "Home Automation",                "HA", "   " }, /* Legacy profile */
  { 0x0105, "Commercial Building Automation", "CBA", "  " }, /* Legacy profile */
  { 0x0107, "Telecom Applications",           "TA", "   " }, /* Legacy profile */
  { 0x0108, "Personal Home & Hospital Care",  "PHHC", " " }, /* Legacy profile */
  { 0x0109, "Advanced Metering Initiative",   "AMI", "  " }, /* SE profile ID */
  { 0xA1E0, "Green Power",                    "GP", "   " }, /* GreenPower profile ID */
  { 0xC05E, "ZigBee Light Link",              "ZLL", "  " }, /* ZLL profile identifier. */
  { 0x7F01, "Test",                           "T", "    " }, /* Should not be used, it has unexpected behavior by sending wrong frames */
  { 0xFFFF, "Wildcard",                       "W", "    " }, /* Wildcard profile identifier. */
  { 0xFFFF, NULL /* End of table detector */, NULL, NULL }
};

void help_profiles(void)
{
  for(int i=0; table_profiles[i].name != NULL; i++)
    menu_printf("  0x%04x - %s: %s%s", table_profiles[i].id, table_profiles[i].initials, table_profiles[i].align, table_profiles[i].name);
}

zb_endpoint_device endpoint_devices[] = {
  /* Generic */
  { 0x0000, "Generic",  "ON/OFF Switch",             "GOFS", "  " },
  { 0x0001, "Generic",  "Level Control Switch",      "GLCS", "  " },
  { 0x0002, "Generic",  "ON/OFF Output",             "GOFO", "  " },
  { 0x0003, "Generic",  "Level Controllable Output", "GLCO", "  " },
  { 0x0004, "Generic",  "Scene Selector",            "GSS", "   " },
  { 0x0005, "Generic",  "Configuration Tool",        "GCT", "   " },
  { 0x0006, "Generic",  "Remote Control",            "GRC", "   " },
  { 0x0007, "Generic",  "Combined Interface",        "GCI", "   " },
  { 0x0008, "Generic",  "Range Extender",            "GRE", "   " },
  { 0x0009, "Generic",  "Mains Power Outlet",        "GMPO", "  " },
  { 0x000A, "Generic",  "Door Lock",                 "GDL", "   " },
  { 0x000B, "Generic",  "Door Lock Controller",      "GDLC", "  " },
  { 0x000C, "Generic",  "Simple Sensor",             "GSSr", "  " },
  { 0x000D, "Generic",  "Consumption Awareness",     "GCA", "   " },
  { 0x0050, "Generic",  "Home Gateway",              "GHG", "   " },
  { 0x0051, "Generic",  "Smart plug",                "GSP", "   " },
  { 0x0052, "Generic",  "White Goods",               "GWG", "   " },
  { 0x0053, "Generic",  "Meter Interface",           "GMI", "   " },
  /* Lighting */
  { 0x0100, "Lighting", "ON/OFF Light",          "LOOL", "  " },
  { 0x0101, "Lighting", "Dimmable Light",        "LDL", "   " },
  { 0x0102, "Lighting", "Color Dimmable Light",  "LCDL", "  " },
  { 0x0103, "Lighting", "ON/OFF Light Switch",   "LOOLS", " " },
  { 0x0104, "Lighting", "Dimmer Switch",         "LDS", "   " },
  { 0x0105, "Lighting", "Color Dimmer Switch",   "LCDS", "  " },
  { 0x0106, "Lighting", "Light Sensor",          "LLS", "   " },
  { 0x0107, "Lighting", "Occupancy Sensor",      "LOS", "   " },
  /* Closures */
  { 0x0200, "Closures", "Shade",                      "CS", "    " },
  { 0x0201, "Closures", "Shade Controller",           "CSC", "   " },
  { 0x0202, "Closures", "Window Covering",            "CWC", "   " },
  { 0x0203, "Closures", "Window Covering Controller", "CWCC", "  " },
  /* HVAC */
  { 0x0300, "HVAC",     "Heating/Cooling Unit", "HHCU", "  " },
  { 0x0301, "HVAC",     "Thermostat",           "HT", "    " },
  { 0x0302, "HVAC",     "Temperature Sensor",   "HTS", "   " },
  { 0x0303, "HVAC",     "Pump",                 "HP", "    " },
  { 0x0304, "HVAC",     "Pump Controller",      "HPC", "   " },
  { 0x0305, "HVAC",     "Pressure Sensor",      "HPS", "   " },
  { 0x0306, "HVAC",     "Flow Sensor",          "HFS", "   " },
  /* Intruder Alarm Systems */
  { 0x0400, "Intruder Alarm Systems", "IAS Control and Indicating Equipment", "ICIE", "  " },
  { 0x0401, "Intruder Alarm Systems", "IAS Ancillary Control Equipment",      "IACE", "  " },
  { 0x0402, "Intruder Alarm Systems", "IAS Zone",                             "IZ", "    " },
  { 0x0403, "Intruder Alarm Systems", "IAS Warning Device",                   "IWD", "   " },
  /* Misc */
  { 0xFFF0, "Misc", "Test",                   "MT", "    " },
  { 0xFFF1, "Misc", "Custom Tunnel",          "MCT", "   " },
  { 0xFFF2, "Misc", "Custom Attributes",      "MCA", "   " },
  /* ... */
  { 0xFFFF, NULL,                    NULL,     NULL, NULL }
};

static void help_device_type(void)
{
  for(int i=0; endpoint_devices[i].id != 0xFFFF; i++)
    menu_printf("  0x%04x - %s: %s[%s] %s", endpoint_devices[i].id, endpoint_devices[i].initials, endpoint_devices[i].align, endpoint_devices[i].group, endpoint_devices[i].name);
}


/* Global endpoint function
 * get endpoint by id
 */
zb_af_endpoint_desc_t *endpoint_get_by_id(zb_uint8_t ep_id)
{
    for(int i=0; i < MAX_ENDPOINTS; i++)
      if(cli_ctx.ep_desc_list[i] &&
         cli_ctx.ep_desc_list[i]->ep_id == ep_id)
        return cli_ctx.ep_desc_list[i];

  return NULL;
}

/* Static printing function
 * print info of incoming packet: EP, Profile, cluster, SeqNum
 */
static void print_profilewide_cmd_info(zb_zcl_parsed_hdr_t *cmd_info, char *extra, ...)
{
  va_list args;
  char ExtraInfo[256] = { "" };

  va_start(args, extra);
  vsnprintf(ExtraInfo, 256, extra, args);
  va_end (args);

  menu_printf("Recv from 0x%04x Endpoint %u to %u, %s %s Profile 0x%04x Cluster %s SeqNum %d: %s",
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
    (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
    get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id),
    cmd_info->profile_id,
    get_cluster_id_str(cmd_info->cluster_id),
    cmd_info->seq_number,
    ExtraInfo);
}

/* Static stack callback function
 * endpoint command handler
 */
static zb_uint8_t endpoint_commands_handler(zb_uint8_t param)
{
  zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(param, zb_zcl_parsed_hdr_t);
  zb_af_endpoint_desc_t *this_ep;
  zb_uint8_t cmd_processed = 0;

  /* Search the endpoint */
  this_ep = endpoint_get_by_id(ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint);
  if(!this_ep)
  {
    menu_printf("Recv from 0x%04x Endpoint %u to %u, %s %s Cluster %s Profile 0x%04x SeqNum %d: ENDPOINT NOT FOUND",
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
      (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
      get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id),
      get_cluster_id_str(cmd_info->cluster_id),
      cmd_info->profile_id,
      cmd_info->seq_number);

    return cmd_processed;
  }

  if(cmd_info->is_common_command)  /* ZCL FrameType 00: Profile-wide */
  {
    // zb_bool_t require_resp = (cmd_info->disable_default_response == ZB_FALSE);
    char ExtraInfo[256] = { "" };
    int ExtraLen = 0;

    switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_REPORT_ATTRIB:        /*!< 0x0a: Report attribute command */
      {
        zb_zcl_report_attr_req_t *rep_attr_req;
        zb_uint8_t param_tmp = zb_buf_get_any();

        zb_buf_copy(param_tmp, param);

        print_profilewide_cmd_info(cmd_info, "");

        ZB_ZCL_GENERAL_GET_NEXT_REPORT_ATTR_REQ(param_tmp, rep_attr_req);
        while(rep_attr_req)
        {
          ExtraLen = wcs_snprintf(ExtraInfo, 256, "\tAttr 0x%04x:%s: ",
            rep_attr_req->attr_id,
            get_cluster_attr_id_str(cmd_info->cluster_id, rep_attr_req->attr_id));
          ExtraLen += wcs_snprintf_attr_val(ExtraInfo+ExtraLen, 256-ExtraLen, rep_attr_req->attr_type, rep_attr_req->attr_value);
          ZB_ZCL_GENERAL_GET_NEXT_REPORT_ATTR_REQ(param_tmp, rep_attr_req);
          if(rep_attr_req) ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ", ");
          menu_printf("%s", ExtraInfo);
        }
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_DEFAULT_RESP:         /*!< 0x0b: Default response command */
      {
        zb_zcl_default_resp_payload_t* default_resp_payload = ZB_ZCL_READ_DEFAULT_RESP(param);

        print_profilewide_cmd_info(cmd_info, "");

        if(default_resp_payload)
        {
          menu_printf("\t%s, Resp %s",
            get_cmd_id_str(ZB_FALSE, cmd_info->cluster_id, default_resp_payload->command_id),
            get_zcl_status_str(default_resp_payload->status));
        }
        else
        {
          menu_printf( "\tERROR: cannot extract default_resp_payload");
        }
      }
      break;

    case ZB_ZCL_CMD_READ_ATTRIB:          /*!< 0x00: Read attributes command */
      {
        zb_zcl_read_attr_req_t *read_attr_req;
        zb_uint8_t param_tmp = zb_buf_get_any();

        zb_buf_copy(param_tmp, param);

        print_profilewide_cmd_info(cmd_info, "");

        ZB_ZCL_GENERAL_GET_READ_ATTR_REQ(param_tmp, read_attr_req);
        menu_printf("\tRead Attr 0x%04x:%s",
          read_attr_req->attr_id[0],
          get_cluster_attr_id_str(cmd_info->cluster_id, read_attr_req->attr_id[0]));
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_READ_ATTRIB_RESP:     /*!< 0x01: Read attributes response command */
      {
        zb_zcl_read_attr_res_t *read_attr_resp;
        zb_uint8_t param_tmp = zb_buf_get_any();

        zb_buf_copy(param_tmp, param);

        print_profilewide_cmd_info(cmd_info, "");

        ZB_ZCL_GENERAL_GET_NEXT_READ_ATTR_RES(param_tmp, read_attr_resp);
        while(read_attr_resp)
        {
          ExtraLen = wcs_snprintf(ExtraInfo, 256, "\tRead Attr 0x%04x:%s",
            read_attr_resp->attr_id,
            get_cluster_attr_id_str(cmd_info->cluster_id, read_attr_resp->attr_id));
          if(read_attr_resp->status == ZB_ZCL_STATUS_SUCCESS)
          {
            ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ": ");
            ExtraLen += wcs_snprintf_attr_val(ExtraInfo+ExtraLen, 256-ExtraLen, read_attr_resp->attr_type, read_attr_resp->attr_value);
            ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ", ");
          }
          ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, " Resp %s", get_zcl_status_str(read_attr_resp->status));
          ZB_ZCL_GENERAL_GET_NEXT_READ_ATTR_RES(param_tmp, read_attr_resp);
          if(read_attr_resp) ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ", ");
          menu_printf("%s", ExtraInfo);
        }
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_WRITE_ATTRIB:         /*!< 0x02: Write attributes foundation command */
    case ZB_ZCL_CMD_WRITE_ATTRIB_UNDIV:   /*!< 0x03: Write attributes undivided command */
      {
        zb_zcl_write_attr_req_t *write_attr_req;
        zb_uint8_t param_tmp = zb_buf_get_any();
        zb_uint8_t *buf_ptr;
        zb_uint8_t buf_len;

        zb_buf_copy(param_tmp, param);

        print_profilewide_cmd_info(cmd_info, "");

        buf_ptr = zb_buf_begin(param_tmp);
        buf_len = zb_buf_len(param_tmp);
        ZB_ZCL_GENERAL_GET_NEXT_WRITE_ATTR_REQ(buf_ptr, buf_len, write_attr_req);
        while(write_attr_req)
        {
          ExtraLen = wcs_snprintf(ExtraInfo, 256, "\tWrite Attr 0x%04x:%s: ",
            write_attr_req->attr_id,
            get_cluster_attr_id_str(cmd_info->cluster_id, write_attr_req->attr_id));
          ExtraLen += wcs_snprintf_attr_raw(ExtraInfo+ExtraLen, 256-ExtraLen, write_attr_req->attr_type, write_attr_req->attr_value);
          ZB_ZCL_GENERAL_GET_NEXT_WRITE_ATTR_REQ(buf_ptr, buf_len, write_attr_req);
          if(write_attr_req) ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ", ");
          menu_printf("%s", ExtraInfo);
        }
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_WRITE_ATTRIB_RESP:    /*!< 0x04: Write attributes response command */
    case ZB_ZCL_CMD_WRITE_ATTRIB_NO_RESP: /*!< 0x05: Write attributes no response command */
      {
        zb_zcl_write_attr_res_t *write_attr_resp;
        zb_uint8_t param_tmp = zb_buf_get_any();

        zb_buf_copy(param_tmp, param);

        print_profilewide_cmd_info(cmd_info, "");

        ZB_ZCL_GET_NEXT_WRITE_ATTR_RES(param_tmp, write_attr_resp);
        while(write_attr_resp)
        {
          ExtraLen = wcs_snprintf(ExtraInfo, 256, "\tWrite Attr ");
          /* In case of success the attribute is not provided */
          if(write_attr_resp->status != ZB_ZCL_STATUS_SUCCESS)
            ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, "0x%04x:%s, ",
              write_attr_resp->attr_id,
              get_cluster_attr_id_str(cmd_info->cluster_id, write_attr_resp->attr_id));
          ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, "Resp %s",
            get_zcl_status_str(write_attr_resp->status));
          ZB_ZCL_GET_NEXT_WRITE_ATTR_RES(param_tmp, write_attr_resp);
          if(write_attr_resp) ExtraLen += wcs_snprintf(ExtraInfo+ExtraLen, 256-ExtraLen, ", ");
          menu_printf("%s", ExtraInfo);
        }
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_DISC_ATTRIB:          /*!< 0x0c: Discover attributes command */
      {
        zb_zcl_disc_attr_req_t *disc_attr_req;

        disc_attr_req = (zb_zcl_disc_attr_req_t *)zb_buf_begin(param);
        print_profilewide_cmd_info(cmd_info, "start_id: 0x%04x, max: %u", disc_attr_req->start_attr_id, disc_attr_req->maximum);
      }
      break;

    case ZB_ZCL_CMD_DISC_ATTRIB_RESP:     /*!< 0x0d: Discover attributes response command */
      {
//        zb_zcl_disc_attr_res_t *disc_attr_res = (zb_zcl_disc_attr_res_t *)zb_buf_begin(param);
          zb_uint8_t complete;                     /*!< Discovery complete */
          zb_zcl_disc_attr_info_t *disc_attr_info; /*!< Attribute desc list */

        zb_uint8_t param_tmp = zb_buf_get_any();

        zb_buf_copy(param_tmp, param);

        ZB_ZCL_GENERAL_GET_COMPLETE_DISC_RES(param_tmp, complete);
        print_profilewide_cmd_info(cmd_info, "complete %d", /*disc_attr_res->*/complete);

        ZB_ZCL_GENERAL_GET_NEXT_DISC_ATTR_RES(param_tmp, disc_attr_info);
        while(disc_attr_info)
        {
          menu_printf("\tDisc Attr 0x%04x:%s, type 0x%02x:%s",
            disc_attr_info->attr_id,
            get_cluster_attr_id_str(cmd_info->cluster_id, disc_attr_info->attr_id),
            disc_attr_info->data_type,
            get_attr_type_str(disc_attr_info->data_type));

          ZB_ZCL_GENERAL_GET_NEXT_DISC_ATTR_RES(param_tmp, disc_attr_info);
        }

        cmd_processed = ZB_TRUE;
        zb_buf_free(param_tmp);
      }
      break;

    case ZB_ZCL_CMD_CONFIG_REPORT:        /*!< 0x06: Configure reporting command */
    case ZB_ZCL_CMD_CONFIG_REPORT_RESP:   /*!< 0x07: Configure reporting response command */
    case ZB_ZCL_CMD_READ_REPORT_CFG:      /*!< 0x08: Read reporting config command */
    case ZB_ZCL_CMD_READ_REPORT_CFG_RESP: /*!< 0x09: Read reporting config response command */
      print_profilewide_cmd_info(cmd_info, "TBD");
      break;
    default:
      print_profilewide_cmd_info(cmd_info, "UNKNOWN cmd 0x%04x", cmd_info->cmd_id);
      break;
    }
  }
  else                             /* ZCL FrameType 01: Cluster-specific */
  {
    /* Search the cluster */
    for(int i=0; i < this_ep->cluster_count; i++)
    {
      zb_zcl_cluster_desc_t *this_cluster = &this_ep->cluster_desc_list[i];

      if(cmd_info->cluster_id == this_cluster->cluster_id)
      {
        /* Check if that cluster has been created */
        if((cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI && this_cluster->role_mask & ZB_ZCL_CLUSTER_CLIENT_ROLE) ||
           (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV && this_cluster->role_mask & ZB_ZCL_CLUSTER_SERVER_ROLE))
         {
           /* Handle cluster command */
           return cluster_commands_handler(cmd_info, param);
         }
      }
    }

    menu_printf("Recv from 0x%04x Endpoint %u to %u, %s %s Cluster %s SeqNum %d: CLUSTER NOT FOUND",
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
      ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
      (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
      get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id),
      get_cluster_id_str(cmd_info->cluster_id),
      cmd_info->seq_number);
  }

  menu_cb_occured();

  /* CAUTION: if TRUE is returned:
   * - It means this command is processed by the command handler
   * - Stack will not handle any processing
   * - Allocated buffers used to handle command must be freed explicitely by the user as the stack will not do it
   */

  return cmd_processed;
}


/* Static stack callback function
 * identify notification handler for endpoint
 */
static void endpoint_identify_notification_handler(zb_uint8_t param)
{
  menu_printf("Recv identification request %s", (param == 1)?("enable"):("disable"));
  if(param)
  {
    /* LED identify on/blink */
  }
  else
  {
    /* LED dentify off */
  }

  menu_cb_occured();
}


/* Commands endpoint handlers */
static zb_ret_t endpoint_create(int argc, char *argv[]);      static zb_ret_t help_create(void);
static zb_ret_t endpoint_print(int argc, char *argv[]);

/* Menu endpoint */
cli_menu_cmd menu_endpoint[] = {
  /* name, args,                             align,      function,             help,        description */
  { "create", " [id] [profile] [device] [version]", " ", endpoint_create,      help_create, "create an endpoint id: [0-255] profile [0xPPPP|initials], device [0xDDDD|initials], version [0-255]" },
  { "print", "", "                                    ", endpoint_print,       help_empty,  "print the endpoint table" },
  /* Add new commands above here */
  { NULL, NULL,                                    NULL, NULL,                 NULL,        NULL }
};


/* Static command endpoint
 * command create
 *
 * endpoint create [id] [profile] [device] [version]
 */
static zb_ret_t endpoint_create(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_id;
  zb_uint16_t new_profile = 0xFFFF;
  zb_uint16_t new_device  = 0xFFFF;
  zb_uint8_t new_version;

  if(config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 4)
    return RET_INVALID_PARAMETER;

  /* get [id] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_id);

  /* get [profile] */
  TOOLS_GET_ARG(ret, profile, argv, 1, &new_profile);

  /* get [device] */
  TOOLS_GET_ARG(ret, device_type, argv, 2, &new_device);

  /* get [version] */
  TOOLS_GET_ARG(ret, uint8, argv, 3, &new_version);
  if(new_version >= 16) /* stored on 4 bits */
    return RET_INVALID_PARAMETER_4;

  /* Params are good, insert that new endpoint in our context */
  {
    zb_af_endpoint_desc_t *new_ep;

    /* Check if that endpoint already exists */
    if(endpoint_get_by_id(new_id) != NULL)
        return RET_ALREADY_EXISTS;

    /* Allocate EP storage */
    new_ep = (zb_af_endpoint_desc_t *)malloc(sizeof(zb_af_endpoint_desc_t));
    if(!new_ep)
      return RET_NO_MEMORY;
    memset(new_ep, 0, sizeof(zb_af_endpoint_desc_t));

    new_ep->ep_id             = new_id;
    new_ep->profile_id        = new_profile;
    /* Normally registered by ZB_AF_SET_ENDPOINT_HANDLER() */
    new_ep->device_handler    = endpoint_commands_handler;
    /* Normally registered by ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER() */
    new_ep->identify_handler  = endpoint_identify_notification_handler;
    /* clusters entry point */
    new_ep->cluster_count     = 0;
    /* To be alloc & realloc for each cluster */
    new_ep->cluster_desc_list = (zb_zcl_cluster_desc_t *)malloc((MAX_CLUSTERS)*sizeof(zb_zcl_cluster_desc_t));
    if(!new_ep->cluster_desc_list)
    {
      free(new_ep);
      return RET_NO_MEMORY;
    }
    memset(new_ep->cluster_desc_list, 0, (MAX_CLUSTERS)*sizeof(zb_zcl_cluster_desc_t));
    new_ep->simple_desc       = (zb_af_simple_desc_1_1_t *)malloc(sizeof(STRUCT_SIMPLE_DESC_NAME));
    if(!new_ep->simple_desc)
    {
      free(new_ep->cluster_desc_list);
      free(new_ep);
      return RET_NO_MEMORY;
    }
    memset(new_ep->simple_desc, 0, sizeof(STRUCT_SIMPLE_DESC_NAME));
    new_ep->simple_desc->endpoint                 = new_id;
    new_ep->simple_desc->app_profile_id           = new_profile;
    new_ep->simple_desc->app_device_id            = new_device;
    new_ep->simple_desc->app_device_version       = new_version;
    /* clusters list entry point */
    new_ep->simple_desc->app_input_cluster_count  = 0; /* Servers: Max 8 */
    new_ep->simple_desc->app_output_cluster_count = 0; /* Clients: Max 9 */
    memset(new_ep->simple_desc->app_cluster_list, 0, sizeof(new_ep->simple_desc->app_cluster_list));

#if defined ZB_ENABLE_ZLL
    new_ep->group_id_count = 0U;
#endif /* defined ZB_ENABLE_ZLL */

#if !defined(ZB_ZCL_DISABLE_REPORTING)
    new_ep->rep_info_count = 20;
    new_ep->reporting_info = (zb_zcl_reporting_info_t *)malloc(new_ep->rep_info_count * sizeof(zb_zcl_reporting_info_t));
    if(!new_ep->reporting_info)
    {
      free(new_ep->simple_desc);
      free(new_ep->cluster_desc_list);
      free(new_ep);
      return RET_NO_MEMORY;
    }
#endif /* !ZB_ZCL_DISABLE_REPORTING */

    new_ep->cvc_alarm_count = 0U;
    new_ep->cvc_alarm_info = NULL;

    /* Finally record it */
    for(int i=0; i < MAX_ENDPOINTS; i++)
      if(!cli_ctx.ep_desc_list[i])
      {
        cli_ctx.ep_desc_list[i] = new_ep;
        cli_ctx.ep_count++;
        return RET_OK;
      }

    /* ep_desc_list is full, ABORT */
    free(new_ep->simple_desc);
    free(new_ep);
  }

  return RET_NO_RESOURCES;
}
static zb_ret_t help_create(void)
{
  menu_printf("profile: either its 16bits id value or its initial in:");
  help_profiles();
  menu_printf("device: either its 16bits id value or its initial in:");
  help_device_type();
  return RET_OK;
}


/* Static command endpoint
 * command print_table
 */
static zb_ret_t endpoint_print(int argc, char *argv[])
{
  ARGV_UNUSED;

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  for(int i=0; i < MAX_ENDPOINTS; i++)
  {
    zb_af_endpoint_desc_t *this_ep = cli_ctx.ep_desc_list[i];
    char listStr[256];
    int  listLen;

    if(!this_ep)
      continue;

    menu_printf("endpoint %u: profile 0x%04x=\'%s\', device 0x%04x=\'%s\', version %u, %u clusters",
      this_ep->ep_id, 
      this_ep->profile_id,                           /* Also stored in this_ep->simple_desc->app_profile_id */
      get_endpoint_profile_id_str(this_ep->profile_id),
      this_ep->simple_desc->app_device_id,
      get_endpoint_device_id_str(this_ep->simple_desc->app_device_id),
      this_ep->simple_desc->app_device_version,
      this_ep->cluster_count                         /* Should be equal to this_ep->simple_desc->app_input_cluster_count + this_ep->simple_desc->app_output_cluster_count */
      );

    listLen = 0;listStr[listLen] = '\0';
    for(int s = 0; s < this_ep->simple_desc->app_input_cluster_count; s++)
      listLen += wcs_snprintf(listStr+listLen, 256-listLen, "0x%04x ", this_ep->simple_desc->app_cluster_list[s]);
    menu_printf("  servers: %s", listStr);
    listLen = 0;listStr[listLen] = '\0';
    for(int c = this_ep->simple_desc->app_input_cluster_count; c < this_ep->simple_desc->app_input_cluster_count + this_ep->simple_desc->app_output_cluster_count; c++)
      listLen += wcs_snprintf(listStr+listLen, 256-listLen, "0x%04x ", this_ep->simple_desc->app_cluster_list[c]);
    menu_printf("  clients: %s", listStr);

    for(int j=0; j <this_ep->cluster_count; j++)
    {
      struct zb_zcl_cluster_desc_s *this_cluster = &this_ep->cluster_desc_list[j];
      
      menu_printf("  cluster %u: %s%s, id 0x%04x=\'%s\', manufacturer 0x%04x, %d attributes",
        j,
        (this_cluster->role_mask & ZB_ZCL_CLUSTER_CLIENT_ROLE)?("Client "):(""),
        (this_cluster->role_mask & ZB_ZCL_CLUSTER_SERVER_ROLE)?("Server "):(""),
        this_cluster->cluster_id,
        get_cluster_id_str(this_cluster->cluster_id),
        this_cluster->manuf_code,
        this_cluster->attr_count-1 /* do not count ZB_ZCL_NULL_ID end of table */
        );
    }
  }

  return RET_OK;
}

