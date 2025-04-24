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

#define ZB_TRACE_FILE_ID 33618
#include <ctype.h>
#include "zboss_api.h"
#include "cli_network.h"
#include "cli_menu.h"
#include "cli_config.h"
#include "cli_tools.h"


/* Commands network handlers */
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
static zb_ret_t network_form(int argc, char *argv[]);
static zb_ret_t network_open(int argc, char *argv[]);
static zb_ret_t network_close(int argc, char *argv[]);
#endif
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
static zb_ret_t network_join(int argc, char *argv[]);
static zb_ret_t network_leave(int argc, char *argv[]);
#endif
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
static zb_ret_t get_source_route(int argc, char *argv[]);
#endif

/* Menu network */
cli_menu_cmd menu_network[] = {
  /* name, args,               align, function,      help,       description */
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  { "form", " [mode]", "             ", network_form,  help_empty, "form network mode: [centralized|distributed]" },
  { "open", " [time]", "             ", network_open,  help_empty, "open network time: [0-255] seconds" },
  { "close", "", "                   ", network_close, help_empty, "close network " },
#endif
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
  { "join", " [mode]", "             ", network_join,  help_empty, "join network mode: [centralized|distributed]" },
  { "leave", " [rejoin]", "          ", network_leave, help_empty, "leave network, rejoin: [0-1] " },
#endif
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  { "get_source_route", " [addr]", " ", get_source_route,  help_empty, "get source route to a specific device at addr [0xAAAA] " },
#endif
  /* Add new commands above here */
  { NULL, NULL,                NULL, NULL,          NULL,       NULL }
};


#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
/* Static command network
 * command form
 *
 * network form [mode]
 */
static zb_ret_t network_form(int argc, char *argv[])
{
  zb_uint8_t distributed = ZB_FALSE;
  zb_bool_t ret;
  cli_tools_strval mode_table[] = {
    { "distributed", ZB_TRUE  },
    { "centralized", ZB_FALSE }
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* get [mode] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, mode_table, &distributed);

  if(ZB_JOINED())
    return RET_ALREADY_EXISTS;

  if(!(config_is_coordinator()) &&
     !(config_is_router() && distributed))
    return RET_UNAUTHORIZED;

  if(distributed)
    zb_zdo_setup_network_as_distributed();
  else
    zb_zdo_set_aps_unsecure_join(ZB_FALSE);

  ret = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION);

  return (ret)?(RET_OK):(RET_PENDING);
}

/* Static stack callback function
 * response for zb_zdo_mgmt_permit_joining_req
 */
static void network_open_cb(zb_uint8_t param)
{
  menu_printf("network opened");
  zb_buf_free(param);
}


/* Static command network
 * command open
 *
 * network open [time]
 */
static zb_ret_t network_open(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t duration;
  zb_bufid_t param;
  zb_zdo_mgmt_permit_joining_req_param_t *request;
  zb_uint8_t tsnRet;
  static zb_bool_t doOnce = ZB_TRUE;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* get [time] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &duration);

  if(doOnce)
  {
    doOnce = ZB_FALSE;
    bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
    /* Wait for signal ZB_BDB_SIGNAL_STEERING which is preceeded by a PERMIT_JOIN_STATUS */
    while(!config_is_steering_received())
      usleep(100);
    if(config_get_permit_join_duration() == duration)
      return RET_OK;
  }

  /* Same comment as cli_dsr: Device is not fully joined, so network opening is not possible */
  if(!ZB_JOINED())
    return RET_INVALID_STATE;

  param = zb_buf_get_out();
  request = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_permit_joining_req_param_t);
//  request->dest_addr = ZB_NWK_BROADCAST_ALL_DEVICES; /* 0xFFFF */
//  request->dest_addr = ZB_NWK_BROADCAST_ROUTER_COORDINATOR; /* 0xFFFC */
  request->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
  request->permit_duration = duration;
  request->tc_significance = 1;
  tsnRet = zb_zdo_mgmt_permit_joining_req(param, network_open_cb);
  if(tsnRet == ZB_ZDO_INVALID_TSN)
  {
    zb_buf_free(param);
    return RET_OPERATION_FAILED;
  }

  /* Could block until network_open_cb() is called... */
  WCS_TRACE_DEBUG("network open %d s, dest %04x", duration, request->dest_addr);

  return RET_OK;
}


