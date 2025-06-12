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


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Basic
 *
 * ----------------------------------------------------------------------------------- */

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_basic_attr_list = ZB_ZCL_BASIC_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &g_attr_zcl_version,
 *                                                     &g_attr_power_source);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(basic_attr_list, &g_attr_zcl_version, &g_attr_power_source)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &g_attr_zcl_version)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &g_attr_power_source)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0000[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&cluster_revision_basic_attr_list },
  { ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID,       ZB_ZCL_ATTR_TYPE_U8,        ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_basic_attr.zcl_version },
  { ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID,      ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_basic_attr.power_source },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0000 = {
  cluster_attr_0000,
  sizeof(cluster_attr_0000)/sizeof(zb_zcl_attr_t),
  zb_zcl_basic_init_server,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_basic_init_client,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  dummy_commands_handler,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Power Configuration
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Device Temperature Configuration
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Identify
 *
 * ----------------------------------------------------------------------------------- */

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_identify_attr_list = ZB_ZCL_IDENTIFY_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &g_attr_identify_time);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(identify_attr_list, ZB_ZCL_IDENTIFY)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, &g_attr_identify_time)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0003[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,  NO_MANUF_SPE, (void*)&cluster_revision_identify_attr_list },
  { ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID,  ZB_ZCL_ATTR_TYPE_U16, ACC_READ_WRITE, NO_MANUF_SPE, (void*)&g_general_identity_attr.identify_time },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0003 = {
  cluster_attr_0003,
  sizeof(cluster_attr_0003)/sizeof(zb_zcl_attr_t),
  zb_zcl_identify_init_server,                       /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_identify_init_client,                       /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  dummy_commands_handler,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Groups
 *
 * ----------------------------------------------------------------------------------- */

static zb_uint8_t groups_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_groups_attr_list = ZB_ZCL_GROUPS_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &g_attr_name_support);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(groups_attr_list, ZB_ZCL_GROUPS)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID, &g_attr_name_support)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0004[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16,     ACC_READ_ONLY,  NO_MANUF_SPE, (void*)&cluster_revision_groups_attr_list },
  { ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID,     ZB_ZCL_ATTR_TYPE_8BITMAP, ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_groups_attr.name_support },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0004 = {
  cluster_attr_0004,
  sizeof(cluster_attr_0004)/sizeof(zb_zcl_attr_t),
  zb_zcl_groups_init_server,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_groups_init_client,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  groups_commands_handler,
};


#define GET_GROUP_MEMBERSHIP_MAX_GROUP_ID 64


/* -------------------------------- Cli commands GROUPS --------------------------- */


/* Static stack callback function
 * response for groups_cmd
 */
static void groups_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("groups_cmd_cb(0x%04x) %s", cmd_send_status->dst_addr.u.short_addr, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
  menu_cb_occured();
}


static zb_ret_t cluster_groups_cmd_arg_grid(enum zb_zcl_groups_cmd_e new_command, int argc, char *argv[]);
static zb_ret_t cluster_groups_cmd_lst_grid(enum zb_zcl_groups_cmd_e new_command, int argc, char *argv[]);


/* Static command cluster
 * command groups_cmd
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_groups_cmds(void);
#endif
static zb_ret_t help_groups_cmds_detailed(char *subcommand);
static zb_ret_t cluster_groups_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "add",             ZB_ZCL_CMD_GROUPS_ADD_GROUP                },
    { "view",            ZB_ZCL_CMD_GROUPS_VIEW_GROUP               },
    { "remove",          ZB_ZCL_CMD_GROUPS_REMOVE_GROUP             },
    { "add_if_ident",    ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING },
    { "get_membership",  ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP     }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_groups_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_GROUPS_ADD_GROUP:
      case ZB_ZCL_CMD_GROUPS_VIEW_GROUP:
      case ZB_ZCL_CMD_GROUPS_REMOVE_GROUP:
      case ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING:
        return cluster_groups_cmd_arg_grid(command_id, argc-1, &argv[1]);
      case ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP:
        return cluster_groups_cmd_lst_grid(command_id, argc-1, &argv[1]);
    }
  }
  /* ... */

  /* not found, print help */
  menu_printf("cluster groups %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_groups_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_groups_cmds(void) { return help_groups_cmds_detailed(NULL); }
static zb_ret_t help_groups_cmds_detailed(char *subcommand)
{
  /* SUBMENU GROUPS Commands */
  if(!subcommand || !strcmp(subcommand, "add"))
  {
    menu_printf("cluster groups_cmd add [endpoint] [dest_addr] [dest_ep] [group_id]:");
    menu_printf("\tsend ZCL Groups Command ADD from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
  }
  if(!subcommand || !strcmp(subcommand, "view"))
  {
    menu_printf("cluster groups_cmd view [endpoint] [dest_addr] [dest_ep]  [group_id]:");
    menu_printf("\tsend ZCL Groups Command VIEW from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
  }
  if(!subcommand || !strcmp(subcommand, "get_membership"))
  {
    zb_zcl_groups_get_group_membership_req_t tmp;
    menu_printf("cluster groups_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_ids_list]:");
    menu_printf("\tsend ZCL Groups Command GET from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] list of 0~%d group_ids [0xGGGG]", 
      sizeof(tmp.group_id)/sizeof(tmp.group_id[0]));
  }
  if(!subcommand || !strcmp(subcommand, "remove"))
  {
    menu_printf("cluster groups_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id]:");
    menu_printf("\tsend ZCL Groups Command REMOVE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
    menu_printf("cluster groups_cmd remove [endpoint] [dest_addr] [dest_ep] all:");
    menu_printf("\tsend ZCL Groups Command REMOVE_ALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255]");
  }
  if(!subcommand || !strcmp(subcommand, "add_if_ident"))
  {
    menu_printf("cluster groups_cmd add_if_ident [endpoint] [dest_addr] [dest_ep] [group_id]:");
    menu_printf("\tsend ZCL Groups Command ADD_IF_IDENTIFYING from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
  }
  return RET_OK;
}


/* Static command cluster
 * command groups_cmd
 *
 * cluster groups_cmd add          [endpoint] [dest_addr] [dest_ep] [group_id]
 * cluster groups_cmd view         [endpoint] [dest_addr] [dest_ep] [group_id]
 * cluster groups_cmd remove       [endpoint] [dest_addr] [dest_ep] [group_id]
 * cluster groups_cmd remove       [endpoint] [dest_addr] [dest_ep] all
 * cluster groups_cmd add_if_ident [endpoint] [dest_addr] [dest_ep] [group_id]
 */
static zb_ret_t cluster_groups_cmd_arg_grid(enum zb_zcl_groups_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_uint16_t new_group_id;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 4)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster groups client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_GROUPS, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [group_id], can be 'all' in case of command remove */
  if(new_command == ZB_ZCL_CMD_GROUPS_REMOVE_GROUP && !strcmp(argv[3], "all"))
  {
    new_command = ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS;
    new_group_id = 0xFFFF;
  }
  else
  {
    TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3, &new_group_id);
  }

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_GROUPS_ADD_GROUP:
      /** @brief Send Add group command
          @params buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id */
      ZB_ZCL_GROUPS_SEND_ADD_GROUP_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, groups_cmd_cb, new_group_id);
      break;

    case ZB_ZCL_CMD_GROUPS_VIEW_GROUP:
      /** @brief Send view group command
          @params buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id */
      ZB_ZCL_GROUPS_SEND_VIEW_GROUP_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, groups_cmd_cb, new_group_id);
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_GROUP:
      /** @brief Send Remove group command
          @params buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id */
      ZB_ZCL_GROUPS_SEND_REMOVE_GROUP_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, groups_cmd_cb, new_group_id);
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS:
      /** @brief Send Remove all groups command
          @params buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb */
      ZB_ZCL_GROUPS_SEND_REMOVE_ALL_GROUPS_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, groups_cmd_cb);
      break;

    case ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING:
      /** @brief Send Add group if identifying command
          @params buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id */
      ZB_ZCL_GROUPS_SEND_ADD_GROUP_IF_IDENT_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, groups_cmd_cb, new_group_id);
      break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}

