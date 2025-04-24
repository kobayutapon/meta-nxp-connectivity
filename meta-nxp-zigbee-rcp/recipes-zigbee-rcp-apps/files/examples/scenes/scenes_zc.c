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
/* PURPOSE: Simple output for HA profile
*/

#define ZB_TRACE_FILE_ID 40597
#include "zboss_api.h"

#include "test_output_dev.h"
#include "scenes_test.h"


void test_device_cb(zb_uint8_t param);
zb_uint8_t test_device_scenes_get_entry(zb_uint16_t group_id, zb_uint8_t scene_id);
void test_device_scenes_remove_entries_by_group(zb_uint16_t group_id);
void test_device_scenes_table_init(void);
static void update_scene_valid_attribute(zb_uint8_t param);

#define TEST_DEVICE_SCENES_TABLE_SIZE 3
#define SCENE_VALID_CHECK_INTERVAL ZB_TIME_ONE_SECOND

typedef struct test_device_scenes_fieldset_data_on_off_s {
    zb_bool_t  has_on_off;
    zb_uint8_t on_off;
}test_device_scenes_fieldset_data_on_off_t;

typedef struct test_device_scenes_fieldset_data_level_control_s {
    zb_bool_t  has_current_level;
    zb_uint8_t current_level;
}test_device_scenes_fieldset_data_level_control_t;

typedef struct test_device_scenes_fieldset_data_thermostat_s {
    zb_bool_t  has_thermostat_fieldset;
    zb_int16_t occupied_cooling_setpoint;
    zb_int16_t occupied_heating_setpoint;
    zb_uint8_t system_mode;
}test_device_scenes_fieldset_data_thermostat_t;

typedef struct test_device_scenes_fieldset_data_window_covering_s {
    zb_bool_t  has_current_position_lift_percentage;
    zb_uint8_t current_position_lift_percentage;
    zb_bool_t  has_current_position_tilt_percentage;
    zb_uint8_t current_position_tilt_percentage;
}test_device_scenes_fieldset_data_window_covering_t;

typedef struct test_device_scenes_table_entry_s {
    zb_zcl_scene_table_record_fixed_t                  common;
    test_device_scenes_fieldset_data_on_off_t          on_off_fieldset;
    test_device_scenes_fieldset_data_level_control_t   level_control_fieldset;
    test_device_scenes_fieldset_data_thermostat_t      thermostat_fieldset;
    test_device_scenes_fieldset_data_window_covering_t window_covering_fieldset;
}test_device_scenes_table_entry_t;

typedef struct resp_info_s
{
  zb_zcl_parsed_hdr_t cmd_info;
  zb_zcl_scenes_view_scene_req_t view_scene_req;
  zb_zcl_scenes_get_scene_membership_req_t get_scene_membership_req;
} resp_info_t;

resp_info_t resp_info;

/* Define table for saving scene state */
test_device_scenes_table_entry_t scenes_table[TEST_DEVICE_SCENES_TABLE_SIZE];

static void add_to_test_group(zb_uint8_t param);

/**
 * Global variables definitions
 */
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}; /* IEEE address of the
                                                                              * device */

zb_uint16_t g_dst_addr;
zb_uint8_t g_addr_mode;
zb_uint8_t g_endpoint;

#ifdef ZB_USE_NVRAM
typedef struct nvram_app_dataset_s
{
  zb_uint8_t start_up_on_off;
  zb_uint8_t align[3];
} ZB_PACKED_STRUCT nvram_app_dataset_t;

ZB_ASSERT_IF_NOT_ALIGNED_TO_4(nvram_app_dataset_t);
#endif

/**
 * Declaring attributes for each cluster
 */

/* On/Off cluster attributes */
zb_uint8_t g_attr_on_off = ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;

zb_bool_t g_attr_global_scene_ctrl = ZB_ZCL_ON_OFF_GLOBAL_SCENE_CONTROL_DEFAULT_VALUE;
zb_uint16_t g_attr_on_time = ZB_ZCL_ON_OFF_ON_TIME_DEFAULT_VALUE;
zb_uint16_t g_attr_off_wait_time = ZB_ZCL_ON_OFF_OFF_WAIT_TIME_DEFAULT_VALUE;
zb_uint8_t g_attr_start_up_on_off;

zb_bool_t g_on_off_cluster_delayed_off_state = ZB_FALSE;

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT_WITH_START_UP_ON_OFF(on_off_attr_list,
                                                           &g_attr_on_off,
                                                           &g_attr_global_scene_ctrl,
                                                           &g_attr_on_time,
                                                           &g_attr_off_wait_time,
                                                           &g_attr_start_up_on_off);

/* Basic cluster attributes */
zb_uint8_t g_attr_zcl_version  = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list,
                                 &g_attr_zcl_version,
                                 &g_attr_power_source);

/* Identify cluster attributes */
zb_uint16_t g_attr_identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &g_attr_identify_time);

/* Groups cluster attributes */
zb_uint8_t g_attr_name_support = 0;

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &g_attr_name_support);

/* Scenes cluster attributes */
zb_uint8_t g_attr_scenes_scene_count = ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE;
zb_uint8_t g_attr_scenes_current_scene =
    ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE;
zb_uint16_t g_attr_scenes_current_group =
    ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE;
zb_uint8_t g_attr_scenes_scene_valid = ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE;
zb_uint8_t g_attr_scenes_name_support =
    ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE;

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list, &g_attr_scenes_scene_count,
    &g_attr_scenes_current_scene, &g_attr_scenes_current_group,
    &g_attr_scenes_scene_valid, &g_attr_scenes_name_support);