/* Static command network
 * command close
 *
 * network close
 */
static zb_ret_t network_close(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* Same comment as cli_dsr: Device is not fully joined, so network opening is not possible */
  if(!ZB_JOINED())
    return RET_INVALID_STATE;

  return zb_bdb_close_network(ZB_UNDEFINED_BUFFER);
}
#endif /* defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE) */


#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
/* Static command network
 * command join
 *
 * network join [mode]
 */
static zb_ret_t network_join(int argc, char *argv[])
{
  zb_uint8_t distributed = ZB_FALSE;
  zb_bool_t ret;
  cli_tools_strval mode_table[] = {
    { "distributed", ZB_TRUE },
    { "centralized", ZB_FALSE }
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* get [mode] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, mode_table, &distributed);

  return network_do_join(distributed);
}

zb_ret_t network_do_join(zb_bool_t distributed)
{
  zb_bool_t ret;

  if(!zb_bdb_is_factory_new())
    return RET_INVALID_STATE;

#ifdef ZB_DISTRIBUTED_SECURITY_ON
  if(distributed)
  {
#if defined(ZB_ROUTER_ROLE) /* No looking for end device */
    zb_enable_distributed();
#endif
  }
  else
  {
    zb_disable_distributed();
  }
#else /* ZB_DISTRIBUTED_SECURITY_ON */
  return RET_NOT_IMPLEMENTED;
#endif /* ZB_DISTRIBUTED_SECURITY_ON */

  ret = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);

  return (ret)?(RET_OK):(RET_PENDING);
}


/* Static stack callback function
 * response for zdo_mgmt_leave_req
 */
static void network_leave_cb(zb_uint8_t param)
{
  menu_printf("left network");
  zb_buf_free(param);
}


/* Static command network
 * command leave
 *
 * network leave [rejoin]
 */
static zb_ret_t network_leave(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_bufid_t param;
  zb_uint8_t do_rejoin;
  zb_zdo_mgmt_leave_param_t *req;
  zb_uint8_t tsnRet;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  /* get [rejoin] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &do_rejoin);
  if(do_rejoin > 1)
    return RET_INVALID_PARAMETER_1;

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;

  param = zb_buf_get_out();

  req = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_leave_param_t);

  zb_get_long_address(req->device_address);

  req->dst_addr = zb_get_short_address();
  req->rejoin = (do_rejoin)?(1):(0);
  req->remove_children = ZB_B2U(ZB_FALSE);

  tsnRet = zdo_mgmt_leave_req(param, network_leave_cb);
  if(tsnRet == ZB_ZDO_INVALID_TSN)
  {
    zb_buf_free(param);
    return RET_OPERATION_FAILED;
  }

  /* Could block until network_open_cb() is called... */

  return RET_OK;
}
#endif /* defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE) */


#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
/* Static command network
 * command get_source_route
 *
 * network get_source_route [addr]
 */
static zb_ret_t get_source_route(int argc, char *argv[])
{
  zb_uint16_t dest_short_addr;
  zb_route_record_t p_route_record = {0};
  zb_ret_t ret = RET_ERROR;
  zb_uindex_t i;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &dest_short_addr);

  if(!config_is_concentrator())
    return RET_UNAUTHORIZED;

  /*Do it*/
  {
    /* Another example usage of this API can be found in tests/system/concentrator_source_routing */
    ret = zb_get_source_route(&p_route_record, dest_short_addr);

    if ( p_route_record.count != 0 && p_route_record.addr == dest_short_addr)
    {
      menu_printf("Found NWK source route entry!");
      menu_printf("Count hops: %02d",  p_route_record.count);
      for (i = 0 ; i < p_route_record.count ; i++)
      {
        menu_printf("Node added: 0x%x",p_route_record.path[i]);
      }
    }
  }
  return ret;
}
#endif

