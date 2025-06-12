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

#define ZB_TRACE_FILE_ID 33621
#include <ctype.h>
#include "zboss_api.h"
#include "cli_menu.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"
#include "cli_tools.h"
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
#include "cli_config.h" /* for config_get_panid2join() */
#include "cli_network.h" /* for network_do_join() */
#endif

#ifdef ZBOSS_ZDO_APP_TSN_ENABLE
static zb_uint8_t aps_tsn_value = 0x00;
#endif

/* Commands zdo handlers */
static zb_ret_t zdo_get_short_addr(int argc, char *argv[]);
static zb_ret_t zdo_get_ieee_addr(int argc, char *argv[]);
static zb_ret_t zdo_get_active(int argc, char *argv[]);
static zb_ret_t zdo_get_bind_table(int argc, char *argv[]);
static zb_ret_t zdo_get_node_desc(int argc, char *argv[]);
static zb_ret_t zdo_get_power_desc(int argc, char *argv[]);
static zb_ret_t zdo_get_simp_desc(int argc, char *argv[]);
static zb_ret_t zdo_get_match_desc(int argc, char *argv[]);      static zb_ret_t help_get_match_desc(void);
static zb_ret_t zdo_get_route(int argc, char *argv[]);
static zb_ret_t zdo_get_lqi(int argc, char *argv[]);
static zb_ret_t zdo_active_scan(int argc, char *argv[]);         static zb_ret_t help_active_scan(void);
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
static zb_ret_t zdo_energy_scan(int argc, char *argv[]);         static zb_ret_t help_energy_scan(void);
#endif
static zb_ret_t zdo_raw_cmd(int argc, char *argv[]);             static zb_ret_t help_raw_cmd(void);
static zb_ret_t zdo_data_ind(int argc, char *argv[]);
#ifdef ZBOSS_ZDO_APP_TSN_ENABLE
static zb_ret_t zdo_tsn_policy(int argc, char *argv[]);
static zb_ret_t zdo_aps_cmd(int argc, char *argv[]);             static zb_ret_t help_aps_cmd(void);
#endif

/* Menu endpoint */
cli_menu_cmd menu_zdo[] = {
  /* name, args,                             align,      function,             help,        description */
  { "get_short_addr", " [ieee]", "                                 ", zdo_get_short_addr, help_empty,          "\r\n\tget from local ieee_addr_table the specific device at addr [AA:AA:AA:AA:AA:AA:AA:AA]" },
  { "get_ieee_addr", " [addr]", "                                  ", zdo_get_ieee_addr,  help_empty,          "\r\n\tsend ZDO IEEE Address Req to a specific device at addr [0xAAAA]" },
  { "get_active", " [addr]", "                                     ", zdo_get_active,     help_empty,          "\r\n\tsend ZDO Active Endpoint Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]" },
  { "get_bind_table", " [addr] [index]", "                         ", zdo_get_bind_table, help_empty,          "\r\n\tsend ZDO Mgmt Bind Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] from index [0~255]" },
  { "get_node_desc", " [addr]", "                                  ", zdo_get_node_desc,  help_empty,          "\r\n\tsend ZDO Node Descriptor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]" },
  { "get_power_desc", " [addr]", "                                 ", zdo_get_power_desc, help_empty,          "\r\n\tsend ZDO Power Descriptor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]" },
  { "get_simple_desc", " [addr] [endpoint]", "                     ", zdo_get_simp_desc,  help_empty,          "\r\n\tsend ZDO Simple Descriptor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] endpoint [0-255]" },
  { "get_match_desc", " [addr] [profile] [#in:#out] [clusters]", " ", zdo_get_match_desc, help_get_match_desc, "\r\n\tsend ZDO Match Descriptor Request to a specific device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] profile [0xPPPP|initials] numbers of input:output clusters [0~255:0~255], at least one, list of clusters [0xCCC1] ... [0xCCCn] (n:#in + #out)" },
  { "get_route", " [addr] [idx]", "                                ", zdo_get_route,      help_empty,          "\r\n\tsend ZDO Route Table Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] starting at index [0-255]" },
  { "get_lqi", " [addr] [idx]", "                                  ", zdo_get_lqi,        help_empty,          "\r\n\tsend ZDO Link Quality Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] starting at index [0-255]" },
  { "active_scan", " [duration] [channel_mask] <panid2join>", "    ", zdo_active_scan,    help_active_scan,    "\r\n\tsend ZDO Active Scan Req for duration [0-14] on channel mask [0xMMMMMMMM], optional panid to join <0xPPPP>" },
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  { "energy_scan", " [duration] [channel_mask]", "                 ", zdo_energy_scan,    help_energy_scan,    "\r\n\tsend ZDO Energy Scan Req for duration [0-14] on channel mask [0xMMMMMMMM]" },
#endif
  { "raw_cmd", " [addr] [cmd] <payload>", "                        ", zdo_raw_cmd,        help_raw_cmd,        "\r\n\tsend ZDO command to a specific device at dest_addr short or ieee [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] command [0xCCCC] optional payload <P1:P2....Pn>" },
  { "data_ind", " [enable]", "                                     ", zdo_data_ind,       help_empty,          "\r\n\tregister ZDO Data Indication callback if enabled [0|1]" },
#ifdef ZBOSS_ZDO_APP_TSN_ENABLE
  { "tsn_policy", " [tsn_policy] [start_tsn]", "                   ", zdo_tsn_policy,     help_empty,          "\r\n\tconfigure ZDO tsn_policy [0: full range (default)|1: range 128_254], start tsn value (if enabled) [0-127]" },
  { "aps_cmd", " [addr] [cmd] <payload>", "                        ", zdo_aps_cmd,        help_aps_cmd,        "\r\n\tsend ZDO APS command to a specific device at dest_addr short or ieee [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] command [0xCCCC] optional payload <P1:P2....Pn>" },
#endif
  /* Add new commands above here */
  { NULL, NULL,                                      NULL, NULL,                              NULL,  NULL }
};


/* Static command zdo
 * command get_short_addr
 *
 * zdo get_short_addr [ieee]
 */
