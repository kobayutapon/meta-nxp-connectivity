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
/* PURPOSE: ZDD device definition
*/

#ifndef BULB_ZDD_H
#define BULB_ZDD_H 1

#include "zboss_api.h"

#define ZB_BULB_ZDD_VER                                 1U
#define ZB_BULB_ZDD_SERVER_CLUSTER_NUM                  3
#define ZB_BULB_ZDD_CLIENT_CLUSTER_NUM                  3
#define ZB_BULB_ZDD_REPORT_ATTR_COUNT                   0
#define ZB_BULB_ZDD_CVC_ATTR_COUNT                      0
#define ZB_BULB_ZDD_ENDPOINT                            1U

/*!
  @brief Declare cluster list for Zigbee Direct Device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param zbd_configuration_attr_list - attribute list for Zigbee Direct Configuration cluster
 */
#define ZB_DECLARE_ZDD_CLUSTER_LIST(                                    \
  cluster_list_name,                                                    \
  basic_attr_list,                                                      \
  identify_attr_list,                                                   \
  zbd_configuration_attr_list)                                          \
  zb_zcl_cluster_desc_t cluster_list_name[] =                           \
  {                                                                     \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
      (basic_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_BASIC_SERVER_ROLE_INIT                          \
    },                                                                  \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
      (identify_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_INIT                       \
    },                                                                  \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION,                           \
      ZB_ZCL_ARRAY_SIZE(zbd_configuration_attr_list, zb_zcl_attr_t),    \
      (zbd_configuration_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION_SERVER_ROLE_INIT           \
    },                                                                  \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_BASIC_CLIENT_ROLE_INIT                          \
    },                                                                  \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_INIT                       \
    },                                                                  \
    {                                                                   \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION,                           \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID,                                        \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION_CLIENT_ROLE_INIT           \
    }                                                                   \
  }

/*!
  @brief Declare simple descriptor for ZDD
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_ZDD_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)            \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                  \
  {                                                                                            \
    ep_id,                                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                                       \
    ZB_HA_TEST_DEVICE_ID,                                                                      \
    ZB_BULB_ZDD_VER,                                                                           \
    0,                                                                                         \
    in_clust_num,                                                                              \
    out_clust_num,                                                                             \
    {                                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                              \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION,                                                  \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                              \
      ZB_ZCL_CLUSTER_ID_DIRECT_CONFIGURATION                                                   \
    }                                                                                          \
  }

/*!
  @brief Declare endpoint for Zigbee Direct Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */
#define ZB_DECLARE_ZDD_EP(ep_name, ep_id, cluster_list)                                     \
  ZB_ZCL_DECLARE_ZDD_SIMPLE_DESC(ep_name, ep_id,                                            \
    ZB_BULB_ZDD_SERVER_CLUSTER_NUM, ZB_BULB_ZDD_CLIENT_CLUSTER_NUM);                                  \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name,                      \
                                     ZB_BULB_ZDD_REPORT_ATTR_COUNT);                             \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info##ep_name,                  \
                                         ZB_BULB_ZDD_CVC_ATTR_COUNT);                            \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                          \
                              0,                                                            \
                              NULL,                                                         \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),       \
                              cluster_list,                                                 \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
                              ZB_BULB_ZDD_REPORT_ATTR_COUNT, reporting_info##ep_name,   \
                              ZB_BULB_ZDD_CVC_ATTR_COUNT, cvc_alarm_info##ep_name)

/**
  @brief Declare application's device context for Zigbee Direct Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_DECLARE_BULB_ZDD_CTX(device_ctx, zdd_ep_name, bulb_ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, zdd_ep_name, bulb_ep_name)

typedef struct zb_bulb_zdd_basic_attr_s
{
  zb_uint8_t zcl_version;
  zb_uint8_t app_version;
  zb_uint8_t stack_version;
  zb_uint8_t hw_version;
  zb_char_t mf_name[32];
  zb_char_t model_id[32];
  zb_char_t date_code[16];
  zb_uint8_t power_source;
  zb_char_t location_id[17];
  zb_uint8_t ph_env;
  zb_char_t sw_build_id[3];
} zb_bulb_zdd_basic_attr_t;

typedef struct zb_bulb_zdd_identify_attr_s
{
  zb_uint16_t identify_time;
} zb_bulb_zdd_identify_attr_t;

typedef struct zb_zbd_configuration_attr_s
{
  zb_zcl_direct_configuration_interface_state_attr_t interface_state;
  zb_zcl_direct_configuration_anonymous_join_timeout_attr_t anonymous_join_timeout;
} zb_zbd_configuration_attr_t;

typedef struct zb_bulb_zdd_ctx_s
{
  zb_bulb_zdd_basic_attr_t basic_attr;
  zb_bulb_zdd_identify_attr_t identify_attr;
  zb_zbd_configuration_attr_t zbd_configuration;
} zb_bulb_zdd_ctx_t;

#endif /* BULB_ZDD_H */
