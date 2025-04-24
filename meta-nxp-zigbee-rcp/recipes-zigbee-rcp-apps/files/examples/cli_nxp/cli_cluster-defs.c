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

/* Basic cluster attributes */
typedef struct {
  /* ZCL version attribute */
  zb_uint8_t zcl_version;
  /* Power source attribute */
  zb_uint8_t power_source;
} zb_zcl_general_basic_attr_t;

static zb_zcl_general_basic_attr_t g_general_basic_attr = {
  .zcl_version  = ZB_ZCL_VERSION,
  .power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN,
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

/* Identify cluster attributes */
typedef struct {
  zb_uint16_t identify_time;
} zb_zcl_general_identity_attr_t;

static zb_zcl_general_identity_attr_t g_general_identity_attr = {
  .identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Groups
 *
 * ----------------------------------------------------------------------------------- */

/* Groups cluster attributes */
typedef struct {
  zb_uint8_t name_support;
} zb_zcl_general_groups_attr_t;

static zb_zcl_general_groups_attr_t g_general_groups_attr = {
  .name_support = 0,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Scenes
 *
 * ----------------------------------------------------------------------------------- */

/* Scenes cluster attributes */
typedef struct {
  zb_uint8_t  scene_count;
  zb_uint8_t  current_scene;
  zb_uint16_t current_group;
  zb_uint8_t  scene_valid;
  zb_uint16_t name_support;
} zb_zcl_general_scenes_attr_t;

static zb_zcl_general_scenes_attr_t g_general_scenes_attr = {
  .scene_count   = ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE,
  .current_scene = ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE,
  .current_group = ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE,
  .scene_valid   = ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE,
  .name_support  = ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF
 *
 * ----------------------------------------------------------------------------------- */


/* On/Off cluster attributes */
typedef struct {
  zb_uint8_t on_off;
} zb_zcl_general_on_off_attr_t;

static zb_zcl_general_on_off_attr_t g_general_on_off_attr = {
  .on_off = ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF Switch Configuration
 *
 * ----------------------------------------------------------------------------------- */

/* Switch config cluster attributes */
typedef struct {
  zb_uint8_t type;
  zb_uint8_t actions;
} zb_zcl_general_on_off_switch_attr_t;

static zb_zcl_general_on_off_switch_attr_t g_general_on_off_switch_attr = {
  .type    = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_TOGGLE,
  .actions = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_DEFAULT_VALUE,
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

/* On/Off cluster attributes */
typedef struct {
  zb_int16_t value;
  zb_int16_t min_value;
  zb_int16_t max_value;
  zb_uint16_t tolerance;
} zb_zcl_measur_lvlsensor_temp_measurement_attr_t;

static zb_zcl_measur_lvlsensor_temp_measurement_attr_t g_lvlsensor_temp_measurement_attr = {
  .value = ZB_ZCL_TEMP_MEASUREMENT_VALUE_DEFAULT_VALUE,
  .min_value = ZB_ZCL_TEMP_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE,
  .max_value = ZB_ZCL_TEMP_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE,
#ifndef ZB_DISABLE_TEMP_MEASUREMENT_TOLERANCE_ID
  .tolerance = 0,
#endif
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


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER OTA Upgrade
 *
 * ----------------------------------------------------------------------------------- */

/* Client config */
#define OTA_UPGRADE_DEFAULT_MANUFACTURER       0xFFFF
#define OTA_UPGRADE_DEFAULT_IMAGE_TYPE         0xFFFF

/* OTA Upgrade client cluster attributes */
typedef struct {
  /* UpgradeServerID attribute */
  zb_ieee_addr_t upgrade_server;
  /* FileOffset attribute */
  zb_uint32_t file_offset;
  /* CurrentFileVersion attribute (custom data) */
  zb_uint32_t file_version;
  /* CurrentZigbeeStackVersion attribute */
  zb_uint16_t stack_version;
  /* DownloadedFileVersion attribute */
  zb_uint32_t downloaded_file_ver;
  /* DownloadedZigbeeStackVersion attribute */
  zb_uint16_t downloaded_stack_ver;
  /* ImageUpgradeStatus attribute */
  zb_uint8_t image_status;
  /* Manufacturer ID attribute (custom data) */
  zb_uint16_t manufacturer;
  /* Image Type ID attribute (custom data) */
  zb_uint16_t image_type;
  /* MinimumBlockPeriod attribute */
  zb_uint16_t min_block_period;
  /* Image Stamp attribute */
  zb_uint16_t image_stamp;
} zb_zcl_general_ota_upgrade_attr_t;

static zb_zcl_general_ota_upgrade_attr_t g_general_ota_upgrade_attr = {
  .upgrade_server       = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE,
  .file_offset          = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE,
  .file_version         = 0x00000000, /* Make sure own version is different from the server to get the file */
  .stack_version        = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO,
  .downloaded_file_ver  = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE,
  .downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE,
  .image_status         = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE,
  .manufacturer         = OTA_UPGRADE_DEFAULT_MANUFACTURER,
  .image_type           = OTA_UPGRADE_DEFAULT_IMAGE_TYPE,
  .min_block_period     = 0,
  .image_stamp          = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE,
};

enum zb_zcl_nxp_ota_cmd_e
{
  ZB_ZCL_CMD_OTA_NXP_STATUS      = 0x00,  /* Status OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_GET_FILE    = 0x01,  /* Get file OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_ACCEPT_FILE = 0x02,  /* Accept File OTA NXP command identifier. */
#ifdef ZB_STACK_REGRESSION_TESTING_API
  ZB_ZCL_CMD_OTA_NXP_AUTO        = 0x03,  /* Auto OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_TEMPO       = 0x04,  /* Tempo OTA NXP command identifier. */
#endif
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Thermostat
 *
 * ----------------------------------------------------------------------------------- */

/* Thermostat cluster attributes */
typedef struct {
  zb_int16_t local_temperature;
  zb_int16_t occupied_cooling_setpoint;
  zb_int16_t occupied_heating_setpoint;
  zb_uint8_t control_seq_of_operation;
  zb_uint8_t system_mode;
} zb_zcl_general_thermostat_attr_t;

static zb_zcl_general_thermostat_attr_t g_general_thermostat_attr = {
  .local_temperature = ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_DEFAULT_VALUE,
  .occupied_cooling_setpoint = ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE,
  .occupied_heating_setpoint = ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE,
  .control_seq_of_operation  = ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_DEFAULT_VALUE,
  .system_mode               = ZB_ZCL_THERMOSTAT_CONTROL_SYSTEM_MODE_DEFAULT_VALUE,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER NXP CUSTOM
 *
 * ----------------------------------------------------------------------------------- */

typedef struct {
  /* ZCL version attribute */
  zb_uint8_t zcl_version;
  /* Power source attribute */
  zb_uint8_t power_source;
  /* Test attributes */
  zb_uint64_t attr_64;
  zb_bool_t attr_bool;
  zb_uint32_t attr_32_bm;
  zb_uint8_t attr_u8_ro;
  zb_uint8_t attr_u8_wo;
  zb_uint8_t attr_u8_rw;
  zb_uint8_t attr_u8_ms;
  zb_uint16_t attr_u16;
  zb_uint24_t attr_u24;
  zb_uint32_t attr_u32;
  zb_uint48_t attr_u40;
  zb_uint48_t attr_u48;
  zb_uint64_t attr_u56;
  zb_uint64_t attr_u64;
  zb_int16_t attr_s16;
  zb_uint8_t attr_8_enum;
  zb_uint16_t attr_16_enum;
  zb_uint16_t attr_semi;
  zb_uint32_t attr_single;
  zb_64bit_data_t attr_double;
  zb_uint32_t attr_os;
  zb_uint32_t attr_cs;
  zb_uint32_t g_attr_array;
  zb_uint32_t g_attr_struct;
  zb_uint32_t g_attr_set;
  zb_uint32_t g_attr_bag;
  zb_time_t g_attr_time;
  zb_uint32_t g_attr_date;
  zb_uint32_t g_attr_utc;
  zb_uint16_t g_attr_clid;
  zb_uint16_t g_attr_id;
  zb_ieee_addr_t g_attr_ieee;
  zb_uint8_t g_attr_128;
  zb_uint32_t g_attr_32ar;
  zb_uint8_t g_attr_inval;
} zb_zcl_custom_nxp_attr_t;

static zb_zcl_custom_nxp_attr_t g_custom_nxp_attr = {
  .zcl_version  = ZB_ZCL_VERSION,
  .power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN,
  .attr_64          = 20,
  .attr_bool        = ZB_TRUE,
  .attr_32_bm       = 40,
  .attr_u8_ro       = 50,
  .attr_u8_wo       = 60,
  .attr_u8_rw       = 70,
  .attr_u8_ms       = 80,
  .attr_u16         = 90,
  .attr_u24         = {5,5},
  .attr_u32         = 110,
  .attr_u40         = {5,5},
  .attr_u48         = {5,5},
  .attr_u56         = 135,
  .attr_u64         = 140,
  .attr_s16         = -15,
  .attr_8_enum      = 160,
  .attr_16_enum     = 170,
  .attr_semi        = 180,
  .attr_single      = 190,
  .attr_double      = {5,5,5,5,5,5,5,5},
  .attr_os          = 210,
  .attr_cs          = 220,
  .g_attr_array     = 230,
  .g_attr_struct    = 240,
  .g_attr_set       = 250,
  .g_attr_bag       = 260,
  .g_attr_time      = 270,
  .g_attr_date      = 280,
  .g_attr_utc       = 290,
  .g_attr_clid      = 300,
  .g_attr_id        = 310,
  .g_attr_ieee      = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07},
  .g_attr_128       = 0xFF,
  .g_attr_32ar      = 340,
  .g_attr_inval     = 0,
};

enum zb_zcl_nxp_custom_cmd_e
{
  ZB_ZCL_CMD_NXP_PAYLOAD = 0x00,  /* Payload NXP custom command identifier. */
};