static zb_ret_t zdo_get_short_addr(int argc, char *argv[])
{
  zb_ret_t ret = RET_OK;
  zb_ieee_addr_t new_ieee_addr = { 0 };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [ieee] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 0, new_ieee_addr);

  /* Do it */
  {
    zb_uint16_t new_short_addr = zb_address_short_by_ieee(new_ieee_addr);

    if(new_short_addr != ZB_UNKNOWN_SHORT_ADDR)
    {
      menu_printf("get_short_addr("TRACE_FORMAT_64"): 0x%04x", TRACE_ARG_64(new_ieee_addr), new_short_addr);
    }
    else
    {
      menu_printf("get_short_addr("TRACE_FORMAT_64"): Unknown", TRACE_ARG_64(new_ieee_addr));
      ret = RET_NOT_FOUND;
    }
  }

  return ret;
}


/* Static stack callback function
 * response for zb_zdo_ieee_addr_req
 */
static void get_ieee_addr_cb(zb_uint8_t param)
{
  const zb_zdo_ieee_addr_resp_t *resp = (zb_zdo_ieee_addr_resp_t*)zb_buf_begin(param);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_ieee_addr_cb(0x%04x) failed: %s", resp->nwk_addr_remote_dev, get_zdp_status_str(resp->status));
  }
  else
  {
    menu_printf("get_ieee_addr_cb(0x%04x): "TRACE_FORMAT_64, resp->nwk_addr_remote_dev, TRACE_ARG_64(resp->ieee_addr_remote_dev));
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_ieee_addr
 *
 * zdo get_ieee_addr [addr]
 */
static zb_ret_t zdo_get_ieee_addr(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &new_short_addr);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_ieee_addr_req_param_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_ieee_addr_req_param_t);
    req->nwk_addr     = new_short_addr;
    req->dst_addr     = new_short_addr;
    req->start_index  = 0;
    req->request_type = 0;
    seq_num = zb_zdo_ieee_addr_req(buffer, get_ieee_addr_cb);

    menu_printf("get_ieee_addr(0x%04x) trans. seq num %hhu", req->nwk_addr, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_active_ep_req
 */
static void get_active_cb(zb_uint8_t param)
{
  const zb_zdo_ep_resp_t *resp = (zb_zdo_ep_resp_t*)zb_buf_begin(param);
  const zb_uint8_t *ep_list = (zb_uint8_t*)zb_buf_begin(param) + sizeof(zb_zdo_ep_resp_t);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_active_ep_cb(0x%04x) failed: %s", resp->nwk_addr, get_zdp_status_str(resp->status));
  }
  else
  {
    menu_printf("get_active_ep_cb(0x%04x): Endpoints count: %hu, trans. seq num %hhu", resp->nwk_addr, resp->ep_count, resp->tsn);
    for(int i=0; i < resp->ep_count; i++)
      menu_printf("\tEndpoint: %hd", *(ep_list+i));
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_active
 *
 * zdo get_active [addr]
 */
static zb_ret_t zdo_get_active(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_active_ep_req_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_active_ep_req_t));
    req->nwk_addr = new_short_addr;
    seq_num = zb_zdo_active_ep_req(buffer, get_active_cb);

    menu_printf("get_active_ep(0x%04x) trans. seq num %hhu", req->nwk_addr, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_bind_table_req
 */
static zb_uint16_t bind_table_dest_addr[256]={0};
static void get_bind_table_cb(zb_uint8_t param)
{
  const zb_zdo_mgmt_bind_resp_t *resp = (zb_zdo_mgmt_bind_resp_t*)zb_buf_begin(param);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("bind_table_cb(0x%04x) trans. seq num: %hhu failed: %s", bind_table_dest_addr[resp->tsn], resp->tsn, get_zdp_status_str(resp->status));
  }
  else
  {
    zb_zdo_binding_table_record_t *record = (zb_zdo_binding_table_record_t *)(resp + 1);

    menu_printf("bind_table_cb(0x%04x) trans. seq num: %hhu: Start index: %hu, List count %hhu",
      bind_table_dest_addr[resp->tsn],
      resp->tsn,
      resp->start_index,
      resp->binding_table_list_count);

    for(int i=0; i < resp->binding_table_list_count; i++)
    {
      /* record = SrcIEEEAddr + SrcEndp + ClusterID + DstAddrMode + DestIEEEAddr + DestEndp = 21 bytes */
      zb_size_t record_len = sizeof(zb_zdo_binding_table_record_t);
      char DestStr[128];

      switch(record->dst_addr_mode) {
      case ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT:
        snprintf(DestStr, 128, "group 0x%04x", record->dst_address.addr_short);
        record_len -= 7U; /* record = SrcIEEEAddr + SrcEndp + ClusterID + DstAddrMode + GroupAddr = 14 bytes */
        break;
      case ZB_APS_ADDR_MODE_64_ENDP_PRESENT:           /*!< 64-bit extended address for DstAddress and DstEndpoint present  */
        snprintf(DestStr, 128, "addr "TRACE_FORMAT_64" ep %hhu", TRACE_ARG_64(record->dst_address.addr_long), record->dst_endp);
        break;
      default:
        snprintf(DestStr, 128, "unsupported addr_mode %u", record->dst_addr_mode);
        break;
      }
      menu_printf("bind_table_cb(0x%04x, %hd) Src {addr "TRACE_FORMAT_64" ep %d cluster %s}, Dest {%s}",
        bind_table_dest_addr[resp->tsn],
        resp->start_index+i,
        TRACE_ARG_64(record->src_address),
        record->src_endp,
        get_cluster_id_str(record->cluster_id),
        DestStr);

      /* Find next */
      record = (zb_zdo_binding_table_record_t *)((zb_uint8_t *)record + record_len);
    }
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_bind_table
 *
 * zdo get_bind_table [addr] [index]
 */
static zb_ret_t zdo_get_bind_table(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;
  zb_uint8_t new_start_index;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* get [index] */
  TOOLS_GET_ARG(ret, uint8,  argv, 1, &new_start_index);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_mgmt_bind_param_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_mgmt_bind_param_t);
    req->dst_addr = new_short_addr;
    req->start_index = new_start_index;
    seq_num = zb_zdo_mgmt_bind_req(buffer, get_bind_table_cb);
    bind_table_dest_addr[seq_num] = new_short_addr;

    menu_printf("get_bind_table(0x%04x) trans. seq num %hhu", req->dst_addr, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for get_node_desc_cb
 */
static void get_node_desc_cb(zb_uint8_t param)
{
  const zb_zdo_node_desc_resp_t *resp = (zb_zdo_node_desc_resp_t*)zb_buf_begin(param);

  if(resp->hdr.status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_node_desc_cb(0x%04x) failed: %s", resp->hdr.nwk_addr, get_zdp_status_str(resp->hdr.status));
  }
  else
  {
    menu_printf("get_node_desc_cb(0x%04x): Flag: 0x%04x: Type %hu, aps: %hu, frequency %hu",
      resp->hdr.nwk_addr,
      resp->node_desc.node_desc_flags,
      (resp->node_desc.node_desc_flags & 0x0007)>>0,
      (resp->node_desc.node_desc_flags & 0x0700)>>8,
      (resp->node_desc.node_desc_flags & 0xF800)>>11);
    menu_printf("get_node_desc_cb(0x%04x): Capability info %02x, Manufacturer: 0x%04x, max buf %hu, max transfer %hu",
      resp->hdr.nwk_addr,
      resp->node_desc.mac_capability_flags,
      resp->node_desc.manufacturer_code,
      resp->node_desc.max_buf_size,
      resp->node_desc.max_outgoing_transfer_size);
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command zdo_get_node_desc
 *
 * zdo get_node_desc [addr]
 */
static zb_ret_t zdo_get_node_desc(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_node_desc_req_t *req;

    buffer = zb_buf_get_out();
    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_node_desc_req_t));
    req->nwk_addr = new_short_addr;
    zb_zdo_node_desc_req(buffer, get_node_desc_cb);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for get_power_desc_cb
 */
static void get_power_desc_cb(zb_uint8_t param)
{
  const zb_zdo_power_desc_resp_t *resp = (zb_zdo_power_desc_resp_t*)zb_buf_begin(param);

  if(resp->hdr.status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_power_desc_cb(0x%04x) failed: %s", resp->hdr.nwk_addr, get_zdp_status_str(resp->hdr.status));
  }
  else
  {
    menu_printf("get_power_desc_cb(0x%04x): Flag: 0x%04x: Power mode %hu, Avail power src %hu, Cur power src %hu, Cur power level %hu",
      resp->hdr.nwk_addr,
      resp->power_desc.power_desc_flags,
      (resp->power_desc.power_desc_flags & 0x000F)>>0,
      (resp->power_desc.power_desc_flags & 0x00F0)>>4,
      (resp->power_desc.power_desc_flags & 0x0F00)>>8,
      (resp->power_desc.power_desc_flags & 0xF000)>>12);
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command zdo_get_power_desc
 *
 * zdo get_power_desc [addr]
 */
static zb_ret_t zdo_get_power_desc(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_power_desc_req_t *req;

    buffer = zb_buf_get_out();
    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_power_desc_req_t));
    req->nwk_addr = new_short_addr;
    zb_zdo_power_desc_req(buffer, get_power_desc_cb);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_simple_desc_req
 */
static void get_simp_desc_cb(zb_uint8_t param)
{
  const zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t*)zb_buf_begin(param);

  if(resp->hdr.status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_simp_desc_cb(0x%04x, %u) failed: %s", resp->hdr.nwk_addr, resp->simple_desc.endpoint, get_zdp_status_str(resp->hdr.status));
  }
  else
  {
    char listStr[256];
    int  listLen;

    menu_printf("get_simp_desc_cb(0x%04x, endpoint %u) profile 0x%04x=\'%s\', device 0x%04x=\'%s\', version %u, clusters %u",
      resp->hdr.nwk_addr,
      resp->simple_desc.endpoint,
      resp->simple_desc.app_profile_id,
      get_endpoint_profile_id_str(resp->simple_desc.app_profile_id),
      resp->simple_desc.app_device_id,
      get_endpoint_device_id_str(resp->simple_desc.app_device_id),
      resp->simple_desc.app_device_version,
      resp->simple_desc.app_input_cluster_count + resp->simple_desc.app_output_cluster_count
      );

    listLen = 0;listStr[listLen] = '\0';
    for(int s = 0; s < resp->simple_desc.app_input_cluster_count; s++)
      listLen += wcs_snprintf(listStr+listLen, 256-listLen, "0x%04x ", resp->simple_desc.app_cluster_list[s]);
    menu_printf("\tservers: %s", listStr);
    listLen = 0;listStr[listLen] = '\0';
    for(int c = resp->simple_desc.app_input_cluster_count; c < resp->simple_desc.app_input_cluster_count + resp->simple_desc.app_output_cluster_count; c++)
      listLen += wcs_snprintf(listStr+listLen, 256-listLen, "0x%04x ", resp->simple_desc.app_cluster_list[c]);
    menu_printf("\tclients: %s", listStr);
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_simple_desc
 *
 * zdo get_simple_desc [addr] [endpoint]
 */
static zb_ret_t zdo_get_simp_desc(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;
  zb_uint8_t new_ep_id;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,  argv, 1, &new_ep_id);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_simple_desc_req_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_simple_desc_req_t));
    req->nwk_addr = new_short_addr;
    req->endpoint = new_ep_id;
    seq_num = zb_zdo_simple_desc_req(buffer, get_simp_desc_cb);

    menu_printf("get_simple_descriptor(0x%04x, %u) trans. seq num %hhu", req->nwk_addr, req->endpoint, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_match_desc_req
 */
static void get_match_desc_cb(zb_uint8_t param)
{
  const zb_zdo_match_desc_resp_t *resp = (zb_zdo_match_desc_resp_t*)zb_buf_begin(param);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_match_desc_cb(0x%04x) failed: %s", resp->nwk_addr, get_zdp_status_str(resp->status));
  }
  else if(zb_buf_len(param) < sizeof(zb_zdo_match_desc_resp_t)+(resp->match_len*sizeof(zb_uint8_t)))
  {
    menu_printf("get_match_desc_cb(0x%04x) status: %s but buffe incomplete", resp->nwk_addr, get_zcl_status_str(resp->status));
  }
  else
  {
    char listStr[256] = {""};
    int  listLen = 0;
    zb_uint8_t *match_ep = (zb_uint8_t*)(resp + 1);

    for(int i=0; i<resp->match_len; i++)
      listLen += wcs_snprintf(listStr+listLen, 256-listLen, "%hhu ", match_ep[i]);

    menu_printf("get_match_desc_cb(0x%04x) match_len %d: endpoints %s",
      resp->nwk_addr,
      resp->match_len,
      listStr);
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_match_desc
 *
 * zdo get_match_desc [addr] [profile] [#in:#out] [clusters]
 */
static zb_ret_t zdo_get_match_desc(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;
  zb_uint16_t new_profile = 0xFFFF;
  zb_uint8_t new_nb_in;
  zb_uint8_t new_nb_out;
  zb_uint8_t nb_clusters;
  zb_uint16_t *cluster_list;

  if(argc < 4)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* get [profile] */
  TOOLS_GET_ARG(ret, profile, argv, 1, &new_profile);

  /* get [#in:#out] */
  if(2 != sscanf(argv[2], "%hhu:%hhu", &new_nb_in, &new_nb_out))
    return RET_INVALID_PARAMETER_3;

  nb_clusters = new_nb_in + new_nb_out;
  if(argc != (3 + nb_clusters))
    return RET_INVALID_PARAMETER;

  cluster_list = malloc(nb_clusters*sizeof(zb_uint16_t));
  if(!cluster_list)
    return RET_NO_MEMORY;

  /* get [clusters] */
  for(zb_uint8_t cl_idx=0; cl_idx<nb_clusters; cl_idx++)
  {
    zb_cluster_entry *this_cluster;

    ret = tools_arg_cluster(argv, 3+cl_idx, &this_cluster, ZB_FALSE);
    if(ret != RET_OK)
    {
      free(cluster_list);
      return ret;
    }

    cluster_list[cl_idx] = this_cluster->id;
  }

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_match_desc_param_t *req;
    zb_uint8_t req_cluster_list_size = sizeof(req->cluster_list)/sizeof(req->cluster_list[0]);

    buffer = zb_buf_get_out();
    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_match_desc_param_t));
    req->nwk_addr         = new_short_addr;
    req->addr_of_interest = new_short_addr;
    req->profile_id       = new_profile;
    req->num_in_clusters  = new_nb_in;
    req->num_out_clusters = new_nb_out;
    /* if req->cluster_list is too small, increase it */
    if(nb_clusters > req_cluster_list_size)
      zb_buf_alloc_right(buffer, (nb_clusters-req_cluster_list_size)*sizeof(req->cluster_list[0]));
    for(zb_uint8_t cl_idx=0; cl_idx<nb_clusters; cl_idx++)
      req->cluster_list[cl_idx] = cluster_list[cl_idx];
    free(cluster_list);
    zb_zdo_match_desc_req(buffer, get_match_desc_cb);
  }

  return RET_OK;
}

static zb_ret_t help_get_match_desc(void)
{
  menu_printf("profile: either its 16bits id value or its initial in:");
  help_profiles();
  menu_printf("cluster: either its 16bits id value or its initial in:");
  help_clusters();
  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_mgmt_rtg_req
 */
static zb_uint16_t dest2get_route;
static void get_route_cb(zb_uint8_t param)
{
  zb_zdo_mgmt_rtg_resp_t *resp = (zb_zdo_mgmt_rtg_resp_t *)zb_buf_begin(param);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_route_cb(0x%04x, %hhu) failed: %s", dest2get_route, resp->tsn, get_zdp_status_str(resp->status));
  }
  else
  {
    zb_zdo_routing_table_record_t *record = (zb_zdo_routing_table_record_t *)(resp + 1);

    menu_printf("get_route_cb(0x%04x, %hhu) %s, index %u, nb %u, total %u", dest2get_route, resp->tsn, get_zcl_status_str(resp->status), 
      resp->start_index,
      resp->routing_table_list_count,
      resp->routing_table_entries);

    for(zb_uint8_t i=0; i<resp->routing_table_list_count; i++)
    {
      menu_printf("\tROUTE[%u] dest 0x%04x, flags 0x%02x, next-hop 0x%04x",
        resp->start_index + i,
        record[i].dest_addr,
        record[i].flags,
        record[i].next_hop_addr);
    }

    if(resp->start_index + resp->routing_table_list_count < resp->routing_table_entries)
    {
      zb_ret_t ret;
      zb_zdo_mgmt_rtg_param_t *req;
      req = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_rtg_param_t);
      req->dst_addr = dest2get_route;
      req->start_index = resp->start_index + resp->routing_table_list_count;

      menu_printf("do_next: zdo get_route 0x%04x %u", req->dst_addr, req->start_index);

      ret = zb_zdo_mgmt_rtg_req(param, get_route_cb);
      if(ret == RET_OK)
        param = 0; // not to free it, it will be done by get_route_cb()
      else
        menu_printf("zdo get_route failed: %s", wcs_get_error_str(ret));
    }
  }

  if (param != 0)
    zb_buf_free(param);
  menu_cb_occured();
}

/* Static command zdo
 * command get_route
 *
 * zdo get_route [addr] [idx]
 */
static zb_ret_t zdo_get_route(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;
  zb_uint8_t new_index;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* get [index] */
  TOOLS_GET_ARG(ret, uint8 , argv, 1, &new_index);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_mgmt_rtg_param_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_mgmt_rtg_param_t);
    req->dst_addr    = new_short_addr;
    req->start_index = new_index;
    dest2get_route = new_short_addr;
    seq_num = zb_zdo_mgmt_rtg_req(buffer, get_route_cb);

    menu_printf("get_route(0x%04x) trans. seq num %hhu", req->dst_addr, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_mgmt_lqi_req
 */
static zb_uint16_t dest2get_lqi;
zb_ret_t zdo_get_diag_data(zb_uint16_t short_address, zb_uint8_t *lqa, zb_int8_t *rssi);
static void get_lqi_cb(zb_uint8_t param)
{
  zb_zdo_mgmt_lqi_resp_t *resp = (zb_zdo_mgmt_lqi_resp_t *)zb_buf_begin(param);

  if(resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("get_lqi_cb(0x%04x, %hhu) failed: %s", dest2get_lqi, resp->tsn, get_zdp_status_str(resp->status));
  }
  else
  {
    zb_uindex_t neighbor_index;

    menu_printf("get_lqi_cb(0x%04x, %hhu) %s, index %u, nb %u, total %u", dest2get_lqi, resp->tsn, get_zcl_status_str(resp->status),
      resp->start_index,
      resp->neighbor_table_list_count,
      resp->neighbor_table_entries);

    for(neighbor_index = 0; neighbor_index < resp->neighbor_table_list_count; neighbor_index++)
    {
      const void *neighbors_list = (const void*)(resp + 1);
      const zb_zdo_neighbor_table_record_t *neighbor = &((const zb_zdo_neighbor_table_record_t*) neighbors_list)[neighbor_index];
      zb_uint8_t lqi = ZB_MAC_LQI_UNDEFINED;
      zb_int8_t rssi = ZB_MAC_RSSI_UNDEFINED;

      zb_zdo_get_diag_data(dest2get_lqi, &lqi, &rssi);

      if(lqi != ZB_MAC_LQI_UNDEFINED && rssi != ZB_MAC_RSSI_UNDEFINED)
        menu_printf("\tLQI[%hd]: dest 0x%04x, lqa %hd (diag_data: lqi: %hu, rssi: %hd)",
          neighbor_index,
          neighbor->network_addr,
          neighbor->lqa,
          lqi,
          rssi);
      else
        menu_printf("\tLQI[%hd]: dest 0x%04x, lqa %hu (diag_data error)",
          neighbor_index,
          neighbor->network_addr,
          neighbor->lqa);
    }
  }

  if (param != 0)
    zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command get_lqi
 *
 * zdo get_lqi [addr] [idx]
 */
static zb_ret_t zdo_get_lqi(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_short_addr;
  zb_uint8_t new_index;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] ieee_addr converted to short */
  TOOLS_GET_ARG_SHORT_ADDR(ret, argv, 0, new_short_addr);

  /* get [index] */
  TOOLS_GET_ARG(ret, uint8 , argv, 1, &new_index);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_mgmt_lqi_param_t *req;
    zb_uint8_t seq_num;

    buffer = zb_buf_get_out();
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_mgmt_lqi_param_t);
    req->dst_addr    = new_short_addr;
    req->start_index = new_index;
    dest2get_lqi = new_short_addr;
    seq_num = zb_zdo_mgmt_lqi_req(buffer, get_lqi_cb);

    menu_printf("get_lqi(0x%04x) trans. seq num %hhu", req->dst_addr, seq_num);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_active_scan_req
 */
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
static zb_uint16_t panid2join = 0xFFFF;
#endif
static void get_active_scan_cb(zb_uint8_t param)
{
  zb_zdo_active_scan_resp_t *resp;

  resp = (zb_zdo_active_scan_resp_t *)zb_buf_begin(param);

  menu_printf("active_scan_cb() status: %s", wcs_get_error_str(resp->status));

  /* Note : MAC LIMIT_REACHED means that the scan operation terminated prematurely because the number
   * of PAN descriptors stored reached the maximum specified in the MAC layer. 
   * However the beacons are still reported but some might be missing */
  if(resp->network_count > 0)
  {
    zb_nlme_network_descriptor_t *nwk_descr;
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
    zb_bool_t join_network = ZB_FALSE;
    zb_bool_t network_distributed = ZB_FALSE;
#endif

    menu_printf("%u network discovered", resp->network_count);

    nwk_descr = (zb_nlme_network_descriptor_t *)(resp + 1);
    for (zb_uint32_t i = 0; i < resp->network_count; i++)
    {
      zb_uint16_t     short_pan_id;
      zb_ext_pan_id_t ext_pan_id;
      zb_int8_t       rssi_pan_id = ZB_MAC_RSSI_UNDEFINED;
      zb_uint8_t      lqi_pan_id = ZB_MAC_LQI_UNDEFINED;

      zb_address_get_short_pan_id(nwk_descr[i].panid_ref, &short_pan_id);
      zb_address_get_pan_id(nwk_descr[i].panid_ref, ext_pan_id);
      zb_address_get_diag_data(nwk_descr[i].panid_ref, &lqi_pan_id, &rssi_pan_id);

      menu_printf("\tNWK[%u] channel %d, panid 0x%04x - "TRACE_FORMAT_64", coordinator %d, permit_join %u, accept {zr %u zed %u} lqi %hd rssi %hd %s",
        i,
        nwk_descr[i].logical_channel,
        short_pan_id,
        TRACE_ARG_64(ext_pan_id),
        nwk_descr[i].pan_coordinator,
        nwk_descr[i].permit_joining,
        nwk_descr[i].router_capacity,
        nwk_descr[i].end_device_capacity,
        lqi_pan_id,
        rssi_pan_id,
        (panid2join != 0xFFFF && panid2join == short_pan_id)?(" JOIN"):(""));

#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
      if(panid2join != 0xFFFF && panid2join == short_pan_id)
      {
        network_distributed = config_update(nwk_descr[i].logical_channel, short_pan_id, ext_pan_id);
        if(!nwk_descr[i].permit_joining)
        {
          menu_printf("cannot join this now, the network is closed");
          continue;
        }
        join_network = ZB_TRUE;
        panid2join = 0xFFFF;
      }
#endif
    }
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
    if(join_network)
    {
      (void)network_do_join(network_distributed);
    }
#endif
  }

  zb_buf_free(param);
  panid2join = 0xFFFF;
  menu_cb_occured();
}


/* Static command zdo
 * command active_scan
 *
 * zdo active_scan [duration] [channel_mask] <panid2join>
 */
static zb_ret_t zdo_active_scan(int argc, char *argv[])
{
#ifndef ZB_MAX_SCAN_DURATION
#define ZB_MAX_SCAN_DURATION 14U
#endif
  zb_ret_t ret;
  zb_uint8_t new_duration;
  zb_uint32_t new_channel_mask;

  if(argc < 2  || argc > 3)
    return RET_INVALID_PARAMETER;

  /* get [duration] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &new_duration);/* || new_duration > ZB_MAX_SCAN_DURATION*/

  /* get [channel_mask] */
  TOOLS_GET_ARG_HEXA(ret, uint32,  argv, 1, &new_channel_mask);

  /* get <panid2join> */
  if(argc == 3)
  {
    if(panid2join != 0xFFFF)
      return RET_BUSY;

    TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 2, &panid2join);
  }
  else
  {
    panid2join = 0xFFFF;
  }

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_active_scan_request_t *req;

    buffer = zb_buf_get_out();
//    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_active_scan_request_t));
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_active_scan_request_t);
    req->scan_duration         = new_duration;
    req->scan_channels_list[0] = new_channel_mask;
    req->cb                    = get_active_scan_cb;
    zb_zdo_active_scan_request(buffer);
  }

  return RET_OK;
}

static zb_ret_t help_active_scan(void)
{
  menu_printf("panid2join: if defined, this network will be joined automatically if found & open");
  menu_printf("duration: Valid range: 0-14. Duration of scan in symbols: 960 * ((2^duration) + 1)");
  return RET_OK;
}


#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
/* Static stack callback function
 * response for zb_zdo_energy_scan_req
 */
static void get_energy_scan_cb(zb_uint8_t param)
{
  zb_energy_detect_list_t *ed_list = ZB_BUF_GET_PARAM(param, zb_energy_detect_list_t);
  zb_ret_t status = zb_buf_get_status(param);

  if(status != RET_OK)
  {
    menu_printf("energy_scan_cb() Error: %s channel_cnt %u", wcs_get_error_str(status), ed_list->channel_count);
  }
  else
  {
    menu_printf("energy_scan_cb() OK, %u channel scanned", ed_list->channel_count);
    for(zb_uint8_t i=0; i < ed_list->channel_count; i++)
    {
      menu_printf("\tCHANNEL[%u] channel %u, energy detected 0x%02X: %s",
        i,
        ed_list->channel_info[i].channel_number,
        ed_list->channel_info[i].energy_detected,
        (ed_list->channel_info[i].energy_detected > ZB_NWK_CHANNEL_ACCEPT_LEVEL)?("GOOD"):("low"));
    }
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command energy_scan
 *
 * zdo energy_scan [duration] [channel_mask]
 */
static zb_ret_t zdo_energy_scan(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_duration;
  zb_uint32_t new_channel_mask;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [duration] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &new_duration);

  /* get [channel_mask] */
  TOOLS_GET_ARG_HEXA(ret, uint32,  argv, 1, &new_channel_mask);

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_ed_scan_param_t *req;

    buffer = zb_buf_get_out();
//    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_ed_scan_param_t));
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_ed_scan_param_t);
    req->nwk_param.scan_duration         = new_duration;
    req->nwk_param.scan_channels_list[0] = new_channel_mask;
    req->cb                              = get_energy_scan_cb;
    zb_zdo_ed_scan_request(buffer);
  }

  return RET_OK;
}
static zb_ret_t help_energy_scan(void)
{
  menu_printf("coordinator or router only");
  menu_printf("duration: Valid range: 0-14. Duration of scan in symbols: 960 * ((2^duration) + 1)");
  return RET_OK;
}
#endif /* defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE) */


#ifndef ZB_ZDO_H
/* Hidden APIs from zb_zdo.h... */
#define ZB_ZDO_CB_UNICAST_COUNTER     1U  /* 1 resp for Unicast */
#define ZB_ZDO_CB_BROADCAST_COUNTER   0xFFU  /* wait for timeout */
#define ZB_ZDO_CB_DEFAULT_COUNTER     0xFEU    /* Default value (just for check) */
extern zb_uint8_t zdo_send_req_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_callback_t cb, zb_uint16_t addr, zb_uint8_t resp_counter);
extern zb_uint8_t zdo_send_req_by_long(zb_uint16_t command_id, zb_uint8_t param, zb_callback_t cb, zb_ieee_addr_t addr);
extern void zdo_send_resp_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_uint16_t addr, zb_bool_t aps_secur);
#endif