/* Declare cluster list for a device */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CLUSTER_LIST(
    on_off_output_clusters,
    on_off_attr_list,
    basic_attr_list,
    identify_attr_list,
    groups_attr_list,
    scenes_attr_list);

/* Declare endpoint */
ZB_HA_DECLARE_ON_OFF_OUTPUT_EP(
    on_off_output_ep,
    HA_OUTPUT_ENDPOINT,
    on_off_output_clusters);

/* Declare application's device context for single-endpoint device */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(on_off_output_ctx, on_off_output_ep);

void apply_start_up_on_off(void)
{
  switch(g_attr_start_up_on_off)
  {
    case ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_OFF:
    {
      g_attr_on_off = ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_OFF;
      break;
    }
    case ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_ON:
    {
      g_attr_on_off = ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_ON;
      break;
    }
    case ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_TOGGLE:
    {
      g_attr_on_off = (g_attr_on_off == ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_OFF) ? ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_ON : ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_OFF;
      break;
    }
    case ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_PREVIOUS:
    {
      break;
    }
    default:
    {
      break;
    }
  }
}

MAIN()
{
  ARGV_UNUSED;

  /* Global ZBOSS initialization */
  ZB_INIT("scenes_zc");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_zc_addr);
  zb_set_network_coordinator_role(1l<<21);
  zb_set_nvram_erase_at_start(ZB_FALSE);

  /* Set Device user application callback */
  ZB_ZCL_REGISTER_DEVICE_CB(test_device_cb);
  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&on_off_output_ctx);
  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(HA_OUTPUT_ENDPOINT, zcl_specific_cluster_cmd_handler);

  test_device_scenes_table_init();

  /* Initiate the stack start with starting the commissioning */
  if (zboss_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "ERROR zdo_dev_start failed", (FMT__0));
  }
  else
  {
    /* Call the main loop */
    zboss_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}

zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;
  zb_bufid_t zcl_cmd_buf = param;
  zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

  TRACE_MSG(TRACE_APP1, "> zcl_specific_cluster_cmd_handler %i", (FMT__H, param));
  ZB_ZCL_DEBUG_DUMP_HEADER(cmd_info);

  if (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)
  {
    TRACE_MSG(
        TRACE_ERROR,
        "Unsupported \"from server\" command direction",
        (FMT__0));
  }

  return ret;
}

void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_t sig = zb_get_app_signal(param, &sg_p);

  TRACE_MSG(TRACE_APP1, ">>zboss_signal_handler: status %hd signal %hd",
            (FMT__H_H, ZB_GET_APP_SIGNAL_STATUS(param), sig));

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch(sig)
    {
#ifdef DEBUG
      case ZB_DEBUG_SIGNAL_TCLK_READY:
      {
        zb_debug_signal_tclk_ready_params_t *params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_debug_signal_tclk_ready_params_t);
        zb_debug_broadcast_aps_key(params->long_addr);
      }
      break;
#endif
      case ZB_ZDO_SIGNAL_DEFAULT_START:
      case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        TRACE_MSG(TRACE_APP1, "Device STARTED OK", (FMT__0));
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        break;

      case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_DEVICE_ANNCE", (FMT__0));
        add_to_test_group(param);
        ZB_SCHEDULE_APP_CALLBACK(update_scene_valid_attribute, 0);
        param = 0;
        break;

      case ZB_BDB_SIGNAL_FINDING_AND_BINDING_INITIATOR_FINISHED:
        TRACE_MSG(TRACE_APP1, "Finding&binding done", (FMT__0));
        break;

      default:
        TRACE_MSG(TRACE_APP1, "Unknown signal %hd", (FMT__H, sig));
    }
  }
  else if (sig == ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY)
  {
    TRACE_MSG(TRACE_APP1, "Production config is not present or invalid", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR,
              "Device started FAILED status %d",
              (FMT__D, ZB_GET_APP_SIGNAL_STATUS(param)));
  }

  if (param)
  {
    zb_buf_free(param);
  }

  TRACE_MSG(TRACE_APP1, "<<zboss_signal_handler", (FMT__0));
}

static void add_to_test_group(zb_uint8_t param)
{
  zb_bufid_t buf = param;
  zb_apsme_add_group_req_t *aps_req;

  TRACE_MSG(TRACE_APP1, "> add_to_test_group buf %p", (FMT__P, buf));

  zb_buf_reuse(buf);
  aps_req = ZB_BUF_GET_PARAM(buf, zb_apsme_add_group_req_t);
  ZB_BZERO(aps_req, sizeof(*aps_req));
  aps_req->group_address = TEST_GROUP_ID;
  aps_req->endpoint = HA_OUTPUT_ENDPOINT;

  zb_zdo_add_group_req(buf);

  TRACE_MSG(TRACE_APP1, "< add_to_test_group", (FMT__0));
}