/* Static command cluster
 * command groups_cmd
 *
 * cluster groups_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_ids_list]
 */
static zb_ret_t cluster_groups_cmd_lst_grid(enum zb_zcl_groups_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  long unsigned int i;
  /* zb_zcl_groups_get_group_membership_req_t has just 1 group_id */
  typedef ZB_PACKED_PRE struct {
    zb_uint8_t group_count;
    zb_uint16_t group_id[GET_GROUP_MEMBERSHIP_MAX_GROUP_ID];
  } ZB_PACKED_STRUCT my_zcl_groups_get_group_membership_req_t;
  my_zcl_groups_get_group_membership_req_t req;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc < 3)
    return RET_INVALID_PARAMETER;

  /* get nb of groups in the list */
  if(argc - 3 > GET_GROUP_MEMBERSHIP_MAX_GROUP_ID)
    return RET_INVALID_PARAMETER_4;
  req.group_count = argc - 3;
#ifdef CLI_DEBUG_ARG
  WCS_TRACE_DEBUG("get argc uint8 0x%02x:%d ", req.group_count, req.group_count);
#endif

  /*  get [endpoint] & validate it has cluster groups client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_GROUPS, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [group_ids_list] */
  for(i=0; i<req.group_count ; i++)
  {
    TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3+i, req.group_id+i);
  }

  if(!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP:
      {
        zb_uint8_t *cmd_ptr;
        /** @brief Initialize Get group membership command
            @params buffer, ptr, def_resp, group_cnt */
        ZB_ZCL_GROUPS_INIT_GET_GROUP_MEMBERSHIP_REQ(buffer, cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, req.group_count);

        for(i=0; i<req.group_count ; i++) {
          /** @brief Add group id to command payload
             @params ptr, group_id */
          ZB_ZCL_GROUPS_ADD_ID_GET_GROUP_MEMBERSHIP_REQ(cmd_ptr, req.group_id[i]);
        }
        /** @brief Sends Get group membership command
            @params buffer, ptr, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb */
        ZB_ZCL_GROUPS_SEND_GET_GROUP_MEMBERSHIP_REQ(buffer, cmd_ptr, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, groups_cmd_cb);
      }
      break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}

/* -------------------------------- Handler commands GROUPS --------------------------- */


static zb_uint8_t groups_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manages it */
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_GROUPS_ADD_GROUP: /* Add GroupId & GroupName in table */
      {
        zb_zcl_groups_add_group_req_t add_group_req = {0};

        ZB_ZCL_GROUPS_GET_ADD_GROUP_REQ(param, add_group_req);
        menu_printf("%s() recv from 0x%04x endpoint %u: ADD group 0x%04x",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          add_group_req.group_id);
      }
      break;

    case ZB_ZCL_CMD_GROUPS_VIEW_GROUP: /* Generate view Group Response */
      {
        zb_zcl_groups_view_group_req_t view_group_req = {0};

        ZB_ZCL_GROUPS_GET_VIEW_GROUP_REQ(param, view_group_req);
        menu_printf("%s() recv from 0x%04x endpoint %u: VIEW group 0x%04x",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          view_group_req.group_id);
      }
      break;

    case ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP:
      {
        zb_zcl_groups_get_group_membership_req_t *get_membership_req;
        char memberListStr[8*GET_GROUP_MEMBERSHIP_MAX_GROUP_ID] = {0};
        int  memberListLen = 0;

        ZB_ZCL_GROUPS_GET_GROUP_MEMBERSHIP_REQ(param, get_membership_req);
        if(get_membership_req)
        {
          for(int i=0; i<get_membership_req->group_count; i++)
            memberListLen += wcs_snprintf(memberListStr+memberListLen, sizeof(memberListStr)-memberListLen, " 0x%04x", get_membership_req->group_id[i]);
          menu_printf("%s() recv from 0x%04x endpoint %u: GET_MEMBERSHIP count %u: %s",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            get_membership_req->group_count,
            memberListStr);
        }
        else
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, could not extract membership_req",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id));
        }
      }
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_GROUP:
      {
        zb_zcl_groups_remove_group_req_t remove_group_req = {0};

        ZB_ZCL_GROUPS_GET_REMOVE_GROUP_REQ(param, remove_group_req);
        menu_printf("%s() recv from 0x%04x endpoint %u: REMOVE group 0x%04x",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          remove_group_req.group_id);
      }
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: REMOVE ALL",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
      }
      break;

    case ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING:
      {
        /* No API to get the group_id :( */
        menu_printf("%s() recv from 0x%04x endpoint %u: ADD IF IDENTIFYING",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
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

static zb_uint8_t groups_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_GROUPS_ADD_GROUP:
      {
        zb_zcl_groups_add_group_res_t *add_group_res;

        ZB_ZCL_GROUPS_GET_ADD_GROUP_RES(param, add_group_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: ADD group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          add_group_res->group_id,
          get_zcl_status_str(add_group_res->status));
      }
      break;

    case ZB_ZCL_CMD_GROUPS_VIEW_GROUP:
      {
        zb_zcl_groups_view_group_res_t *view_group_res;

        ZB_ZCL_GROUPS_GET_VIEW_GROUP_RES(param, view_group_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: VIEW group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          view_group_res->group_id,
          get_zcl_status_str(view_group_res->status));
      }
      break;

    case ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP:
      {
        zb_zcl_groups_get_group_membership_res_t *get_membership_res;
        char memberListStr[8*GET_GROUP_MEMBERSHIP_MAX_GROUP_ID] = {0};
        int  memberListLen = 0;

        ZB_ZCL_GROUPS_GET_GROUP_MEMBERSHIP_RES(param, get_membership_res);
        if(get_membership_res)
        {
          for(int i=0; i<get_membership_res->group_count; i++)
            memberListLen += wcs_snprintf(memberListStr+memberListLen, sizeof(memberListStr)-memberListLen, " 0x%04x", get_membership_res->group_id[i]);
          menu_printf("%s() recv from 0x%04x endpoint %u: GET_MEMBERSHIP capacity %u, count %u: %s",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            get_membership_res->capacity,
            get_membership_res->group_count,
            memberListStr);
        }
        else
        {
          menu_printf("%s() recv from 0x%04x endpoint %u: %s ERROR, could not extract membership_res",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id));
        }
      }
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_GROUP:
      {
        zb_zcl_groups_remove_group_res_t *remove_group_res;

        ZB_ZCL_GROUPS_GET_REMOVE_GROUP_RES(param, remove_group_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: REMOVE group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          remove_group_res->group_id,
          get_zcl_status_str(remove_group_res->status));
      }
      break;

    case ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS:
      break;

    case ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING:
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

static zb_uint8_t groups_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(groups_client_commands_handler(cmd_info, param)):(groups_server_commands_handler(cmd_info, param));
}


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Scenes
 *
 * ----------------------------------------------------------------------------------- */

static zb_uint8_t scenes_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_scenes_attr_list = ZB_ZCL_SCENES_CLUSTER_REVISION_DEFAULT;


/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list, &g_attr_scenes_scene_count,
 *                                                       &g_attr_scenes_current_scene,
 *                                                       &g_attr_scenes_current_group,
 *                                                       &g_attr_scenes_scene_valid,
 *                                                       &g_attr_scenes_name_support);
 * to have a better view of its content
 *       ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(scenes_attr_list, ZB_ZCL_SCENES)
 *       ZZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID, &g_attr_scenes_scene_count)
 *       ZZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID, &g_attr_scenes_current_scene)
 *       ZZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID, &g_attr_scenes_current_group)
 *       ZZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID, &g_attr_scenes_scene_valid)
 *       ZZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID, &g_attr_scenes_name_support)
 *       ZZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0005[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16,     ACC_READ_ONLY, NO_MANUF_SPE, (void*)&cluster_revision_scenes_attr_list },
  { ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID,      ZB_ZCL_ATTR_TYPE_U8,      ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_scenes_attr.scene_count },
  { ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID,    ZB_ZCL_ATTR_TYPE_U8,      ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_scenes_attr.current_scene },
  { ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID,    ZB_ZCL_ATTR_TYPE_U16,     ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_scenes_attr.current_group },
  { ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID,      ZB_ZCL_ATTR_TYPE_BOOL,    ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_scenes_attr.scene_valid },
  { ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID,     ZB_ZCL_ATTR_TYPE_8BITMAP, ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_scenes_attr.name_support },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0005 = {
  cluster_attr_0005,
  sizeof(cluster_attr_0005)/sizeof(zb_zcl_attr_t),
  zb_zcl_scenes_init_server,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_scenes_init_client,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  scenes_commands_handler,
};