/* Static stack callback function
 * response for zdo_raw_cmd
 */
static void raw_cmd_cb(zb_uint8_t param)
{
  zb_zdo_desc_resp_hdr_t *resp_hdr = (zb_zdo_desc_resp_hdr_t *)zb_buf_begin(param);
  zb_uint8_t *resp_data_ptr = (zb_uint8_t *)(resp_hdr);
  zb_uint_t   resp_data_len = zb_buf_len(param);

  if(resp_hdr->status != ZB_ZDP_STATUS_SUCCESS)
  {
    menu_printf("raw_cmd_cb(0x%04x, %hhu) failed: %s", resp_hdr->nwk_addr, resp_hdr->tsn, get_zdp_status_str(resp_hdr->status));
  }
  else
  {
    menu_printf("raw_cmd_cb(0x%04x, %hhu) %s, %u bytes", resp_hdr->nwk_addr, resp_hdr->tsn, get_zcl_status_str(resp_hdr->status), resp_data_len);

    for(zb_uint8_t i=0; i<resp_data_len; i+=16)
    {
      char dataStr[50];
      int   dataLen = 0;

      for(zb_uint8_t j=0; j<16 && i+j<resp_data_len; j++)
        dataLen += wcs_snprintf(dataStr+dataLen, 50-dataLen, "%02x:", resp_data_ptr[i+j]);
      dataStr[dataLen-1] = '\0';/* remove last ':' */

      menu_printf("\tAPS[%04x] %s", i, dataStr);
    }
  }

  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command zdo_raw_cmd
 *
 * zdo raw_cmd [addr] [cmd] <payload>
 */
static zb_ret_t zdo_raw_cmd(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr = 0xFFFF;
  zb_ieee_addr_t dest_ieee_addr = { 0 };
  zb_uint16_t dest_raw_cmd;
  zb_uint_t dest_payload_len = 0;
  zb_uint8_t *dest_payload_data = NULL;

  if(argc < 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] short or ieee */
  if((ret = tools_arg_get_uint16(argv, 0, &dest_short_addr, ARG_HEX)) != RET_OK &&
     (ret = tools_arg_get_ieee  (argv, 0, dest_ieee_addr,   ARG_HEX)) != RET_OK)
  {
   return ret;
  }

  /* get [cmd] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_raw_cmd);

  /* get <payload> */
  if(argc == 3)
  {
    /* get <payload len> */
    TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 2, &dest_payload_len);
    dest_payload_data = malloc(sizeof(zb_uint8_t)*dest_payload_len);
    if(!dest_payload_data)
      return RET_NO_MEMORY;
    /* get <payload data> */
    ret = tools_arg_get_hex_array_data(argv, 2, dest_payload_len, dest_payload_data);
    if(ret != RET_OK)
    {
      free(dest_payload_data);
      return ret;
    }
  }

  if (!ZB_JOINED())
  {
    if(dest_payload_data) free(dest_payload_data);
    return RET_UNAUTHORIZED;
  }


  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_uint8_t *ptr_buf;
    zb_uint8_t tsn;

    buffer = zb_buf_get_out();

    if(dest_payload_len > 0)
    {
      ptr_buf = zb_buf_initial_alloc(buffer, dest_payload_len);
      memcpy(ptr_buf, dest_payload_data, dest_payload_len);
    }

    if(dest_short_addr != 0xFFFF)
      tsn = zdo_send_req_by_short(dest_raw_cmd, buffer, raw_cmd_cb, dest_short_addr, ZB_ZDO_CB_DEFAULT_COUNTER);
    else
      tsn = zdo_send_req_by_long(dest_raw_cmd, buffer, raw_cmd_cb, dest_ieee_addr);

    menu_printf("%s(0x%04x) trans. seq num %hhu", __FUNCTION__, dest_raw_cmd, tsn);

  }

  if(dest_payload_data) free(dest_payload_data);

  return RET_OK;
}