void send_view_scene_resp(zb_uint8_t param, zb_uint16_t idx)
{
  zb_bufid_t buf = param;
  zb_uint8_t *payload_ptr;
  zb_uint8_t view_scene_status = ZB_ZCL_STATUS_NOT_FOUND;

  TRACE_MSG(TRACE_APP1, ">> send_view_scene_resp param %hd idx %d", (FMT__H_D, param, idx));

  if (idx != 0xFF &&
      scenes_table[idx].common.group_id != ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
  {
    /* Scene found */
    view_scene_status = ZB_ZCL_STATUS_SUCCESS;
  }
  else if (!zb_aps_is_endpoint_in_group(
             resp_info.view_scene_req.group_id,
             ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).dst_endpoint))
  {
    /* Not in the group */
    view_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
  }

  ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES(
    buf,
    payload_ptr,
    resp_info.cmd_info.seq_number,
    view_scene_status,
    resp_info.view_scene_req.group_id,
    resp_info.view_scene_req.scene_id);

  if (view_scene_status == ZB_ZCL_STATUS_SUCCESS)
  {
    ZB_ZCL_SCENES_ADD_TRANSITION_TIME_VIEW_SCENE_RES(
      payload_ptr,
      scenes_table[idx].common.transition_time);

    ZB_ZCL_SCENES_ADD_SCENE_NAME_VIEW_SCENE_RES(
      payload_ptr,
      scenes_table[idx].common.scene_name);

    /* Extension set: Cluster ID = On/Off */
    ZB_ZCL_PACKET_PUT_DATA16_VAL(payload_ptr, ZB_ZCL_CLUSTER_ID_ON_OFF);

    /* Extension set: Fieldset length = 1 */
    ZB_ZCL_PACKET_PUT_DATA8(payload_ptr, 1);

    /* Extension set: On/Off state */
    ZB_ZCL_PACKET_PUT_DATA8(payload_ptr, scenes_table[idx].on_off_fieldset.on_off);
  }

  ZB_ZCL_SCENES_SEND_VIEW_SCENE_RES(
    buf,
    payload_ptr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).source.u.short_addr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).src_endpoint,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).dst_endpoint,
    resp_info.cmd_info.profile_id,
    NULL);

  TRACE_MSG(TRACE_APP1, "<< send_view_scene_resp", (FMT__0));
}

void send_get_scene_membership_resp(zb_uint8_t param)
{
  zb_bufid_t buf = param;
  zb_uint8_t *payload_ptr;
  zb_uint8_t *capacity_ptr;
  zb_uint8_t *scene_count_ptr;

  TRACE_MSG(TRACE_APP1, ">> send_get_scene_membership_resp param %hd", (FMT__H, param));

  if (!zb_aps_is_endpoint_in_group(
        resp_info.get_scene_membership_req.group_id,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).dst_endpoint))
  {
    /* Not in the group */
    ZB_ZCL_SCENES_INIT_GET_SCENE_MEMBERSHIP_RES(
      buf,
      payload_ptr,
      resp_info.cmd_info.seq_number,
      capacity_ptr,
      ZB_ZCL_STATUS_INVALID_FIELD,
      ZB_ZCL_SCENES_CAPACITY_UNKNOWN,
      resp_info.get_scene_membership_req.group_id);
  }
  else
  {
    zb_uint8_t i = 0;

    ZB_ZCL_SCENES_INIT_GET_SCENE_MEMBERSHIP_RES(
      buf,
      payload_ptr,
      resp_info.cmd_info.seq_number,
      capacity_ptr,
      ZB_ZCL_STATUS_SUCCESS,
      0,
      resp_info.get_scene_membership_req.group_id);

    scene_count_ptr = payload_ptr;
    ZB_ZCL_SCENES_ADD_SCENE_COUNT_GET_SCENE_MEMBERSHIP_RES(payload_ptr, 0);

    while (i < TEST_DEVICE_SCENES_TABLE_SIZE)
    {
      if (scenes_table[i].common.group_id == resp_info.get_scene_membership_req.group_id)
      {
        /* Add to payload */
        TRACE_MSG(TRACE_APP1, "add scene_id %hd", (FMT__H, scenes_table[i].common.scene_id));
        ++(*scene_count_ptr);
        ZB_ZCL_SCENES_ADD_SCENE_ID_GET_SCENE_MEMBERSHIP_RES(
          payload_ptr,
          scenes_table[i].common.scene_id);
      }
      else if (scenes_table[i].common.group_id == ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
      {
        TRACE_MSG(TRACE_APP1, "add capacity num", (FMT__0));
        ++(*capacity_ptr);
      }
      ++i;
    }
  }

  ZB_ZCL_SCENES_SEND_GET_SCENE_MEMBERSHIP_RES(
    buf,
    payload_ptr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).source.u.short_addr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).src_endpoint,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(&resp_info.cmd_info).dst_endpoint,
    resp_info.cmd_info.profile_id,
    NULL);

  TRACE_MSG(TRACE_APP1, "<< send_get_scene_membership_resp", (FMT__0));
}

