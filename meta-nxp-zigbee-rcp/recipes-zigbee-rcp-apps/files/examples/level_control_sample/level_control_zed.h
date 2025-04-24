/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: ZR Level Control sample
*/

#ifndef _LEVEL_CONTROL_ZED_H_
#define _LEVEL_CONTROL_ZED_H_

#include "zboss_api.h"

#define ZB_LEVEL_CONTROL_SWITCH_DEVICE_ID 1
#define ZB_LEVEL_CONTROL_SWITCH_DEVICE_VER 1
#define ZB_LEVEL_CONTROL_SWITCH_IN_CLUSTER_NUM_ZED 2
#define ZB_LEVEL_CONTROL_SWITCH_OUT_CLUSTER_NUM_ZED 3

typedef struct peer_entry_s
{
  zb_ieee_addr_t ieee_addr;
  zb_uint16_t cluster_id;
  zb_uint8_t cluster_role;
  zb_uint8_t endpoint;
  zb_uint16_t peer_revision;
} peer_entry_t;

/*! Program states according to test scenario */
enum test_step_e
{
  LEVEL_CONTROL_SWITCH_SEND_MOVE_TO_LEVEL,
  LEVEL_CONTROL_SWITCH_SEND_MOVE,
  LEVEL_CONTROL_SWITCH_SEND_STOP,
  LEVEL_CONTROL_SWITCH_SEND_STEP_DOWN,
  LEVEL_CONTROL_SWITCH_SEND_STEP_UP,
  LEVEL_CONTROL_SWITCH_SEND_MOVE_TO_LEVEL_ZCL8,
  LEVEL_CONTROL_SWITCH_SEND_MOVE_ZCL8,
  LEVEL_CONTROL_SWITCH_SEND_STOP_ZCL8,
  LEVEL_CONTROL_SWITCH_SEND_STEP_DOWN_ZCL8,
  LEVEL_CONTROL_SWITCH_SEND_STEP_UP_ZCL8,
  LEVEL_CONTROL_SWITCH_FINISHED
};

/*!
  @brief Declare attribute list for Level Control cluster
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_CLI_ATTRIB_LIST(attr_list)                      \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_LEVEL_CONTROL) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare cluster list for Level Control Switch device
    @param cluster_list_name - cluster list variable name
    @param basic_attr_list - attribute list for Basic cluster
    @param identify_attr_list - attribute list for Identify cluster
 */
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_CLUSTER_LIST( \
    cluster_list_name,                                    \
    basic_attr_list,                                      \
    identify_attr_list,                                   \
    level_control_cli_attr_list)                          \
zb_zcl_cluster_desc_t cluster_list_name[] =               \
{                                                         \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t), \
    (identify_attr_list),                                 \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_BASIC,                              \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),    \
    (basic_attr_list),                                    \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                      \
    ZB_ZCL_ARRAY_SIZE(level_control_cli_attr_list, zb_zcl_attr_t),  \
    level_control_cli_attr_list,                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  )                                                       \
}

/** @brief Declare simple descriptor for Level control switch device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_SIMPLE_DESC_ZED(                  \
  ep_name, ep_id, in_clust_num, out_clust_num)                                \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name = \
  {                                                                           \
    ep_id,                                                                    \
    ZB_AF_HA_PROFILE_ID,                                                      \
    ZB_LEVEL_CONTROL_SWITCH_DEVICE_ID,                                        \
    ZB_LEVEL_CONTROL_SWITCH_DEVICE_VER,                                       \
    0,                                                                        \
    in_clust_num,                                                             \
    out_clust_num,                                                            \
    {                                                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                             \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                               \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                             \
    }                                                                         \
  }

/** @brief Declare endpoint for Level Control Switch device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
 */
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_EP(ep_name, ep_id, cluster_list)                    \
  ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_SIMPLE_DESC_ZED(ep_name, ep_id,                           \
      ZB_LEVEL_CONTROL_SWITCH_IN_CLUSTER_NUM_ZED, ZB_LEVEL_CONTROL_SWITCH_OUT_CLUSTER_NUM_ZED); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL,                     \
                          ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list, \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                     \
                          0, NULL, /* No reporting ctx */                                       \
                          0, NULL) /* No CVC ctx */

/*!
  @brief Declare application's device context for Level Control Switch device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/

#define ZB_ZCL_DECLARE_LEVEL_CONTROL_SWITCH_CTX(device_ctx, ep_name)     \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)


#endif /* #ifndef _LEVEL_CONTROL_ZED_H_ */