static zb_ret_t help_raw_cmd(void)
{
  menu_printf("Send ZDO APS commands using zdo_send_req_by_short()/zdo_send_req_by_long() APIs)");
  menu_printf("payload: it corresponds to the APS payload (without sequence number on the request, it will be returned)");
  return RET_OK;
}


/* Static command zdo
 * command data_ind
 *
 * zdo data_ind [enable]
 */
static zb_uint8_t data_indication(zb_uint8_t param);
static zb_ret_t zdo_data_ind(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_enable;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [enable] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_enable);
  if(new_enable >= 2)
      return RET_INVALID_PARAMETER_1;

  zb_af_set_data_indication((new_enable)?(data_indication):(NULL));

  return RET_OK;
}

static zb_uint8_t data_indication(zb_uint8_t param)
{
  zb_uint8_t aps_payload_size;
  zb_uint8_t *aps_payload_ptr;
  zb_apsde_data_indication_t *data_ind = ZB_BUF_GET_PARAM(param, zb_apsde_data_indication_t);
  zb_bool_t processed = ZB_FALSE;

  menu_printf("Recv DATA_IND from 0x%04x ep %u to 0x%04x ep %u group 0x%04x profileid: 0x%04x clusterid 0x%04x lqi: %hu, rssi: %hd",
    data_ind->src_addr,
    data_ind->src_endpoint,
    data_ind->dst_addr,
    data_ind->dst_endpoint,
    data_ind->group_addr,
    data_ind->profileid,
    data_ind->clusterid,
    data_ind->lqi,
    data_ind->rssi
    );

  aps_payload_size = zb_buf_len(param);
  aps_payload_ptr = (zb_uint8_t*)zb_buf_begin(param);

  for(zb_uint8_t i=0; i<aps_payload_size; i+=16)
  {
    char dataStr[50];
    int   dataLen = 0;

    for(zb_uint8_t j=0; j<16 && i+j<aps_payload_size; j++)
      dataLen += wcs_snprintf(dataStr+dataLen, 50-dataLen, "%02x:", aps_payload_ptr[i+j]);
    dataStr[dataLen-1] = '\0';/* remove last ':' */

    menu_printf("\tAPS[%04x] %s", i, dataStr);
 }

  return processed;
}