/* -------------------------------- Cli commands SCENES --------------------------- */


/* Static stack callback function
 * response for scenes_cmd
 */
static void scenes_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("scenes_cmd_cb(0x%04x) %s", cmd_send_status->dst_addr.u.short_addr, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
  menu_cb_occured();
}


static zb_ret_t cluster_scenes_cmd_arg_grid_scid_trtm_extfldsets(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[]);
static zb_ret_t cluster_scenes_cmd_arg_grid_scid(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[]);
static zb_ret_t cluster_scenes_cmd_arg_grid(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[]);


/* Static command cluster
 * command scenes_cmd
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_scenes_cmds(void);
#endif
static zb_ret_t help_scenes_cmds_detailed(char *subcommand);
static zb_ret_t cluster_scenes_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "add",             ZB_ZCL_CMD_SCENES_ADD_SCENE            },
    { "view",            ZB_ZCL_CMD_SCENES_VIEW_SCENE           },
    { "remove",          ZB_ZCL_CMD_SCENES_REMOVE_SCENE         },
    { "store",           ZB_ZCL_CMD_SCENES_STORE_SCENE          },
    { "recall",          ZB_ZCL_CMD_SCENES_RECALL_SCENE         },
    { "get_membership",  ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_scenes_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_SCENES_ADD_SCENE:
        return cluster_scenes_cmd_arg_grid_scid_trtm_extfldsets(command_id, argc-1, &argv[1]);
      case ZB_ZCL_CMD_SCENES_VIEW_SCENE:
      case ZB_ZCL_CMD_SCENES_REMOVE_SCENE:
      case ZB_ZCL_CMD_SCENES_STORE_SCENE:
      case ZB_ZCL_CMD_SCENES_RECALL_SCENE:
        return cluster_scenes_cmd_arg_grid_scid(command_id, argc-1, &argv[1]);
      case ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP:
        return cluster_scenes_cmd_arg_grid(command_id, argc-1, &argv[1]);
    }
  }
  /* ... */

  /* not found, print help */
  menu_printf("cluster scenes %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_scenes_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_scenes_cmds(void) { return help_scenes_cmds_detailed(NULL); }
static zb_ret_t help_scenes_cmds_detailed(char *subcommand)
{
  /* SUBMENU SCENES Commands */
  if(!subcommand || !strcmp(subcommand, "add"))
  {
    menu_printf("cluster scenes_cmd add [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id] [tr_time] { [cluster] [field_len] [field_data] }...:");
    menu_printf("\tsend ZCL Scenes Command ADD from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS] transition  [0-65535]");
    menu_printf("\tExtension field sets: one per cluster: for Cluster id [0xCCCC or initials] field len [1-255] field set [0xFF....FF]");
  }
  if(!subcommand || !strcmp(subcommand, "view"))
  {
    menu_printf("cluster scenes_cmd view [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:");
    menu_printf("\tsend ZCL Scenes Command VIEW from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]");
  }
  if(!subcommand || !strcmp(subcommand, "remove"))
  {
    menu_printf("cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:");
    menu_printf("\tsend ZCL Scenes Command REMOVE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]");
    menu_printf("cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] all:");
    menu_printf("\tsend ZCL Scenes Command REMOVE_ALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
  }
  if(!subcommand || !strcmp(subcommand, "store"))
  {
    menu_printf("cluster scenes_cmd store [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:");
    menu_printf("\tsend ZCL Scenes Command STORE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]");
  }
  if(!subcommand || !strcmp(subcommand, "recall"))
  {
    menu_printf("cluster scenes_cmd recall [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:");
    menu_printf("\tsend ZCL Scenes Command RECALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]");
  }
  if(!subcommand || !strcmp(subcommand, "get_membership"))
  {
    menu_printf("cluster scenes_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_id]:");
    menu_printf("\tsend ZCL Scenes Command GET_MEMBERSHIP from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]");
  }
  return RET_OK;
}


/* Static command cluster
 * command scenes_cmd
 *
 * cluster scenes_cmd add    [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id] [tr_time] { [cluster] [field_len] [field_data] }...
 */
static zb_ret_t cluster_scenes_cmd_arg_grid_scid_trtm_extfldsets(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_uint16_t new_group_id;
  zb_uint8_t new_scene_id;
  zb_uint16_t new_transition_time;
  long unsigned int i, ext_field_set_cnt = 0;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc < 6)
    return RET_INVALID_PARAMETER;

  /* each extension field set comes with 3 args: cluster_id, field_len, field_set */
  if((argc - 6)%3 != 0)
    return RET_INVALID_PARAMETER;

  ext_field_set_cnt = (argc - 6)/3;
#if 0 /* Do we need to have at least 1 ext_field_set? */
  if(ext_field_set_cnt == 0)
    return RET_INVALID_PARAMETER
#endif

  /*  get [endpoint] & validate it has cluster scenes client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_SCENES, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [group_id] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3, &new_group_id);

  /* get [scene_id] */
  TOOLS_GET_ARG_HEXA(ret, uint8,  argv, 4, &new_scene_id);

  /* get [tr_time] */
  TOOLS_GET_ARG(ret, uint16, argv, 5, &new_transition_time);

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_SCENES_ADD_SCENE:
      {
        zb_uint8_t *cmd_ptr;
        /*! @brief Start filling Add scene command frame
            @params buffer, cmd_struct_ptr, dis_default_resp, group_id, scene_id, transition_time */
        ZB_ZCL_SCENES_INIT_ADD_SCENE_REQ(buffer, cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE, new_group_id, new_scene_id, new_transition_time);

        /* get { [cluster] [field_len] [field_data] }... */
        for(i = 0; i < ext_field_set_cnt; i++)
        {
          zb_uint16_t new_cluster_id = 0xFFFF;
          zb_uint8_t  j, new_field_len;
          char *argv_field_set;

          /* get [cluster] */
          ret = tools_arg_get_uint16(argv, 6+3*i+0, &new_cluster_id, ARG_HEX);
          if(ret != RET_OK)
          {/* Not an hexa, look for initials */
            for(int k=0; table_clusters[k].id != 0xFFFF; k++)
              if(!strcasecmp(argv[6+3*i+0], table_clusters[k].initials))
              {
                new_cluster_id = table_clusters[k].id;
                break;
              }
          }
          if(new_cluster_id == 0xFFFF)
          {
            zb_buf_free(buffer);
            return RET_INVALID_PARAMETER_7;
          }

          /* get [field_len] */
          ret = tools_arg_get_uint8(argv, 6+3*i+1, &new_field_len, ARG_DEC);
          if(ret != RET_OK)
          {
            zb_buf_free(buffer);
            return ret;
          }

          /* get [field_data] */
          argv_field_set = argv[6+3*i+2];
          if(strstr(argv_field_set, "0x") != argv_field_set && strstr(argv_field_set, "0X") != argv_field_set)
          {
            zb_buf_free(buffer);
            return RET_INVALID_PARAMETER_9;
          }
          argv_field_set += 2;

          /** @brief Adds fieldset into the buffer for sending command
              @params cmd_struct_ptr, cluster identifier, fs_length */
          ZB_ZCL_SCENES_INIT_FIELDSET(cmd_ptr, new_cluster_id, new_field_len);
          for(j = 0; j < new_field_len; j++)
          {
            char byteStr[3] = {
              argv_field_set[j*2],
              argv_field_set[j*2+1],
              '\0'
            };
            zb_uint8_t val8;

            if(1 != sscanf(byteStr, "%hhx", &val8))
            {
              zb_buf_free(buffer);
              return RET_INVALID_PARAMETER_9;
            }

            ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, val8);
          }
        }
        /*! @brief Sends Add scene command
            @params buffer, cmd_struct_ptr, dst_addr, dst_ep, ep, prof_id, callback */
        ZB_ZCL_SCENES_SEND_ADD_SCENE_REQ(buffer, cmd_ptr, dest_short_addr, dest_ep_id, this_ep->ep_id, this_ep->profile_id, scenes_cmd_cb);
    }
    break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}

