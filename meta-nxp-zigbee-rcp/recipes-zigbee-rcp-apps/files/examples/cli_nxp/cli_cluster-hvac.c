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
 *                                  CLUSTER Pump Configuration and Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Thermostat
 *
 * ----------------------------------------------------------------------------------- */

static zb_uint8_t thermostat_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_thermostat_attr_list = ZB_ZCL_THERMOSTAT_CLUSTER_REVISION_DEFAULT;


static zb_zcl_attr_t cluster_attr_0201[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                   ZB_ZCL_ATTR_TYPE_U16,        ACC_READ_ONLY,                  NO_MANUF_SPE, (void*)&cluster_revision_thermostat_attr_list },
  { ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,              ZB_ZCL_ATTR_TYPE_S16,        ACC_READ_ONLY | ACC_REPORTING,  NO_MANUF_SPE, (void*)&g_general_thermostat_attr.local_temperature },
  { ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,      ZB_ZCL_ATTR_TYPE_S16,        ACC_READ_WRITE | ACC_SCENE,     NO_MANUF_SPE, (void*)&g_general_thermostat_attr.occupied_heating_setpoint },
  { ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID,      ZB_ZCL_ATTR_TYPE_S16,        ACC_READ_WRITE | ACC_SCENE,     NO_MANUF_SPE, (void*)&g_general_thermostat_attr.occupied_cooling_setpoint },
  { ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID,  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,  ACC_READ_WRITE,                 NO_MANUF_SPE, (void*)&g_general_thermostat_attr.control_seq_of_operation },
  { ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID,                    ZB_ZCL_ATTR_TYPE_8BIT_ENUM,  ACC_READ_WRITE | ACC_SCENE,     NO_MANUF_SPE, (void*)&g_general_thermostat_attr.system_mode },

  /* Optional attributes */
/* End of table */
  { ZB_ZCL_NULL_ID,                         0,                     0,                                         NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0201 = {
  cluster_attr_0201,
  sizeof(cluster_attr_0201)/sizeof(zb_zcl_attr_t),
  zb_zcl_thermostat_init_server,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_thermostat_init_client,                         /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  thermostat_commands_handler,
};


/* -------------------------------- Cli commands Thermostat --------------------------- */


/* Static stack callback function
 * response for thermostat_cmd
 */
static void thermostat_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);;

  menu_printf("thermostat_cmd_cb() %s", wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}

/* since raise/lower command has 3 modes, dedicate its own function*/
static zb_ret_t cluster_thermostat_cmd_raise_lower(int argc, char *argv[]);


/* Static command cluster
 * command thermostat_server
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_thermo_cmds(void);
#endif
static zb_ret_t help_thermo_cmds_detailed(char *subcommand);
static zb_ret_t cluster_thermo_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "setpoint_raise_lower", ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_thermo_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER:
        return cluster_thermostat_cmd_raise_lower(argc-1, &argv[1]);
    }
  }
  /* ... */

  /* not found, print help */
  menu_printf("cluster thermostat %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_thermo_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_thermo_cmds(void) { return help_thermo_cmds_detailed(NULL); }
static zb_ret_t help_thermo_cmds_detailed(char *subcommand)
{
  /* SUBMENU THERMOSTAT Commands */
  if(!subcommand || !strcmp(subcommand, "setpoint_raise_lower"))
  {
    menu_printf("cluster thermostat_cmd setpoint_raise_lower [endpoint] [dest_addr] [dest_ep] [mode] [amount]:");
    menu_printf("\tsend ZCL Command SETPOINT_RAISE_LOWER from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] mode [heat|cool|both] with the amount the setpoint(s) are to be increased (or decreased) by, in steps of 0.1 degrees celsius [(-128)-127]");
  }
  return RET_OK;
}

/* Static command cluster
 * command thermostat_cmd
 *
 * cluster thermostat_cmd setpoint_raise_lower [endpoint] [dest_addr] [dest_ep] [mode] [amount]
 */
static zb_ret_t cluster_thermostat_cmd_raise_lower(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_uint8_t mode;
  zb_int8_t amount;
  cli_tools_strval mode_table[] = {
    {"heat", ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_HEAT },
    {"cool", ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_COOL },
    {"both", ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_BOTH }
  };

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 5)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster thermostat client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_THERMOSTAT, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [mode] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 3, mode_table, &mode);

  /* get [amount] */
  TOOLS_GET_ARG(ret, int8,  argv, 4, &amount);

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    buffer = zb_buf_get_out();

    /*! @brief General macro for sending Setpoint Raise/Lower cluster command
        @params buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, command_id, cb, mode, amount */
    ZB_ZCL_THERMOSTAT_SEND_SETPOINT_RAISE_LOWER_REQ(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, ZB_FALSE, thermostat_cmd_cb, mode, amount);

  }
  return RET_OK;
}

/* -------------------------------- Handler commands Thermostat --------------------------- */


static zb_uint8_t thermostat_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manage it */
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER:
      {
        zb_zcl_thermostat_setpoint_raise_lower_req_t setpoint_raise_lower_req = {0};
        zb_zcl_parse_status_t status;

        ZB_ZCL_THERMOSTAT_GET_SETPOINT_RAISE_LOWER_REQ(param, setpoint_raise_lower_req, status);

        if(status == ZB_ZCL_PARSE_STATUS_SUCCESS)
          menu_printf("%s() recv from 0x%04x endpoint %u: Setpoint Raise/Lower mode: %u, amount: %d",
            __FUNCTION__,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
            ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
            setpoint_raise_lower_req.mode,
            setpoint_raise_lower_req.amount);
       else
          menu_printf( "\tERROR: cannot extract setpoint_raise_lower_req");

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

static zb_uint8_t thermostat_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {

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

static zb_uint8_t thermostat_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(thermostat_client_commands_handler(cmd_info, param)):(thermostat_server_commands_handler(cmd_info, param));
}

/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Fan Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Dehumidification Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Thermostat User Interface Config
 *
 * ----------------------------------------------------------------------------------- */