#ifdef ZBOSS_ZDO_APP_TSN_ENABLE
#ifdef TSN_POLICY_USE_LOCK
static void tsn_lock(zb_uint8_t param);
#endif
static void zdo_aps_cmd_cb(zb_uint8_t param);

/* Static command zdo
 * command zdo_tsn_policy
 *
 * zdo tsn_policy [tsn_policy] [start_tsn]
 */
static zb_ret_t zdo_tsn_policy(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_policy;
  zb_uint8_t new_aps_tsn_value;

  if(argc < 1 && argc > 2)
    return RET_INVALID_PARAMETER;

  /* get [tsn_policy] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_policy);
  if(new_policy >= 2)
    return RET_INVALID_PARAMETER_1;

  if(new_policy == ZDO_TSN_POLICY_RANGE_128_254)
  {
    if(argc != 2)
      return RET_INVALID_PARAMETER;

    /* get [start_tsn] */
    TOOLS_GET_ARG(ret, uint8, argv, 1, &new_aps_tsn_value);
    if(new_aps_tsn_value >= 128)
      return RET_INVALID_PARAMETER_2;

    aps_tsn_value = new_aps_tsn_value;

    zb_aps_set_user_data_tx_cb(zdo_aps_cmd_cb);
  }
  else
  {
    zb_aps_set_user_data_tx_cb(NULL);
  }