/* Static command cluster
 * command scenes_cmd
 *
 * cluster scenes_cmd view   [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]
 * cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]
 * cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] all
 * cluster scenes_cmd store  [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]
 * cluster scenes_cmd recall [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]
 */
static zb_ret_t cluster_scenes_cmd_arg_grid_scid(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_uint16_t new_group_id;
  zb_uint8_t new_scene_id;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 5)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster scenes client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_SCENES, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [group_id] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3, &new_group_id);

  /* get [scene_id], can be 'all' in case of command remove */
  if(new_command == ZB_ZCL_CMD_SCENES_REMOVE_SCENE && !strcmp(argv[4], "all"))
  {
    new_command = ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES;
    new_scene_id = 0xFF;
  }
  else
  {
    TOOLS_GET_ARG_HEXA(ret, uint8, argv, 4, &new_scene_id);
  }


  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_SCENES_VIEW_SCENE:
      /*! @brief Send View Scene commands
          @params buffer, dst_addr, dst_ep, ep, prof_id, dis_default_resp, callback, group_id, scene_id */
      ZB_ZCL_SCENES_SEND_VIEW_SCENE_REQ(buffer, dest_short_addr, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id, new_scene_id);
       break;

    case ZB_ZCL_CMD_SCENES_REMOVE_SCENE:
      /*! @brief Send Remove scene command
          @params buffer, dst_addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, callback, group_id, scene_id */
      ZB_ZCL_SCENES_SEND_REMOVE_SCENE_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id, new_scene_id);
      break;

    case ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES:
      /*! @brief Send Remove all scenes command
          @params buffer, dst_addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, callback, group_id */
      ZB_ZCL_SCENES_SEND_REMOVE_ALL_SCENES_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id);
       break;

    case ZB_ZCL_CMD_SCENES_STORE_SCENE:
      /*! @brief Send Store Scene command
          @param buffer, dst_addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, callback, group_id, scene_id */
      ZB_ZCL_SCENES_SEND_STORE_SCENE_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id, new_scene_id);
       break;

    case ZB_ZCL_CMD_SCENES_RECALL_SCENE:
      /*! @brief Send Recall scene command (pre-ZCL8)
          @param buffer, dst_addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, callback, group_id, scene_id */
      ZB_ZCL_SCENES_SEND_RECALL_SCENE_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id, new_scene_id);
      break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}

/* Static command cluster
 * command scenes_cmd
 *
 * cluster scenes_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_id]
 */
static zb_ret_t cluster_scenes_cmd_arg_grid(enum zb_zcl_scenes_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_uint16_t new_group_id;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 4)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster scenes client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_SCENES, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [group_id] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3, &new_group_id);

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP:
      /*! @brief Send Get scenes membership command
          @params buffer, dst_addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, callback, group_id */
      ZB_ZCL_SCENES_SEND_GET_SCENE_MEMBERSHIP_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id,  ZB_ZCL_ENABLE_DEFAULT_RESPONSE, scenes_cmd_cb, new_group_id);
      break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}

/* -------------------------------- Handler commands SCENES --------------------------- */


static zb_uint8_t scenes_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manages it */
  zb_uint8_t ret = ZB_FALSE;

  /* Do nothing here, it will be managed by cluster_attributes_cb(): case ZB_ZCL_SCENES_XXX_CB_ID -> cluster_scenes_srv_device_value_cb() */

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_SCENES_ADD_SCENE:
    case ZB_ZCL_CMD_SCENES_VIEW_SCENE:
    case ZB_ZCL_CMD_SCENES_REMOVE_SCENE:
    case ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES:
    case ZB_ZCL_CMD_SCENES_STORE_SCENE:
    case ZB_ZCL_CMD_SCENES_RECALL_SCENE:
    case ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP:
#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
    case ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE:
    case ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE:
    case ZB_ZCL_CMD_SCENES_COPY_SCENE:
#endif
    {
      char *cmd_name = "unknown";

      switch(cmd_info->cmd_id) {
        case ZB_ZCL_CMD_SCENES_ADD_SCENE:            cmd_name = "ADD scene";            break;
        case ZB_ZCL_CMD_SCENES_VIEW_SCENE:           cmd_name = "VIEW scene";           break;
        case ZB_ZCL_CMD_SCENES_REMOVE_SCENE:         cmd_name = "REMOVE scene";         break;
        case ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES:    cmd_name = "REMOVE ALL";           break;
        case ZB_ZCL_CMD_SCENES_STORE_SCENE:          cmd_name = "STORE scene";          break;
        case ZB_ZCL_CMD_SCENES_RECALL_SCENE:         cmd_name = "RECALL scene";         break;
        case ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP: cmd_name = "GET scene membership"; break;
#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
        case ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE:   cmd_name = "Enhanced ADD scene";   break;
        case ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE:  cmd_name = "Enhanced VIEW scene";  break;
        case ZB_ZCL_CMD_SCENES_COPY_SCENE:           cmd_name = "COPY scene";           break;
#endif
      }
      menu_printf("%s() recv from 0x%04x endpoint %u: %s",
        __FUNCTION__,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
        cmd_name);
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

static zb_uint8_t scenes_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_SCENES_ADD_SCENE:
      {
        zb_zcl_scenes_add_scene_res_t *add_scene_res;

        ZB_ZCL_SCENES_GET_ADD_SCENE_RES(param, add_scene_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: ADD scene 0x%02x group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          add_scene_res->scene_id,
          add_scene_res->group_id,
          get_zcl_status_str(add_scene_res->status));
      }
      break;

    case ZB_ZCL_CMD_SCENES_VIEW_SCENE:
      {
        zb_zcl_scenes_view_scene_res_fixed_size_t *view_scene_res;

        ZB_ZCL_SCENES_GET_VIEW_SCENE_RES_COMMON(param, view_scene_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: VIEW scene 0x%02x group 0x%04x transition time %u Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          view_scene_res->scene_id,
          view_scene_res->group_id,
          view_scene_res->transition_time,
          get_zcl_status_str(view_scene_res->status));

        /* Do the action... */
        do {
          zb_zcl_scenes_fieldset_common_t *fieldset;
          zb_uint8_t fs_content_length;
          zb_uint8_t *fs_data_ptr;

          /*  Humh, ZB_ZCL_SCENES_GET_VIEW_SCENE_REQ_NEXT_FIELDSET_DESC does not exist, this one should work... */
          ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_NEXT_FIELDSET_DESC(param, fieldset, fs_content_length);
          if(!fieldset)
            break;

          {
            char fs_data_str[256];
            int fs_data_len = 0;
            zb_cluster_entry *this_cluster = NULL;;

              /* ... real action */
            fs_data_ptr = (zb_uint8_t*)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

            /* Not required, just to get cluster name... */
            for(int i = 0; table_clusters[i].id != 0xFFFF; i++)
              if(table_clusters[i].id == fieldset->cluster_id)
              {
                this_cluster = &table_clusters[i];
                break;
              }

            for(int i = 0; i < fieldset->fieldset_length && fs_data_len < 250; i++)
              fs_data_len += wcs_snprintf(fs_data_str+fs_data_len, 256-fs_data_len, "%02x ", fs_data_ptr[i]);

            menu_printf("\tExtension field set: cluster 0x%04x:%s, len: %u, data: %s",
              fieldset->cluster_id,
              (this_cluster)?(this_cluster->name):("unknown"),
              fieldset->fieldset_length,
              fs_data_str);
          }
        } while(1);
      }
      break;

    case ZB_ZCL_CMD_SCENES_REMOVE_SCENE:
      {
        zb_zcl_scenes_remove_scene_res_t *remove_scene_res;

        ZB_ZCL_SCENES_GET_REMOVE_SCENE_RES(param, remove_scene_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: REMOVE scene 0x%02x group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          remove_scene_res->scene_id,
          remove_scene_res->group_id,
          get_zcl_status_str(remove_scene_res->status));
      }
      break;

    case ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES:
      {
        zb_zcl_scenes_remove_all_scenes_res_t *remove_all_scenes_res;

        ZB_ZCL_SCENES_GET_REMOVE_ALL_SCENES_RES(param, remove_all_scenes_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: REMOVE ALL scenes group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          remove_all_scenes_res->group_id,
          get_zcl_status_str(remove_all_scenes_res->status));
      }
      break;

    case ZB_ZCL_CMD_SCENES_STORE_SCENE:
      {
        zb_zcl_scenes_store_scene_res_t *store_scene_res;

        ZB_ZCL_SCENES_GET_STORE_SCENE_RES(param, store_scene_res);
        menu_printf("%s() recv from 0x%04x endpoint %u: STORE scene 0x%02x group 0x%04x Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          store_scene_res->scene_id,
          store_scene_res->group_id,
          get_zcl_status_str(store_scene_res->status));
      }
      break;

    case ZB_ZCL_CMD_SCENES_RECALL_SCENE:
      { /* Should not occur, goes throught default response */
        menu_printf("%s() recv from 0x%04x endpoint %u: RECALL scene Resp",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
      }
      break;

    case ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP:
      {
        zb_zcl_scenes_get_scene_membership_res_t *get_scene_membership_res;

        ZB_ZCL_SCENES_GET_GET_SCENE_MEMBERSHIP_RES(param, get_scene_membership_res);

        if(!get_scene_membership_res)
        { /* Should not occur by it happens if get_scene_membership_res->mandatory.status != ZB_ZCL_STATUS_SUCCESS  */
          menu_printf("%s() recv from 0x%04x endpoint %u: GET_SCENE_MEMBERSHIP ERROR, cannot get response",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
          break;
        }

        menu_printf("%s() recv from 0x%04x endpoint %u: GET_SCENE_MEMBERSHIP group 0x%04x capacity %u Resp %s",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          get_scene_membership_res->mandatory.group_id,
          get_scene_membership_res->mandatory.capacity,
          get_zcl_status_str(get_scene_membership_res->mandatory.status));

        if(get_scene_membership_res->mandatory.status == ZB_ZCL_STATUS_SUCCESS)
        {
          char scene_list_str[1024];
          int  scene_list_len = 0;

          for(int i = 0; i < get_scene_membership_res->optional.scene_count; i++)
            scene_list_len += wcs_snprintf(scene_list_str+scene_list_len, 1024-scene_list_len, "0x%02x ", get_scene_membership_res->optional.scene_list[i]);

          menu_printf("\tscene count: %u, list: %s",
            get_scene_membership_res->optional.scene_count,
            scene_list_str);
        }
      }
      break;

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
    case ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE:
    case ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE:
    case ZB_ZCL_CMD_SCENES_COPY_SCENE:
#endif
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

static zb_uint8_t scenes_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(scenes_client_commands_handler(cmd_info, param)):(scenes_server_commands_handler(cmd_info, param));
}


typedef struct {
    zb_uint16_t cluter_id;
    zb_uint8_t  len;
    zb_uint8_t  *data;
  } ext_field_set_t;
typedef struct {
  zb_zcl_scenes_add_scene_req_t info;
  zb_uint8_t      ext_field_set_cnt;
  ext_field_set_t ext_field_sets[64];
} cluster_scene_db;

cluster_scene_db scenes_db[256] = {0};

static zb_ret_t cluster_scenes_srv_device_value_cb(zb_zcl_device_callback_param_t *device_cb_params, zb_uint8_t param)
{
  device_cb_params->status = RET_NOT_IMPLEMENTED;

  /* Getting here we are sure the cluster has been created */
  switch(device_cb_params->device_cb_id) {
  case ZB_ZCL_SCENES_ADD_SCENE_CB_ID:
    {
      const zb_zcl_scenes_add_scene_req_t *add_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_add_scene_req_t);
      zb_bufid_t buf = param;
      zb_uint8_t *add_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      cluster_scene_db *this_scene_db = &scenes_db[add_scene_req->scene_id];

      menu_printf("%s() endpoint %u: ADD_SCENE: group 0x%04x, scene 0x%02x, transition time %u",
        __FUNCTION__,
        device_cb_params->endpoint,
        add_scene_req->group_id,
        add_scene_req->scene_id,
        add_scene_req->transition_time);

      *add_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;

      /* Do the action... */
      do {
        zb_zcl_scenes_fieldset_common_t *fieldset;
        zb_uint8_t fs_content_length;
        zb_uint8_t *fs_data_ptr;

        ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_NEXT_FIELDSET_DESC(buf, fieldset, fs_content_length);
        if(!fieldset)
          break;

        {
          char fs_data_str[256];
          int fs_data_len = 0;
          int my_cluster_attr_len = 0;
          zb_cluster_entry *this_cluster = NULL;
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[this_scene_db->ext_field_set_cnt];

          /* ... real action */
          fs_data_ptr = (zb_uint8_t*)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

          /* Not required, just to get cluster name... */
          for(int i = 0; table_clusters[i].id != 0xFFFF; i++)
            if(table_clusters[i].id == fieldset->cluster_id)
            {
              this_cluster = &table_clusters[i];
              break;
            }

          /* check len matching that cluster attr size */
          switch(fieldset->cluster_id) {
          case ZB_ZCL_CLUSTER_ID_BASIC:                my_cluster_attr_len = sizeof(g_general_basic_attr);         break;
          case ZB_ZCL_CLUSTER_ID_IDENTIFY:             my_cluster_attr_len = sizeof(g_general_identity_attr);      break;
          case ZB_ZCL_CLUSTER_ID_GROUPS:               my_cluster_attr_len = sizeof(g_general_groups_attr);        break;
          case ZB_ZCL_CLUSTER_ID_SCENES:               my_cluster_attr_len = sizeof(g_general_scenes_attr);        break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF:               my_cluster_attr_len = sizeof(g_general_on_off_attr);        break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG: my_cluster_attr_len = sizeof(g_general_on_off_switch_attr); break;
          case ZB_ZCL_CLUSTER_ID_OTA_UPGRADE:          my_cluster_attr_len = sizeof(g_general_ota_upgrade_attr);   break;
          case ZB_ZCL_CLUSTER_ID_THERMOSTAT:           my_cluster_attr_len = sizeof(g_general_thermostat_attr);   break;
//TODO : review ext_field_set_t management : not all attributes can be part of that
//          case : my_cluster_attr_len = sizeof()); break;
          }
          if(fieldset->fieldset_length > my_cluster_attr_len)
          {
            menu_printf("\tExtension field set ERROR: cluster 0x%04x:%s, len: %u is too big (max: %d)",
              fieldset->cluster_id,
              (this_cluster)?(this_cluster->name):("unknown"),
              fieldset->fieldset_length,
              my_cluster_attr_len);
            *add_scene_status = ZB_ZCL_STATUS_INVALID_VALUE;
            device_cb_params->status = RET_INVALID_PARAMETER;
            return device_cb_params->status;
          }

          /* Store this efs data */
          if(!this_efs->data)
            this_efs->data       = malloc(fieldset->fieldset_length);
          else
            this_efs->data       = realloc(this_efs->data, fieldset->fieldset_length);
          if(!this_efs->data)
          {
            menu_printf("\tExtension field set ERROR: cluster 0x%04x:%s, len: %u cannot allocate memory)",
              fieldset->cluster_id,
              (this_cluster)?(this_cluster->name):("unknown"),
              fieldset->fieldset_length);
            *add_scene_status = ZB_ZCL_STATUS_INSUFF_SPACE;
            device_cb_params->status = RET_NO_MEMORY;
            return device_cb_params->status;
          }

          this_efs->cluter_id  = fieldset->cluster_id;
          this_efs->len        = fieldset->fieldset_length;
          memcpy(this_efs->data, fs_data_ptr, fieldset->fieldset_length);

          for(int i = 0; i < fieldset->fieldset_length && fs_data_len < 250; i++)
            fs_data_len += wcs_snprintf(fs_data_str+fs_data_len, 256-fs_data_len, "%02x ", fs_data_ptr[i]);

          menu_printf("\tExtension field set: cluster 0x%04x:%s, len: %u, data: %s",
            fieldset->cluster_id,
            (this_cluster)?(this_cluster->name):("unknown"),
            fieldset->fieldset_length,
            fs_data_str);

          this_scene_db->ext_field_set_cnt++;
        }
      } while(1);

      memcpy(&this_scene_db->info, add_scene_req, sizeof(this_scene_db->info));
      *add_scene_status = ZB_ZCL_STATUS_SUCCESS;
      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_STORE_SCENE_CB_ID:
    {
      const zb_zcl_scenes_store_scene_req_t *store_scene_req    = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_store_scene_req_t);
      zb_uint8_t                            *store_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      cluster_scene_db                      *this_scene_db      = &scenes_db[store_scene_req->scene_id];
      zb_bool_t                             scene_found;

      scene_found = (this_scene_db->info.scene_id == store_scene_req->scene_id && this_scene_db->info.group_id == store_scene_req->group_id);

      menu_printf("%s() endpoint %u SCENES_STORE_SCENE_CB: group 0x%04x, scene 0x%02x",
        __FUNCTION__,
        device_cb_params->endpoint,
        store_scene_req->group_id,
        store_scene_req->scene_id);

      if (!zb_aps_is_endpoint_in_group(store_scene_req->group_id, device_cb_params->endpoint))
      {
        *store_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
        device_cb_params->status = RET_OK; //INVALID_PARAMETER;
        return device_cb_params->status;
      }

      /* Do the action... */
      if(scene_found)
      {
        /* Update existing entry with current state */
        device_cb_params->status = RET_ALREADY_EXISTS;

        for(int i = 0; i < this_scene_db->ext_field_set_cnt; i++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[i];

          switch(this_efs->cluter_id) {
          /* We should access only attributes with access & ZB_ZCL_ATTR_ACCESS_SCENE */
          case ZB_ZCL_CLUSTER_ID_BASIC:                memcpy(this_efs->data, &g_general_basic_attr,         this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_IDENTIFY:             memcpy(this_efs->data, &g_general_identity_attr,      this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_GROUPS:               memcpy(this_efs->data, &g_general_groups_attr,        this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_SCENES:               memcpy(this_efs->data, &g_general_scenes_attr,        this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF:               memcpy(this_efs->data, &g_general_on_off_attr,        this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG: memcpy(this_efs->data, &g_general_on_off_switch_attr, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_OTA_UPGRADE:          memcpy(this_efs->data, &g_general_ota_upgrade_attr,   this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_THERMOSTAT:           memcpy(this_efs->data, &g_general_thermostat_attr,    this_efs->len); break;
//TODO : review ext_field_set_t management : not all attributes can be part of that
//          case :          memcpy(this_efs->data, &,   this_efs->len); break;
          }
        }
      }
      else
      {
        device_cb_params->status = RET_OK;

        /* Create new entry with empty name and 0 transition time */
        this_scene_db->info.group_id          = store_scene_req->group_id;
        this_scene_db->info.scene_id          = store_scene_req->scene_id;
        this_scene_db->info.transition_time   = 0;
        this_scene_db->ext_field_set_cnt = 0;
      }

      *store_scene_status = ZB_ZCL_STATUS_SUCCESS;
    }
    break;

  case ZB_ZCL_SCENES_RECALL_SCENE_CB_ID:
    {
      const zb_zcl_scenes_recall_scene_req_t *recall_scene_req    = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_recall_scene_req_t);
      zb_uint8_t                             *recall_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      cluster_scene_db                       *this_scene_db      = &scenes_db[recall_scene_req->scene_id];
      zb_bool_t                              scene_found;

      scene_found = (this_scene_db->info.scene_id == recall_scene_req->scene_id && this_scene_db->info.group_id == recall_scene_req->group_id);

      menu_printf("%s() endpoint %u SCENES_RECALL_SCENE_CB: group 0x%04x, scene 0x%02x",
        __FUNCTION__,
        device_cb_params->endpoint,
        recall_scene_req->group_id,
        recall_scene_req->scene_id);

      if (!zb_aps_is_endpoint_in_group(recall_scene_req->group_id, device_cb_params->endpoint))
      {
        *recall_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
        device_cb_params->status = RET_OK; //INVALID_PARAMETER;
        return device_cb_params->status;
      }

      /* Do the action... */
      if(scene_found)
      {
        /* Recall this entry */
        *recall_scene_status = ZB_ZCL_STATUS_SUCCESS;

        for(int i = 0; i < this_scene_db->ext_field_set_cnt; i++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[i];

// TODO: should only write writable arributes...
// scenes_zc is doing ZB_ZCL_SET_ATTRIBUTE, but it only implement ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID (1 cluster, 1 attribute of 1 byte)
          /* We should access only attributes with access & ZB_ZCL_ATTR_ACCESS_SCENE */
          switch(this_efs->cluter_id) {
          case ZB_ZCL_CLUSTER_ID_BASIC:                memcpy(&g_general_basic_attr,         this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_IDENTIFY:             memcpy(&g_general_identity_attr,      this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_GROUPS:               memcpy(&g_general_groups_attr,        this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_SCENES:               memcpy(&g_general_scenes_attr,        this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF:               memcpy(&g_general_on_off_attr,        this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG: memcpy(&g_general_on_off_switch_attr, this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_OTA_UPGRADE:          memcpy(&g_general_ota_upgrade_attr,   this_efs->data, this_efs->len); break;
          case ZB_ZCL_CLUSTER_ID_THERMOSTAT:           memcpy(&g_general_thermostat_attr,    this_efs->data, this_efs->len); break;
//TODO : review ext_field_set_t management : not all attributes can be part of that
//          case :          memcpy(this_efs->data, &,   this_efs->len); break;
          }
        }
      }
      else
        *recall_scene_status = ZB_ZCL_STATUS_NOT_FOUND;

      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_VIEW_SCENE_CB_ID:
    { /* It is upto the application to send the response */
      const zb_zcl_scenes_view_scene_req_t *view_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_view_scene_req_t);
      const zb_zcl_parsed_hdr_t            *in_cmd_info    = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);
      zb_uint8_t                           view_scene_status;
      zb_bufid_t                           buf             = zb_buf_get_out();
      zb_uint8_t                           *payload_ptr;
      cluster_scene_db                     *this_scene_db  = &scenes_db[view_scene_req->scene_id];
      zb_bool_t                            scene_found;

      scene_found = (this_scene_db->info.scene_id == view_scene_req->scene_id && this_scene_db->info.group_id == view_scene_req->group_id);

      menu_printf("%s() endpoint %u SCENES_VIEW_SCENE_CB: group 0x%04x, scene 0x%02x %s",
        __FUNCTION__,
        device_cb_params->endpoint,
        view_scene_req->group_id,
        view_scene_req->scene_id,
        (scene_found)?(""):("NOT FOUND"));

      if(scene_found)
        view_scene_status = ZB_ZCL_STATUS_SUCCESS;
      else if(!zb_aps_is_endpoint_in_group(view_scene_req->group_id, device_cb_params->endpoint))
        view_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
      else
        view_scene_status = ZB_ZCL_STATUS_NOT_FOUND;

      ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES(
        buf,
        payload_ptr,
        in_cmd_info->seq_number,
        view_scene_status,
        view_scene_req->group_id,
        view_scene_req->scene_id);

      if(view_scene_status == ZB_ZCL_STATUS_SUCCESS)
      {
        ZB_ZCL_SCENES_ADD_TRANSITION_TIME_VIEW_SCENE_RES(payload_ptr, this_scene_db->info.transition_time);

        ZB_ZCL_SCENES_ADD_SCENE_NAME_VIEW_SCENE_RES(payload_ptr, this_scene_db->info.scene_name);

        for(int i = 0; i < this_scene_db->ext_field_set_cnt; i++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[i];

          ZB_ZCL_PACKET_PUT_DATA16_VAL(payload_ptr, this_efs->cluter_id);

          /* Extension set: Fieldset length */
          ZB_ZCL_PACKET_PUT_DATA8(payload_ptr, this_efs->len);

          /* Extension set: Fieldset data */
          for(int j=0; j<this_efs->len; j++)
            ZB_ZCL_PACKET_PUT_DATA8(payload_ptr, this_efs->data[j]);
        }
      }

      ZB_ZCL_SCENES_SEND_VIEW_SCENE_RES(
        buf,
        payload_ptr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).dst_endpoint,
        in_cmd_info->profile_id,
        NULL);

      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_REMOVE_SCENE_CB_ID:
    {
      const zb_zcl_scenes_remove_scene_req_t *remove_scene_req    = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_scene_req_t);
      zb_uint8_t                             *remove_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      cluster_scene_db                       *this_scene_db       = &scenes_db[remove_scene_req->scene_id];
      zb_bool_t                              scene_found;

      scene_found = (this_scene_db->info.scene_id == remove_scene_req->scene_id && this_scene_db->info.group_id == remove_scene_req->group_id);

      menu_printf("%s() endpoint %u SCENES_REMOVE_SCENE_CB: group 0x%04x, scene 0x%02x %s",
        __FUNCTION__,
        device_cb_params->endpoint,
        remove_scene_req->group_id,
        remove_scene_req->scene_id,
        (scene_found)?(""):("NOT FOUND"));

      if(scene_found)
      {
        /* Remove this entry */
        *remove_scene_status = ZB_ZCL_STATUS_SUCCESS;

        for(int i = 0; i < this_scene_db->ext_field_set_cnt; i++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[i];

          free(this_efs->data);
        }
        memset(this_scene_db, 0, sizeof(*this_scene_db));
      }
      else if(!zb_aps_is_endpoint_in_group(remove_scene_req->group_id, device_cb_params->endpoint))
        *remove_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
      else
        *remove_scene_status = ZB_ZCL_STATUS_NOT_FOUND;

      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_REMOVE_ALL_SCENES_CB_ID:
    {
      const zb_zcl_scenes_remove_all_scenes_req_t *remove_all_scenes_req    = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_all_scenes_req_t);
      zb_uint8_t                                  *remove_all_scenes_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      int                                          scenes_removed = 0;

      if (!zb_aps_is_endpoint_in_group(remove_all_scenes_req->group_id, device_cb_params->endpoint))
      {
        scenes_removed = -1;
        *remove_all_scenes_status = ZB_ZCL_STATUS_INVALID_FIELD;
      }
      else
      {
        *remove_all_scenes_status = ZB_ZCL_STATUS_SUCCESS;

        for(int i = 0; i < 256; i++)
        {
          cluster_scene_db *this_scene_db = &scenes_db[i];

          if(this_scene_db->info.group_id != remove_all_scenes_req->group_id)
            continue;

          scenes_removed++;
          for(int j = 0; j < this_scene_db->ext_field_set_cnt; j++)
          {
            ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[j];

            free(this_efs->data);
          }
          memset(this_scene_db, 0, sizeof(*this_scene_db));
        }
      }

      menu_printf("%s() endpoint %u SCENES_REMOVE_ALL_SCENE_CB: group 0x%04x, %s %d scenes removed",
        __FUNCTION__,
        device_cb_params->endpoint,
        remove_all_scenes_req->group_id,
        (*remove_all_scenes_status == ZB_ZCL_STATUS_INVALID_FIELD)?("INVALID_FIELD"):(""),
        scenes_removed);

      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_GET_SCENE_MEMBERSHIP_CB_ID:
    { /* It is upto the application to send the response */
      const zb_zcl_scenes_get_scene_membership_req_t *get_scene_membership_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_get_scene_membership_req_t);
      const zb_zcl_parsed_hdr_t                      *in_cmd_info              = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);
      zb_bufid_t                                      buf                      = zb_buf_get_out();
      zb_uint8_t                                     *payload_ptr;
      zb_uint8_t                                     *capacity_ptr;
      zb_uint8_t                                     *scene_count_ptr;
      zb_bool_t                                      invalid_field;

      invalid_field = !zb_aps_is_endpoint_in_group(get_scene_membership_req->group_id, device_cb_params->endpoint);

      ZB_ZCL_SCENES_INIT_GET_SCENE_MEMBERSHIP_RES(
        buf,
        payload_ptr,
        in_cmd_info->seq_number,
        capacity_ptr,
        (invalid_field)?(ZB_ZCL_STATUS_INVALID_FIELD):(ZB_ZCL_STATUS_SUCCESS),
        (invalid_field)?(ZB_ZCL_SCENES_CAPACITY_UNKNOWN):(0),
        get_scene_membership_req->group_id);

      scene_count_ptr = payload_ptr;
      if(!invalid_field)
      {
        ZB_ZCL_SCENES_ADD_SCENE_COUNT_GET_SCENE_MEMBERSHIP_RES(payload_ptr, 0);

        for(int i = 0; i < 256; i++)
        {
          cluster_scene_db *this_scene_db = &scenes_db[i];

          if(this_scene_db->info.group_id == 0 && this_scene_db->info.scene_id == 0)
            ++(*capacity_ptr);

          if(this_scene_db->info.group_id != get_scene_membership_req->group_id)
            continue;

          /* Add to payload */
          ++(*scene_count_ptr);
          ZB_ZCL_SCENES_ADD_SCENE_ID_GET_SCENE_MEMBERSHIP_RES(payload_ptr, this_scene_db->info.scene_id);
        }
      }

      menu_printf("%s() endpoint %u SCENES_GET_SCENE_MEMBERSHIP_CB: group 0x%04x %s %d scenes, %d capacity",
        __FUNCTION__,
        device_cb_params->endpoint,
        get_scene_membership_req->group_id,
        (invalid_field)?("INVALID FIELD"):(""),
        (*scene_count_ptr),
        (*capacity_ptr));

      ZB_ZCL_SCENES_SEND_GET_SCENE_MEMBERSHIP_RES(
        buf,
        payload_ptr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).dst_endpoint,
        in_cmd_info->profile_id,
        NULL);

      device_cb_params->status = RET_OK;
    }
    break;

  case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_CB_ID:
    {
      const zb_zcl_scenes_remove_all_scenes_req_t *remove_all_scenes_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_all_scenes_req_t);
      int                                          scenes_removed = 0;

      for(int i = 0; i < 256; i++)
      {
        cluster_scene_db *this_scene_db = &scenes_db[i];

        if(this_scene_db->info.group_id != remove_all_scenes_req->group_id)
          continue;

        scenes_removed++;
        for(int j = 0; j < this_scene_db->ext_field_set_cnt; j++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[j];

          free(this_efs->data);
        }
        memset(this_scene_db, 0, sizeof(*this_scene_db));
      }
      menu_printf("%s() endpoint %u SCENES_INTERNAL_REMOVE_ALL_SCENE_CB: group 0x%04x, %d scenes removed",
        __FUNCTION__,
        device_cb_params->endpoint,
        remove_all_scenes_req->group_id,
        scenes_removed);
    }
    break;

  case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_ALL_GROUPS_CB_ID:
    {
      const cluster_scene_db empty_scene_db = {0};
      int scenes_removed = 0;

      for(int i = 0; i < 256; i++)
      {
        cluster_scene_db *this_scene_db = &scenes_db[i];

        if(!memcmp(this_scene_db, &empty_scene_db, sizeof(cluster_scene_db)))
          continue;

        scenes_removed++;
        for(int j = 0; j < this_scene_db->ext_field_set_cnt; j++)
        {
          ext_field_set_t *this_efs = &this_scene_db->ext_field_sets[j];

          free(this_efs->data);
        }
        memset(this_scene_db, 0, sizeof(*this_scene_db));
      }
      menu_printf("%s() endpoint %u SCENES_INTERNAL_REMOVE_ALL_SCENE_ALL_GROUPS_CB: %d scenes removed",
        __FUNCTION__,
        device_cb_params->endpoint,
        scenes_removed);
    }
    break;

  default:
    break;
  }

  return device_cb_params->status;
}


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF
 *
 * ----------------------------------------------------------------------------------- */

static zb_uint8_t onoff_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_on_off_attr_list = ZB_ZCL_ON_OFF_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list, &g_attr_on_off);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(on_off_attr_list, ZB_ZCL_ON_OFF)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, &g_attr_on_off)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0006[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16,  ACC_READ_ONLY,                             NO_MANUF_SPE, (void*)&cluster_revision_on_off_attr_list },
  { ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,           ZB_ZCL_ATTR_TYPE_BOOL, ACC_READ_ONLY | ACC_REPORTING | ACC_SCENE, NO_MANUF_SPE, (void*)&g_general_on_off_attr.on_off },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                     0,                                         NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0006 = {
  cluster_attr_0006,
  sizeof(cluster_attr_0006)/sizeof(zb_zcl_attr_t),
  zb_zcl_on_off_init_server,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_on_off_init_client,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  onoff_commands_handler,
};


/* -------------------------------- Cli commands ON/OFF --------------------------- */


/* Static stack callback function
 * response for onoff_cmd
 */
static void onoff_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);;

  menu_printf("onoff_cmd_cb(0x%04x) %s", cmd_send_status->dst_addr.u.short_addr, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
  menu_cb_occured();
}


/* since the 3 commands have the same params, use common function */
static zb_ret_t cluster_onoff_cmd(enum zb_zcl_on_off_cmd_e new_command, int argc, char *argv[]);


/* Static command cluster
 * command onoff_cmd
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_onoff_cmds(void);
#endif
static zb_ret_t help_onoff_cmds_detailed(char *subcommand);
static zb_ret_t cluster_onoff_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "on",     ZB_ZCL_CMD_ON_OFF_ON_ID     },
    { "off",    ZB_ZCL_CMD_ON_OFF_OFF_ID    },
    { "toggle", ZB_ZCL_CMD_ON_OFF_TOGGLE_ID }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_onoff_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
    return cluster_onoff_cmd(command_id, argc-1, &argv[1]);
  /* ... */

  /* not found, print help */
  menu_printf("cluster on/off %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_onoff_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_onoff_cmds(void) { return help_onoff_cmds_detailed(NULL); }
static zb_ret_t help_onoff_cmds_detailed(char *subcommand)
{
  /* SUBMENU ON/OFF Commands */
  if(!subcommand || !strcmp(subcommand, "on"))
  {
    menu_printf("cluster onoff_cmd on [endpoint] [dest_addr] <dest_ep>:");
    menu_printf("\tsend ZCL OnOff Command ON from endpoint [0-255] to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group");
  }
  if(!subcommand || !strcmp(subcommand, "off"))
  {
    menu_printf("cluster onoff_cmd off [endpoint] [dest_addr] <dest_ep>:");
    menu_printf("\tsend ZCL OnOff Command OFF from endpoint [0-255] to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group");
  }
  if(!subcommand || !strcmp(subcommand, "toggle"))
  {
    menu_printf("cluster onoff_cmd toggle [endpoint] [dest_addr] <dest_ep>:");
    menu_printf("\tsend ZCL OnOff Command TOGGLE from endpoint [0-255] to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group");
  }
  return RET_OK;
}



/* Static command cluster
 * command onoff_cmd
 *
 * cluster onoff_cmd on     [endpoint] [dest_addr] <dest_ep>
 * cluster onoff_cmd off    [endpoint] [dest_addr] <dest_ep>
 * cluster onoff_cmd toggle [endpoint] [dest_addr] <dest_ep>
 */
static zb_ret_t cluster_onoff_cmd(enum zb_zcl_on_off_cmd_e new_command, int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_bool_t dest_group = ZB_TRUE;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 3 && argc != 2)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster on_off client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_ON_OFF, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get <dest_ep>: optional arg, to send command to a specific device endpoint, else a group command */
  if (argc == 3)
  {
    TOOLS_GET_ARG(ret, uint8, argv, 2, &dest_ep_id);
    dest_group = ZB_FALSE;
  }

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;

    buffer = zb_buf_get_out();

    switch(new_command) {
    case ZB_ZCL_CMD_ON_OFF_ON_ID:
    case ZB_ZCL_CMD_ON_OFF_OFF_ID:
    case ZB_ZCL_CMD_ON_OFF_TOGGLE_ID:
      {
        /*! @brief General macro for sending On/Off cluster command
            @params buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, command_id, cb */
        if (dest_group)
        {
          ZB_ZCL_ON_OFF_SEND_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT, 0, this_ep->ep_id, this_ep->profile_id, ZB_FALSE, new_command, onoff_cmd_cb);
        }
        else
        {
          ZB_ZCL_ON_OFF_SEND_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_FALSE, new_command, onoff_cmd_cb);
          dest_group = ZB_TRUE;
        }
      }
      break;

    default:
      /* should not occur */
      return RET_ILLEGAL_REQUEST;
    }
  }

  return RET_OK;
}


/* -------------------------------- Handler commands ON/OFF --------------------------- */


static zb_uint8_t onoff_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manages it */
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_ON_OFF_ON_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: ON",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
      }
      break;

    case ZB_ZCL_CMD_ON_OFF_OFF_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: OFF",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
      }
      break;

    case ZB_ZCL_CMD_ON_OFF_TOGGLE_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: TOGGLE",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint);
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

