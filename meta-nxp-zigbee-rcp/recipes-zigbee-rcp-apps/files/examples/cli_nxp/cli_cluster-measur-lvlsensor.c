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
 *                                  CLUSTER Illuminance Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Illuminance Level Sensing
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Temperature Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_temp_measurement_attr_list = ZB_ZCL_TEMP_MEASUREMENT_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temp_measurement_attr_list, 
 *                                               &g_attr_temp_measurement_value, 
 *                                               &g_attr_temp_measurement_min_value, 
 *                                               &g_attr_temp_measurement_max_value, 
 *                                               &g_attr_temp_measurement_tolerance);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_TEMP_MEASUREMENT)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, &g_attr_temp_measurement_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, &g_attr_temp_measurement_min_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, &g_attr_temp_measurement_max_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, &g_attr_temp_measurement_tolerance)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0402[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,    ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&cluster_revision_temp_measurement_attr_list },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,     ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY | ACC_REPORTING, NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.value },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.min_value },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.max_value },
#ifndef ZB_DISABLE_TEMP_MEASUREMENT_TOLERANCE_ID
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.tolerance },
#endif
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0402 = {
  cluster_attr_0402,
  sizeof(cluster_attr_0402)/sizeof(zb_zcl_attr_t),
  zb_zcl_temp_measurement_init_server,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_temp_measurement_init_client,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  dummy_commands_handler,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Pressure Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Flow Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Relative Humidity Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Occupancy Sensing
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Electrical Measurement
 *
 * ----------------------------------------------------------------------------------- */