#ifdef TSN_POLICY_USE_LOCK
  zb_zdo_tsn_policy_set(new_policy, tsn_lock);
#else
  zb_zdo_tsn_policy_set(new_policy, NULL);
#endif

  /* Now, we can use :
   * - zb_zdo_app_tsn_allocate():       allocate & define TSN to use (app_tsn_entry->app_tsn = ...)
   * - zb_zdo_app_tsn_get_by_buf_ref(): get current TSN (app_tsn_entry->app_tsn)
   * - zb_zdo_app_tsn_release():        release TSN
   */

  return RET_OK;
}

#ifdef TSN_POLICY_USE_LOCK
static void tsn_lock(zb_uint8_t param)
{
  if(param)
  {
    //TODO: LOCK
  }
  else
  {
    //TODO: UNLOCK
  }
}
#endif

/* Static stack callback function
 * status of zdo_aps_cmd
 */
static void zdo_aps_cmd_cb(zb_uint8_t param)
{
  zb_ret_t buf_status = zb_buf_get_status(param);
  zb_aps_user_payload_cb_status_t aps_status = zb_buf_get_status(param);
  zb_zdo_app_tsn_entry_t *app_tsn_entry = zb_zdo_app_tsn_get_by_buf_ref(param);

  if(aps_status != ZB_APS_USER_PAYLOAD_CB_STATUS_SUCCESS)
  {
    menu_printf("zdo_aps_cmd_cb(%hhu) failed %s",
      (app_tsn_entry)?(app_tsn_entry->app_tsn):(-1),
      (aps_status == ZB_APS_USER_PAYLOAD_CB_STATUS_NO_APS_ACK)?("Aps No Ack"):(wcs_get_error_str(buf_status)));
  }
  else
  {
    zb_uint8_t aps_payload_size = 0U;
    zb_uint8_t *aps_payload_ptr;

    aps_payload_ptr = zb_aps_get_aps_payload(param, &aps_payload_size);

    menu_printf("zdo_aps_cmd_cb(%hhu) %s, %u bytes", (app_tsn_entry)?(app_tsn_entry->app_tsn):(-1), get_zcl_status_str(aps_status), aps_payload_size);

    for(zb_uint8_t i=0; i<aps_payload_size; i+=16)
    {
      char dataStr[50];
      int   dataLen = 0;

      for(zb_uint8_t j=0; j<16 && i+j<aps_payload_size; j++)
        dataLen += wcs_snprintf(dataStr+dataLen, 50-dataLen, "%02x:", aps_payload_ptr[i+j]);
      dataStr[dataLen-1] = '\0';/* remove last ':' */

      menu_printf("\tAPS[%04x] %s", i, dataStr);
    }
  }

  if(app_tsn_entry)
    zb_zdo_app_tsn_release(param);
  zb_buf_free(param);
  menu_cb_occured();
}