static zb_uint8_t onoff_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_ON_OFF_OFF_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: OFF Resp, payload len %d",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          zb_buf_len(param));
      }
      break;

    case ZB_ZCL_CMD_ON_OFF_ON_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: ON Resp, payload len %d",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          zb_buf_len(param));
      }
      break;

    case ZB_ZCL_CMD_ON_OFF_TOGGLE_ID:
      {
        menu_printf("%s() recv from 0x%04x endpoint %u: TOGGLE Resp, payload len %d",
          __FUNCTION__,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
          ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
          zb_buf_len(param));
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

static zb_uint8_t onoff_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(onoff_client_commands_handler(cmd_info, param)):(onoff_server_commands_handler(cmd_info, param));
}


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF Switch Configuration
 *
 * ----------------------------------------------------------------------------------- */

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_switch_cfg_attr_list = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_ON_OFF_SWITCH_CONFIGURATION_ATTRIB_LIST(switch_cfg_attr_list, &attr_switch_type,
 *                                                                                &attr_switch_actions);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(switch_cfg_attr_list, ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_ID, &attr_switch_type)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_ID, &attr_switch_actions)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0007[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                    ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY,  NO_MANUF_SPE, (void*)&cluster_revision_switch_cfg_attr_list },
  { ZB_ZCL_ATTR_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_ID,    ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ACC_READ_ONLY,  NO_MANUF_SPE, (void*)&g_general_on_off_switch_attr.type },
  { ZB_ZCL_ATTR_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_ID, ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ACC_READ_WRITE, NO_MANUF_SPE, (void*)&g_general_on_off_switch_attr.actions },
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                     0,                                         NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0007 = {
  cluster_attr_0007,
  sizeof(cluster_attr_0007)/sizeof(zb_zcl_attr_t),
  zb_zcl_on_off_switch_config_init_server,           /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_on_off_switch_config_init_client,           /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  dummy_commands_handler,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Level Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Alarms
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Time
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER RSSI Location
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analog Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analog Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analogue Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Diagnostics
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Poll Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Power Profile
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Meter Identification
 *
 * ----------------------------------------------------------------------------------- */