static zb_ret_t get_scene_valid_value(zb_bool_t *scene_valid)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_SCENES,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID);
  if (attr_desc != NULL)
  {
    *scene_valid = (zb_bool_t)ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_current_scene_value(zb_uint8_t *scene_id)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_SCENES,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID);
  if (attr_desc != NULL)
  {
    *scene_id = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_current_group_value(zb_uint16_t *group_id)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_SCENES,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID);
  if (attr_desc != NULL)
  {
    *group_id = ZB_ZCL_GET_ATTRIBUTE_VAL_16(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_on_off_value(zb_uint8_t *on_off)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_ON_OFF,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID);
  if (attr_desc != NULL)
  {
    *on_off = (zb_bool_t)ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_current_level_value(zb_uint8_t *current_level)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID);
  if (attr_desc != NULL)
  {
    *current_level = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_occupied_cooling_setpoint(zb_int16_t *setpoint)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID);
  if (attr_desc != NULL)
  {
    *setpoint = ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_occupied_heating_setpoint(zb_int16_t *setpoint)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID);
  if (attr_desc != NULL)
  {
    *setpoint = ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_system_mode(zb_uint8_t *mode)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID);
  if (attr_desc != NULL)
  {
    *mode = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_current_lift_value(zb_uint8_t *percentage)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID);
  if (attr_desc != NULL)
  {
    *percentage = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t get_current_tilt_value(zb_uint8_t *percentage)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(HA_OUTPUT_ENDPOINT, ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
                                                    ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_TILT_PERCENTAGE_ID);
  if (attr_desc != NULL)
  {
    *percentage = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_ret_t set_scene_valid_value(zb_bool_t scene_valid)
{
  zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(
    HA_OUTPUT_ENDPOINT,
    ZB_ZCL_CLUSTER_ID_SCENES,
    ZB_ZCL_CLUSTER_SERVER_ROLE,
    ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID);

  if (attr_desc != NULL)
  {
    ZB_ZCL_SET_DIRECTLY_ATTR_VAL8(attr_desc, scene_valid);
    return RET_OK;
  }
  return RET_ERROR;
}

static zb_bool_t check_cluster(zb_uint16_t cluster_id)
{
  return (get_endpoint_by_cluster(cluster_id, ZB_ZCL_CLUSTER_SERVER_ROLE) == HA_OUTPUT_ENDPOINT) ? ZB_TRUE : ZB_FALSE;
}

static zb_bool_t add_fieldset(zb_zcl_scenes_fieldset_common_t *fieldset, test_device_scenes_table_entry_t *entry)
{
  zb_uint8_t *fs_data_ptr;

  if (fieldset->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
      fieldset->fieldset_length >= 1 &&
      check_cluster(ZB_ZCL_CLUSTER_ID_ON_OFF))
  {
    fs_data_ptr = (zb_uint8_t *)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

    entry->on_off_fieldset.has_on_off = ZB_TRUE;
    entry->on_off_fieldset.on_off = *fs_data_ptr;

    TRACE_MSG(TRACE_APP1, "Add fieldset: cluster_id=0x%x, length=%hd, on_off=%hd",
             (FMT__D_H_H,  fieldset->cluster_id, fieldset->fieldset_length, entry->on_off_fieldset.on_off));

    return ZB_TRUE;
  }
  if (fieldset->cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL &&
      fieldset->fieldset_length >= 1 &&
      check_cluster(ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL))
  {
    fs_data_ptr = (zb_uint8_t *)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

    entry->level_control_fieldset.has_current_level = ZB_TRUE;
    entry->level_control_fieldset.current_level = *fs_data_ptr;

    TRACE_MSG(TRACE_APP1, "Add fieldset: cluster_id=0x%x, length=%hd, current_level=%hd",
              (FMT__D_H_H,  fieldset->cluster_id, fieldset->fieldset_length, entry->level_control_fieldset.current_level));

    return ZB_TRUE;
  }
  if (fieldset->cluster_id == ZB_ZCL_CLUSTER_ID_THERMOSTAT &&
      fieldset->fieldset_length >= 5 &&
      check_cluster(ZB_ZCL_CLUSTER_ID_THERMOSTAT))
  {
    fs_data_ptr = (zb_uint8_t *)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

    entry->thermostat_fieldset.has_thermostat_fieldset = ZB_TRUE;
    ZB_ZCL_HTOLE16_INPLACE(fs_data_ptr);
    ZB_MEMCPY(&entry->thermostat_fieldset.occupied_cooling_setpoint, fs_data_ptr, sizeof(zb_int16_t));
    fs_data_ptr += 2;
    ZB_ZCL_HTOLE16_INPLACE(fs_data_ptr);
    ZB_MEMCPY(&entry->thermostat_fieldset.occupied_heating_setpoint, fs_data_ptr, sizeof(zb_int16_t));
    fs_data_ptr += 2;
    entry->thermostat_fieldset.system_mode = *fs_data_ptr;

    TRACE_MSG(TRACE_APP1, "Add fieldset: cluster_id=0x%x, length=%hd, occupied_cooling_setpoint=%d, occupied_heating_setpoint=%d system_mode=%hd",
              (FMT__D_H_D_D_H,  fieldset->cluster_id, fieldset->fieldset_length,
               entry->thermostat_fieldset.occupied_cooling_setpoint, entry->thermostat_fieldset.occupied_heating_setpoint, entry->thermostat_fieldset.system_mode));

    return ZB_TRUE;
  }
  if (fieldset->cluster_id == ZB_ZCL_CLUSTER_ID_WINDOW_COVERING &&
      fieldset->fieldset_length >= 1 &&
      check_cluster(ZB_ZCL_CLUSTER_ID_WINDOW_COVERING))
  {
    fs_data_ptr = (zb_uint8_t *)fieldset + sizeof(zb_zcl_scenes_fieldset_common_t);

    entry->window_covering_fieldset.has_current_position_lift_percentage = ZB_TRUE;
    entry->window_covering_fieldset.current_position_lift_percentage = *fs_data_ptr;

    TRACE_MSG(TRACE_APP1, "Add fieldset: cluster_id=0x%x, length=%hd, current_position_lift_percentage=%hd",
              (FMT__D_H_H,  fieldset->cluster_id, fieldset->fieldset_length, entry->window_covering_fieldset.current_position_lift_percentage));

    if (fieldset->fieldset_length >= 2)
    {
      entry->window_covering_fieldset.has_current_position_tilt_percentage = ZB_TRUE;
      entry->window_covering_fieldset.current_position_tilt_percentage = *(fs_data_ptr + 1);
      TRACE_MSG(TRACE_APP1, "Add fieldset: cluster_id=0x%x, length=%hd, current_position_tilt_percentage=%hd",
                (FMT__D_H_H,  fieldset->cluster_id, fieldset->fieldset_length, entry->window_covering_fieldset.current_position_tilt_percentage));
    }

    return ZB_TRUE;
  }

  TRACE_MSG(TRACE_APP1, "Ignore fieldset: cluster_id=0x%x, length=%hd", (FMT__D_H,  fieldset->cluster_id, fieldset->fieldset_length));

  return ZB_FALSE;
}

static void test_device_save_scene(test_device_scenes_table_entry_t *entry)
{
  if (check_cluster(ZB_ZCL_CLUSTER_ID_ON_OFF) && get_on_off_value(&entry->on_off_fieldset.on_off) == RET_OK)
  {
    TRACE_MSG(TRACE_APP1, "Save On_Off state inside scene table", (FMT__0));
    entry->on_off_fieldset.has_on_off = ZB_TRUE;
  }
  if (check_cluster(ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && get_current_level_value(&entry->level_control_fieldset.current_level) == RET_OK)
  {
    TRACE_MSG(TRACE_APP1, "Save Level Control state inside scene table", (FMT__0));
    entry->level_control_fieldset.has_current_level = ZB_TRUE;
  }
  if (check_cluster(ZB_ZCL_CLUSTER_ID_THERMOSTAT) &&
      get_occupied_cooling_setpoint(&entry->thermostat_fieldset.occupied_cooling_setpoint) == RET_OK &&
      get_occupied_heating_setpoint(&entry->thermostat_fieldset.occupied_heating_setpoint) == RET_OK &&
      get_system_mode(&entry->thermostat_fieldset.system_mode) == RET_OK)
  {
    TRACE_MSG(TRACE_APP1, "Save Thermostat state inside scene table", (FMT__0));
    entry->thermostat_fieldset.has_thermostat_fieldset = ZB_TRUE;
  }
  if (check_cluster(ZB_ZCL_CLUSTER_ID_WINDOW_COVERING))
  {
    TRACE_MSG(TRACE_APP1, "Save Window Covering state inside scene table", (FMT__0));
    if (get_current_lift_value(&entry->window_covering_fieldset.current_position_lift_percentage) == RET_OK)
    {
      entry->window_covering_fieldset.has_current_position_lift_percentage = ZB_TRUE;
    }
    if (get_current_tilt_value(&entry->window_covering_fieldset.current_position_tilt_percentage) == RET_OK)
    {
      entry->window_covering_fieldset.has_current_position_tilt_percentage = ZB_TRUE;
    }
  }
}

static zb_ret_t recall_scene(test_device_scenes_table_entry_t *entry)
{
  zb_bufid_t buf = zb_buf_get_any();
  zb_zcl_attr_t *attr_desc;
  zb_ret_t result;

  if (entry->on_off_fieldset.has_on_off)
  {
    TRACE_MSG(TRACE_APP1, "Recall On_Off state", (FMT__0));

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_ON_OFF,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_ON_OFF,
      attr_desc,
      &entry->on_off_fieldset.on_off,
      result);
  }
  if (entry->level_control_fieldset.has_current_level)
  {
    TRACE_MSG(TRACE_APP1, "Recall Level Control state", (FMT__0));

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      attr_desc,
      &entry->level_control_fieldset.current_level,
      result);
  }
  if (entry->thermostat_fieldset.has_thermostat_fieldset)
  {
    TRACE_MSG(TRACE_APP1, "Recall Thermostat state", (FMT__0));

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      attr_desc,
      &entry->thermostat_fieldset.occupied_cooling_setpoint,
      result);

    zb_buf_reuse(buf);

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      attr_desc,
      &entry->thermostat_fieldset.occupied_heating_setpoint,
      result);

   zb_buf_reuse(buf);

   attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      attr_desc,
      &entry->thermostat_fieldset.system_mode,
      result);

  }
  if (entry->window_covering_fieldset.has_current_position_lift_percentage)
  {
    TRACE_MSG(TRACE_APP1, "Recall Window Covering Lift state", (FMT__0));

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
      attr_desc,
      &entry->window_covering_fieldset.current_position_lift_percentage,
      result);
  }
  if (entry->window_covering_fieldset.has_current_position_tilt_percentage)
  {
    TRACE_MSG(TRACE_APP1, "Recall Window Covering Tilt state", (FMT__0));

    attr_desc = zb_zcl_get_attr_desc_a(
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_TILT_PERCENTAGE_ID);

    ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(
      buf,
      HA_OUTPUT_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
      attr_desc,
      &entry->window_covering_fieldset.current_position_tilt_percentage,
      result);
  }
  zb_buf_free(buf);
  return result;
}