/* Static command zdo
 * command zdo_aps_cmd
 *
 * zdo aps_cmd [addr] [cmd] <payload>
 */
static zb_ret_t zdo_aps_cmd(int argc, char *argv[])
{
  zb_ret_t ret = RET_ERROR;
  zb_uint16_t dest_short_addr = 0xFFFF;
  zb_ieee_addr_t dest_ieee_addr = { 0 };
  zb_uint16_t dest_raw_cmd;
  zb_uint_t dest_payload_len = 0;
  zb_uint8_t *dest_payload_data = NULL;

  if(argc < 2)
    return RET_INVALID_PARAMETER;

  /* get [addr] short or ieee */
  if((ret = tools_arg_get_uint16(argv, 0, &dest_short_addr, ARG_HEX)) != RET_OK &&
     (ret = tools_arg_get_ieee  (argv, 0, dest_ieee_addr,   ARG_HEX)) != RET_OK)
  {
   return ret;
  }

  /* get [cmd] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_raw_cmd);

  /* get <payload> */
  if(argc == 3)
  {
    /* get <payload len> */
    TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 2, &dest_payload_len);
    /* Index 0 for TSN */
    dest_payload_data = malloc(sizeof(zb_uint8_t)*dest_payload_len + 1);
    if(!dest_payload_data)
      return RET_NO_MEMORY;
    /* get <payload data> */
    ret = tools_arg_get_hex_array_data(argv, 2, dest_payload_len, &dest_payload_data[1]);
    if(ret != RET_OK)
    {
      free(dest_payload_data);
      return ret;
    }
  }
  else
  {
    dest_payload_data = malloc(1);
    if(!dest_payload_data)
      return RET_NO_MEMORY;
  }

  if (!ZB_JOINED())
  {
    if(dest_payload_data) free(dest_payload_data);
    return RET_UNAUTHORIZED;
  }


  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_aps_addr_mode_t addr_mode;
    zb_addr_u addr_addr = {0};
    zb_zdo_app_tsn_entry_t* app_tsn_entry;

    buffer = zb_buf_get_out();

    app_tsn_entry = zb_zdo_app_tsn_allocate(buffer);
    if(!app_tsn_entry)
      return RET_OPERATION_FAILED;
    app_tsn_entry->app_tsn = aps_tsn_value;

    /* Index 0 for TSN */
    dest_payload_data[0] = aps_tsn_value;
    dest_payload_len++;

    if(dest_short_addr != 0xFFFF)
    {
      addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      addr_addr.addr_short = dest_short_addr;
    }
    else
    {
      addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      ZB_IEEE_ADDR_COPY(addr_addr.addr_long, dest_ieee_addr);
    }

    ret = zb_aps_send_user_payload(
      buffer,
      addr_addr,             /* dst_addr */
      ZB_AF_ZDO_PROFILE_ID,  /* profile id */
      dest_raw_cmd,          /* zdo command id */
      0,                     /* destination endpoint */
      0,                     /* source endpoint */
      addr_mode,
      ZB_TRUE, /* Ack enabled */
      dest_payload_data,
      dest_payload_len);

    if(ret == RET_OK)
    {
      aps_tsn_value++;
      if(aps_tsn_value > 127) aps_tsn_value = 0;
    }
    else
    {
      zb_zdo_app_tsn_release(buffer);
    }

    menu_printf("%s(0x%04x) trans. seq num %hhu: %s", __FUNCTION__, dest_raw_cmd, app_tsn_entry->app_tsn, wcs_get_error_str(ret));

  }

  if(dest_payload_data) free(dest_payload_data);

  return RET_OK;
}

static zb_ret_t help_aps_cmd(void)
{
  menu_printf("Send APS commands using ZBOSS_ZDO_APP_TSN_ENABLE APIs)");
  menu_printf("payload: it corresponds to the APS payload (without sequence number on the request, it will be returned)");
  menu_printf("status:  catched by callback registered with zb_aps_set_user_data_tx_cb()");
  return RET_OK;
}
#endif

