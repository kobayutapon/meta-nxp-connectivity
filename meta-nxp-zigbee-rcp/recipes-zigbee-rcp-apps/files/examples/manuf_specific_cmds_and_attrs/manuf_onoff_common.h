/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2024 NXP
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

/* PURPOSE: common declarations for a test
 */

#ifndef MANUF_ONOFF_COMMON_H
#define MANUF_ONOFF_COMMON_H

/* Endpoint is used in ZED */
#define ZB_SWITCH_ENDPOINT 10
/* Endpoint is used in ZC */
#define ZB_OUTPUT_ENDPOINT  5

#define LONG_ADDR_ED {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}
#define LONG_ADDR_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define MANUF_CODE ZB_DEFAULT_MANUFACTURER_CODE

/* Manuf-specific attribute will have the same ID with the OnOff attribute */
#define MANUF_CLST_ON_OFF_ATTR ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID
#define MANUF_CLST_ON_OFF_CMD_SET ZB_ZCL_CMD_ON_OFF_TOGGLE_ID

#define ZB_HA_ON_OFF_OUTPUT_REPORT_ATTR_COUNT_MANUF_TEST 2

#define ZB_HA_ON_OFF_OUTPUT_IN_CLUSTER_NUM_MANUF_TEST  5 /*!< On/Off output IN cluster number */
#define ZB_HA_ON_OFF_OUTPUT_OUT_CLUSTER_NUM_MANUF_TEST 0 /*!< On/Off output OUT cluster number */

#define ZB_HA_ON_OFF_OUTPUT_CLUSTER_NUM_MANUF_TEST                      \
  (ZB_HA_ON_OFF_OUTPUT_IN_CLUSTER_NUM_MANUF_TEST                        \
   + ZB_HA_ON_OFF_OUTPUT_OUT_CLUSTER_NUM_MANUF_TEST)

#define ZB_ZCL_DECLARE_ON_OFF_OUTPUT_SIMPLE_DESC_MANUF_TEST(ep_name, ep_id, in_clust_num, out_clust_num) \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                    \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name =            \
      {                                                                                       \
        ep_id,                                                                                \
        ZB_AF_HA_PROFILE_ID,                                                                  \
        ZB_HA_ON_OFF_OUTPUT_DEVICE_ID,                                                        \
        ZB_HA_DEVICE_VER_ON_OFF_OUTPUT,                                                       \
        0,                                                                                    \
        in_clust_num,                                                                         \
        out_clust_num,                                                                        \
        {                                                                                     \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                                           \
          ZB_ZCL_CLUSTER_ID_SCENES,                                                           \
          ZB_ZCL_CLUSTER_ID_GROUPS                                                            \
        }                                                                                     \
      }

#define ZB_HA_DECLARE_ON_OFF_OUTPUT_EP_MANUF_TEST(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_ON_OFF_OUTPUT_SIMPLE_DESC_MANUF_TEST(                  \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_HA_ON_OFF_OUTPUT_IN_CLUSTER_NUM_MANUF_TEST,                      \
    ZB_HA_ON_OFF_OUTPUT_OUT_CLUSTER_NUM_MANUF_TEST);                    \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                   \
    reporting_info##device_ctx_name,                                    \
    ZB_HA_ON_OFF_OUTPUT_REPORT_ATTR_COUNT_MANUF_TEST);                  \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_AF_HA_PROFILE_ID,                                                \
    0,                                                                  \
    NULL,                                                               \
    ZB_ZCL_ARRAY_SIZE(                                                  \
      cluster_list,                                                     \
      zb_zcl_cluster_desc_t),                                           \
    cluster_list,                                                       \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                   \
    ZB_HA_ON_OFF_OUTPUT_REPORT_ATTR_COUNT_MANUF_TEST,                   \
    reporting_info##device_ctx_name,                                    \
    0, NULL)

#endif /* MANUF_ONOFF_COMMON_H */