static void update_scene_valid_attribute(zb_uint8_t param)
{
  zb_bool_t scene_valid = ZB_FALSE;
  zb_uint8_t scene_id = 0xFF;
  zb_uint16_t group_id = ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD;

  ZVUNUSED(param);

  TRACE_MSG(TRACE_APP1, "Update Scene Valid attribute", (FMT__0));

  if (get_scene_valid_value(&scene_valid) == RET_OK &&
      get_current_scene_value(&scene_id) == RET_OK &&
      get_current_group_value(&group_id) == RET_OK &&
      scene_valid == ZB_TRUE)
  {
    zb_uint8_t idx = test_device_scenes_get_entry(group_id, scene_id);
    if (group_id == ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD ||
        scene_id < TEST_DEVICE_SCENES_TABLE_SIZE ||
        idx == (zb_uint8_t)ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
    {
      (void)set_scene_valid_value(ZB_FALSE);
      return;
    }

    if (scenes_table[idx].on_off_fieldset.has_on_off)
    {
      zb_uint8_t on_off;

      if (get_on_off_value(&on_off) == RET_OK
          && on_off != scenes_table[idx].on_off_fieldset.on_off)
      {
        (void)set_scene_valid_value(ZB_FALSE);
        return;
      }
    }

    if (scenes_table[idx].level_control_fieldset.has_current_level)
    {
      zb_uint8_t current_level;

      if (get_current_level_value(&current_level) == RET_OK
          && current_level != scenes_table[idx].level_control_fieldset.current_level)
      {
        (void)set_scene_valid_value(ZB_FALSE);
        return;
      }
    }

    if (scenes_table[idx].thermostat_fieldset.has_thermostat_fieldset)
    {
      zb_int16_t occupied_cooling_setpoint;
      zb_int16_t occupied_heating_setpoint;
      zb_uint8_t system_mode;

      if (get_occupied_cooling_setpoint(&occupied_cooling_setpoint) == RET_OK
          && get_occupied_heating_setpoint(&occupied_heating_setpoint) == RET_OK
          && get_system_mode(&system_mode) == RET_OK
          && (occupied_cooling_setpoint !=
              scenes_table[idx].thermostat_fieldset.occupied_cooling_setpoint
              || occupied_heating_setpoint !=
              scenes_table[idx].thermostat_fieldset.occupied_heating_setpoint
              || system_mode != scenes_table[idx].thermostat_fieldset.system_mode))
        {
          (void)set_scene_valid_value(ZB_FALSE);
          return;
        }
    }

    if (scenes_table[idx].window_covering_fieldset.has_current_position_lift_percentage)
    {
      zb_uint8_t lift;

      if (get_current_lift_value(&lift) == RET_OK
          && lift != scenes_table[idx].window_covering_fieldset.current_position_lift_percentage)
      {
        (void)set_scene_valid_value(ZB_FALSE);
        return;
      }
    }

    if (scenes_table[idx].window_covering_fieldset.has_current_position_tilt_percentage)
    {
      zb_uint8_t tilt;

      if (get_current_tilt_value(&tilt) == RET_OK
          && tilt != scenes_table[idx].window_covering_fieldset.current_position_tilt_percentage)
      {
        (void)set_scene_valid_value(ZB_FALSE);
        return;
      }
    }
  }
  ZB_SCHEDULE_APP_ALARM(update_scene_valid_attribute, 0, SCENE_VALID_CHECK_INTERVAL);
}

void test_device_cb(zb_uint8_t param)
{
  zb_bufid_t buffer = param;
  zb_zcl_device_callback_param_t *device_cb_param =
    ZB_BUF_GET_PARAM(buffer, zb_zcl_device_callback_param_t);
  TRACE_MSG(TRACE_APP1, "> test_device_cb param %hd id %hd", (FMT__H_H,
      param, device_cb_param->device_cb_id));

  device_cb_param->status = RET_OK;
  switch (device_cb_param->device_cb_id)
  {
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
      TRACE_MSG(TRACE_APP1, "on/off setting to %hd", (FMT__H, device_cb_param->cb_param.set_attr_value_param.values.data8));
      break;

      /* >>>> Scenes */
    case ZB_ZCL_SCENES_ADD_SCENE_CB_ID:
    {
      const zb_zcl_scenes_add_scene_req_t *add_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_add_scene_req_t);
      zb_bufid_t buf = param;
      zb_uint8_t idx = 0xFF;
      zb_uint8_t *add_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_ADD_SCENE_CB_ID: group_id 0x%x scene_id 0x%hd transition_time %d", (FMT__D_H_D, add_scene_req->group_id, add_scene_req->scene_id, add_scene_req->transition_time));

      *add_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
      idx = test_device_scenes_get_entry(add_scene_req->group_id, add_scene_req->scene_id);

      if (idx != 0xFF)
      {
        zb_zcl_scenes_fieldset_common_t *fieldset;
        zb_uint8_t fs_content_length;
        zb_bool_t empty_entry = ZB_TRUE;

        if (scenes_table[idx].common.group_id != ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
        {
          /* Indicate that we overwriting existing record */
          device_cb_param->status = RET_ALREADY_EXISTS;
        }

        ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_NEXT_FIELDSET_DESC(buf,
                                                           fieldset,
                                                           fs_content_length);

        while (fieldset)
        {
          if (add_fieldset(fieldset, &scenes_table[idx]) == ZB_TRUE)
          {
            empty_entry = ZB_FALSE;
          }
          ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_NEXT_FIELDSET_DESC(buf,
                                                             fieldset,
                                                             fs_content_length);

        }
        if (empty_entry == ZB_FALSE)
        {
          /* Store this scene */
          scenes_table[idx].common.group_id = add_scene_req->group_id;
          scenes_table[idx].common.scene_id = add_scene_req->scene_id;
          scenes_table[idx].common.transition_time = add_scene_req->transition_time;
          TRACE_MSG(TRACE_APP1, "onoff_state %hd", (FMT__H, scenes_table[idx].on_off_fieldset.on_off));
          *add_scene_status = ZB_ZCL_STATUS_SUCCESS;
        }
      }
      else
      {
        *add_scene_status = ZB_ZCL_STATUS_INSUFF_SPACE;
      }
    }
    break;
    case ZB_ZCL_SCENES_VIEW_SCENE_CB_ID:
    {
      const zb_zcl_scenes_view_scene_req_t *view_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_view_scene_req_t);
      const zb_zcl_parsed_hdr_t *in_cmd_info = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);
      zb_uint8_t idx = 0xFF;

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_VIEW_SCENE_CB_ID: group_id 0x%x scene_id 0x%hd", (FMT__D_H, view_scene_req->group_id, view_scene_req->scene_id));

      idx = test_device_scenes_get_entry(view_scene_req->group_id, view_scene_req->scene_id);

      /* Send View Scene Response */
      ZB_MEMCPY(&resp_info.cmd_info, in_cmd_info, sizeof(zb_zcl_parsed_hdr_t));
      ZB_MEMCPY(&resp_info.view_scene_req, view_scene_req, sizeof(zb_zcl_scenes_view_scene_req_t));
      zb_buf_get_out_delayed_ext(send_view_scene_resp, idx, 0);
    }
    break;
    case ZB_ZCL_SCENES_REMOVE_SCENE_CB_ID:
    {
      const zb_zcl_scenes_remove_scene_req_t *remove_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_scene_req_t);
      zb_uint8_t idx = 0xFF;
      zb_uint8_t *remove_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      const zb_zcl_parsed_hdr_t *in_cmd_info = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_REMOVE_SCENE_CB_ID: group_id 0x%x scene_id 0x%hd", (FMT__D_H, remove_scene_req->group_id, remove_scene_req->scene_id));

      *remove_scene_status = ZB_ZCL_STATUS_NOT_FOUND;
      idx = test_device_scenes_get_entry(remove_scene_req->group_id, remove_scene_req->scene_id);

      if (idx != 0xFF &&
          scenes_table[idx].common.group_id != ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
      {
        /* Remove this entry */
        ZB_MEMSET(&scenes_table[idx], 0, sizeof(scenes_table[idx]));
        scenes_table[idx].common.group_id = ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD;
        TRACE_MSG(TRACE_APP1, "removing scene: entry idx %hd", (FMT__H, idx));
        *remove_scene_status = ZB_ZCL_STATUS_SUCCESS;
      }
      else if (!zb_aps_is_endpoint_in_group(
                 remove_scene_req->group_id,
                 ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).dst_endpoint))
      {
        *remove_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
      }
    }
    break;
    case ZB_ZCL_SCENES_REMOVE_ALL_SCENES_CB_ID:
    {
      const zb_zcl_scenes_remove_all_scenes_req_t *remove_all_scenes_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_all_scenes_req_t);
      zb_uint8_t *remove_all_scenes_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      const zb_zcl_parsed_hdr_t *in_cmd_info = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_REMOVE_ALL_SCENES_CB_ID: group_id 0x%x", (FMT__D, remove_all_scenes_req->group_id));

      if (!zb_aps_is_endpoint_in_group(
                 remove_all_scenes_req->group_id,
                 ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).dst_endpoint))
      {
        *remove_all_scenes_status = ZB_ZCL_STATUS_INVALID_FIELD;
      }
      else
      {
        test_device_scenes_remove_entries_by_group(remove_all_scenes_req->group_id);
        *remove_all_scenes_status = ZB_ZCL_STATUS_SUCCESS;
      }
    }
    break;
    case ZB_ZCL_SCENES_STORE_SCENE_CB_ID:
    {
      const zb_zcl_scenes_store_scene_req_t *store_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_store_scene_req_t);
      zb_uint8_t idx = 0xFF;
      zb_uint8_t *store_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);
      const zb_zcl_parsed_hdr_t *in_cmd_info = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_STORE_SCENE_CB_ID: group_id 0x%x scene_id 0x%hd", (FMT__D_H, store_scene_req->group_id, store_scene_req->scene_id));

      if (!zb_aps_is_endpoint_in_group(
                 store_scene_req->group_id,
                 ZB_ZCL_PARSED_HDR_SHORT_DATA(in_cmd_info).dst_endpoint))
      {
        *store_scene_status = ZB_ZCL_STATUS_INVALID_FIELD;
      }
      else
      {
        idx = test_device_scenes_get_entry(store_scene_req->group_id, store_scene_req->scene_id);

        if (idx != 0xFF)
        {
          if (scenes_table[idx].common.group_id != ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
          {
            /* Update existing entry with current On/Off state */
            device_cb_param->status = RET_ALREADY_EXISTS;
            TRACE_MSG(TRACE_APP1, "update scene: entry idx %hd onoff_state %hd", (FMT__H_H, idx, scenes_table[idx].on_off_fieldset.on_off));
          }
          else
          {
            /* Create new entry with empty name and 0 transition time */
            scenes_table[idx].common.group_id = store_scene_req->group_id;
            scenes_table[idx].common.scene_id = store_scene_req->scene_id;
            scenes_table[idx].common.transition_time = 0;
            TRACE_MSG(TRACE_APP1, "store new scene: entry idx %hd onoff_state %hd", (FMT__H_H, idx, scenes_table[idx].on_off_fieldset.on_off));
          }
          test_device_save_scene(&scenes_table[idx]);
          *store_scene_status = ZB_ZCL_STATUS_SUCCESS;
        }
        else
        {
          *store_scene_status = ZB_ZCL_STATUS_INSUFF_SPACE;
        }
      }
    }
    break;
    case ZB_ZCL_SCENES_RECALL_SCENE_CB_ID:
    {
      const zb_zcl_scenes_recall_scene_req_t *recall_scene_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_recall_scene_req_t);
      zb_uint8_t idx = 0xFF;
      zb_uint8_t *recall_scene_status = ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(param, zb_uint8_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_RECALL_SCENE_CB_ID: group_id 0x%x scene_id 0x%hd", (FMT__D_H, recall_scene_req->group_id, recall_scene_req->scene_id));

      idx = test_device_scenes_get_entry(recall_scene_req->group_id, recall_scene_req->scene_id);

      if (idx != 0xFF &&
          scenes_table[idx].common.group_id != ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
      {
        /* Recall this entry */
        TRACE_MSG(TRACE_APP1, "recall scene: entry idx %hd onoff_state %hd transition_time 0x%x", (FMT__H_H_D, idx, scenes_table[idx].on_off_fieldset.on_off, scenes_table[idx].common.transition_time));
        recall_scene(&scenes_table[idx]);
        *recall_scene_status = ZB_ZCL_STATUS_SUCCESS;
      }
      else
      {
        *recall_scene_status = ZB_ZCL_STATUS_NOT_FOUND;
      }
    }
    break;
    case ZB_ZCL_SCENES_GET_SCENE_MEMBERSHIP_CB_ID:
    {
      const zb_zcl_scenes_get_scene_membership_req_t *get_scene_membership_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_get_scene_membership_req_t);
      const zb_zcl_parsed_hdr_t *in_cmd_info = ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(param);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_GET_SCENE_MEMBERSHIP_CB_ID: group_id 0x%xd", (FMT__D, get_scene_membership_req->group_id));

      /* Send Get Scene Membership Response */
      ZB_MEMCPY(&resp_info.cmd_info, in_cmd_info, sizeof(zb_zcl_parsed_hdr_t));
      ZB_MEMCPY(&resp_info.get_scene_membership_req, get_scene_membership_req, sizeof(zb_zcl_scenes_get_scene_membership_req_t));
      zb_buf_get_out_delayed(send_get_scene_membership_resp);
    }
    break;

    case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_CB_ID:
    {
      const zb_zcl_scenes_remove_all_scenes_req_t *remove_all_scenes_req = ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(param, zb_zcl_scenes_remove_all_scenes_req_t);

      TRACE_MSG(TRACE_APP1, "ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_CB_ID: group_id 0x%x", (FMT__D, remove_all_scenes_req->group_id));

      /* Have only one endpoint */
      test_device_scenes_remove_entries_by_group(remove_all_scenes_req->group_id);
    }
    break;
    case ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_ALL_GROUPS_CB_ID:
    {
      test_device_scenes_table_init();
    }
    break;
      /* <<<< Scenes */
    default:
      device_cb_param->status = RET_NOT_IMPLEMENTED; /* Use not implemented instead of error to have in ZCL Cmd Resp Status unsup. cluster instead of hw failure */
      break;
  }
  TRACE_MSG(TRACE_APP1, "< test_device_cb %hd", (FMT__H, device_cb_param->status));
}

void test_device_scenes_table_init(void)
{
  zb_uint8_t i = 0;
  while (i < TEST_DEVICE_SCENES_TABLE_SIZE)
  {
    scenes_table[i].common.group_id = ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD;
    ++i;
  }
}

zb_uint8_t test_device_scenes_get_entry(zb_uint16_t group_id, zb_uint8_t scene_id)
{
  zb_uint8_t i = 0;
  zb_uint8_t idx = 0xFF, free_idx = 0xFF;

  while (i < TEST_DEVICE_SCENES_TABLE_SIZE)
  {
    if (scenes_table[i].common.group_id == group_id &&
        scenes_table[i].common.scene_id == scene_id)
    {
      idx = i;
      break;
    }
    else if (free_idx == 0xFF &&
             scenes_table[i].common.group_id == ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD)
    {
      /* Remember free index */
      free_idx = i;
    }
    ++i;
  }

  return ((idx != 0xFF) ? idx : free_idx);
}

void test_device_scenes_remove_entries_by_group(zb_uint16_t group_id)
{
  zb_uint8_t i = 0;

  TRACE_MSG(TRACE_APP1, ">> test_device_scenes_remove_entries_by_group: group_id 0x%x", (FMT__D, group_id));
  while (i < TEST_DEVICE_SCENES_TABLE_SIZE)
  {
    if (scenes_table[i].common.group_id == group_id)
    {
      TRACE_MSG(TRACE_APP1, "removing scene: entry idx %hd", (FMT__H, i));
      ZB_MEMSET(&scenes_table[i], 0, sizeof(scenes_table[i]));
      scenes_table[i].common.group_id = ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD;
    }
    ++i;
  }
  TRACE_MSG(TRACE_APP1, "<< test_device_scenes_remove_entries_by_group", (FMT__0));
}

#ifdef ZB_USE_NVRAM
void nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length)
{
  nvram_app_dataset_t ds;
  zb_ret_t ret;

  ZB_ASSERT(payload_length == sizeof(ds));
  /* If we fail, trace is given and assertion is triggered */
  ret = zb_nvram_read_data(page, pos, (zb_uint8_t*)&ds, sizeof(ds));
  if (ret == RET_OK)
  {
    g_attr_start_up_on_off = ds.start_up_on_off;
    apply_start_up_on_off();
  }
}

zb_ret_t nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos)
{
  zb_ret_t ret;
  nvram_app_dataset_t ds;

  ds.start_up_on_off = g_attr_start_up_on_off;
  /* If we fail, trace is given and assertion is triggered */
  ret = zb_nvram_write_data(page, pos, (zb_uint8_t*)&ds, sizeof(ds));
  return ret;
}

zb_uint16_t nvram_get_app_data_size(void)
{
 return sizeof(nvram_app_dataset_t);
}
#endif
