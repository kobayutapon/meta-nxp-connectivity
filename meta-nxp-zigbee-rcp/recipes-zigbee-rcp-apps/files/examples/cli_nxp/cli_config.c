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

#define ZB_TRACE_FILE_ID 33613
#include <ctype.h>
#include <stdlib.h>
#include "zboss_api.h"
#include "zboss_api_error.h"
#include "cli_config.h"
#include "cli_menu.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"
#include "cli_nvram.h"
#include "cli_tools.h"

typedef enum {
	role_unknown = 0,
#if defined(ZB_COORDINATOR_ROLE)
	role_coordinator,
#endif
#if defined(ZB_ROUTER_ROLE)
	role_router,
#endif
#if defined(ZB_ED_ROLE)
	role_end_device
#endif
} zb_role_t;


typedef enum {
	behavior_undef = 0,
	behavior_r23,
	behavior_r22,
} zb_behavior_t;


typedef struct {
  zb_ret_t        got_error;
  zb_cfg_state_e  state;
  zb_ieee_addr_t  ieee_addr;
  zb_role_t       role;
  struct {
  #define CHANNEL_TYPE_NUMBER  0
  #define CHANNEL_TYPE_MASK    1
    zb_bool_t     type;
    union  {
      zb_uint8_t  number;
      zb_uint32_t mask;
    } val;
  } channel;
  zb_behavior_t   behavior;
  zb_bool_t       nwk_distrib;
  zb_uint8_t      nwk_key[4][ZB_CCM_KEY_SIZE];
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  zb_uint16_t     panid;
  zb_ext_pan_id_t extpanid;
  zb_uint8_t      max_children;
  zb_bool_t       concentrator;
#endif
#define TX_POWER_NOT_INITIALIZED -128
  zb_int8_t       tx_power;
  zb_bool_t       once;
  zb_bool_t       signal_skip_startup;
  zb_bool_t       signal_steering;
  zb_uint8_t      permit_join_duration;
  zb_uint8_t      dbgtty;
  zb_uint16_t     manufacturer_code;
} zb_config_t;

zb_config_t config;

#if defined(ZB_COORDINATOR_ROLE)
#define CONFIG_ROLE_COORDINATOR "coordinator"
#else
#define CONFIG_ROLE_COORDINATOR "\x08"
#endif
#if defined(ZB_ROUTER_ROLE)
#define CONFIG_ROLE_ROUTER      "router"
#else
#define CONFIG_ROLE_ROUTER       "\x08"
#endif
#if defined(ZB_ED_ROLE)
#define CONFIG_ROLE_ED           "ed"
#else
#define CONFIG_ROLE_ED           "\x08"
#endif

/* list of all supported roles (zczr or zed) */
#define CONFIG_ROLES             CONFIG_ROLE_COORDINATOR" "CONFIG_ROLE_ROUTER" "CONFIG_ROLE_ED

/* Commands config handlers */
static zb_ret_t config_ieee_addr(int argc, char *argv[]);     static zb_ret_t help_ieee_addr(void);
static zb_ret_t config_role(int argc, char *argv[]);          static zb_ret_t help_role(void);
static zb_ret_t config_channel(int argc, char *argv[]);       static zb_ret_t help_channel(void);
static zb_ret_t config_behavior(int argc, char *argv[]);      static zb_ret_t help_behavior(void);
static zb_ret_t config_nwk_distrib(int argc, char *argv[]);   static zb_ret_t help_nwk_distrib(void);
static zb_ret_t config_nwk_key(int argc, char *argv[]);       static zb_ret_t help_nwk_key(void);
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
static zb_ret_t config_panid(int argc, char *argv[]);         static zb_ret_t help_panid(void);
static zb_ret_t config_extpanid(int argc, char *argv[]);      static zb_ret_t help_extpanid(void);
static zb_ret_t config_max_children(int argc, char *argv[]);  static zb_ret_t help_max_children(void);
static zb_ret_t config_concentrator(int argc, char *argv[]);  static zb_ret_t help_concentrator(void);
#endif
static zb_ret_t config_tx_power(int argc, char *argv[]);      static zb_ret_t help_tx_power(void);
static zb_ret_t config_factory_reset(int argc, char *argv[]); static zb_ret_t help_factory_reset(void);
/* create endpoint */
/* create cluster */
static zb_ret_t config_print(int argc, char *argv[]);
static zb_ret_t config_start(int argc, char *argv[]);
static zb_ret_t config_status(int argc, char *argv[]);
#ifdef ZB_ZBOSS_DEINIT
static zb_ret_t config_stop(int argc, char *argv[]);
#endif
static zb_ret_t config_pan_channel(int argc, char *argv[]);    static zb_ret_t help_pan_channel(void);
static zb_ret_t config_power_capa(int argc, char *argv[]);     static zb_ret_t help_power_capa(void);
static zb_ret_t config_rx_sensitivity(int argc, char *argv[]); static zb_ret_t help_rx_sensitivity(void);
static zb_ret_t config_manuf_code(int argc, char *argv[]);
#ifdef ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE
static zb_ret_t config_custom_msp(int argc, char *argv[]);     static zb_ret_t help_custom_msp(void);
#endif
#ifdef ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER
static zb_ret_t config_frag_on_msc(int argc, char *argv[]);     static zb_ret_t help_frag_on_msc(void);
#endif
static zb_ret_t config_get_version(int argc, char *argv[]);
#if defined(ZB_TRACE_LEVEL)
static zb_ret_t config_trace(int argc, char *argv[]);         static zb_ret_t help_trace(void);
#endif
static zb_ret_t config_dbgtty(int argc, char *argv[]);        static zb_ret_t help_dbgtty(void);

/* Menu config */
cli_menu_cmd menu_config[] = {
  /* name, args,                       align, function,             help,               description */
  { "ieee_addr", " [IEEE]", "              ", config_ieee_addr,     help_ieee_addr,     "configure the IEEE address: [AA:AA:AA:AA:AA:AA:AA:AA]" },
  { "role", " [role]", "                   ", config_role,          help_role,          "configure the role: [ "CONFIG_ROLES" ]" },
  { "channel", " [channel]", "             ", config_channel,       help_channel,       "configure the channel number or mask: [11-26|0xMMMMMMMM]" },
  { "behavior", " [behavior]", "           ", config_behavior,      help_behavior,      "configure the stack behavior [r22|r23]" },
  { "nwk_distrib", " [distrib]", "         ", config_nwk_distrib,   help_nwk_distrib,   "configure network distributed: [0-1]" },
  { "nwk_key", " [idx] [KEY128]", "        ", config_nwk_key,       help_nwk_key,       "configure secur network key index: [0-3] key [KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK]" },
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  { "panid", " [panid]", "                 ", config_panid,         help_panid,         "configure the panid: [0x0~0xFFFF]" },
  { "extpanid", " [extpanid]", "           ", config_extpanid,      help_extpanid,      "configure the extpanid: [EE:EE:EE:EE:EE:EE:EE:EE]" },
  { "max_children", " [nb]", "             ", config_max_children,  help_max_children,  "configure max devices to join, nb: [0-MAX_ED]" },
  { "concentrator", " [time] [radius]", "  ", config_concentrator,  help_concentrator,  "configure device as concentrator, time: the time in seconds between concentrator route discoveries [0x0~0xFFFFFFFF], radius the hop count radius for concentrator route discoveries [0-255]" },
#endif
  { "tx_power", " [power_value]", "        ", config_tx_power,      help_tx_power,      "configure tx power: [-20, 22] dBm" },
  { "factory_reset", " [factory_reset]", " ", config_factory_reset, help_factory_reset, "configure start with factory reset: [0-1], deprecated" },
  { "print", "", "                         ", config_print,         help_empty,         "printf config values" },
  { "start", "", "                         ", config_start,         help_empty,         "start ZigBee stack" },
  { "status", "", "                        ", config_status,        help_empty,         "status ZigBee stack" },
#ifdef ZB_ZBOSS_DEINIT
  { "stop", "", "                          ", config_stop,          help_empty,         "stop ZigBee stack" },
#endif
  { "pan_channel", " [get|set] <ch>", "    ", config_pan_channel,   help_pan_channel,   "get or set the pan channel (shared with OpenThread)" },
  { "power_capa", "", "                    ", config_power_capa,    help_power_capa,    "get tx power capabilities" },
  { "rx_sensitivity", "", "                ", config_rx_sensitivity,help_rx_sensitivity,"get rx sensitivity" },
  { "manuf_code", " [manuf_code]", "       ", config_manuf_code,    help_empty,         "configure the manufacturer code: [0xMMMM]" },
#ifdef ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE
  { "custom_msp", " [profile] [index]", "  ", config_custom_msp,    help_custom_msp,    "register a custom manufacturer specific profile [0xC000~0xFFFF] at index [0~19]" },
#endif
#ifdef ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER
  { "frag_on_msc", " [cluster] [index]", " ", config_frag_on_msc,   help_frag_on_msc,   "register a manufacturer specific cluster allowing APS fragmentation [0xFC00~0xFFFF] at index [0~19]" },
#endif
  { "get_version", " [version_type]", "    ", config_get_version,   help_empty,         "get software version [host|stack|firmware]"},
#if defined(ZB_TRACE_LEVEL)
  { "trace", " [level] [mask]", "          ", config_trace,         help_trace,         "configure the zboss log, level [0-4], mask [0x0~0xFFFFFFFF] " },
#endif
  { "dbgtty", " [level]", "                ", config_dbgtty,        help_dbgtty,        "configure debug TTY frames, level: [0-3]" },
  /* Add new commands above here */
  { NULL, NULL,                         NULL, NULL,                NULL,                NULL }
};


/* Global function
 * configure initial default values
 */
void config_init_default(void)
{
  char *dumpTtyStr = getenv("DUMP_TTY");

  memset(&config, 0, sizeof(config));
  config.state = STATE_INITTING;
  config.manufacturer_code = ZB_DEFAULT_MANUFACTURER_CODE;
  zb_get_long_address(config.ieee_addr);
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  config.panid = zb_get_pan_id();
  zb_get_extended_pan_id(config.extpanid);
  config.max_children = zb_nwk_get_max_ed_capacity();
#endif
  config.tx_power = TX_POWER_NOT_INITIALIZED;
  if(dumpTtyStr) config.dbgtty = atoi(dumpTtyStr);
  else           config.dbgtty = 0;
}


/* Static shared function
 * check the validity of the given channel
 */
static zb_bool_t config_is_channel_number_valid(zb_uint8_t channel)
{
  return (channel < 11 || channel > 26)?(ZB_FALSE):(ZB_TRUE);
}
static zb_bool_t config_is_channel_mask_valid(zb_uint32_t mask)
{
  // 0x800:     bit11 at 1
  // 0x7FFF800: bit11~26 at 1
  return (mask < 0x800 || mask > 0x7FFF800)?(ZB_FALSE):(ZB_TRUE);
}


/* Static shared function
 * check if the config minimal to start
 */
static zb_bool_t config_is_complete(void)
{
  zb_ieee_addr_t ieee_addr_empty = {0};

  if(!memcmp(config.ieee_addr, ieee_addr_empty, sizeof(zb_ieee_addr_t)))
    return ZB_FALSE;

  if(config.role == role_unknown)
    return ZB_FALSE;

  if(!(config.channel.type == CHANNEL_TYPE_NUMBER && config_is_channel_number_valid(config.channel.val.number)) &&
     !(config.channel.type == CHANNEL_TYPE_MASK && config_is_channel_mask_valid(config.channel.val.mask)))
    return ZB_FALSE;

  return ZB_TRUE;
}


/* Static shared function
 * convert role type to string
 */
static char *config_get_role_str(zb_role_t role)
{
  char *name="???";

  switch(role)
  {
#if defined(ZB_COORDINATOR_ROLE)
  case role_coordinator: name = CONFIG_ROLE_COORDINATOR; break;
#endif
#if defined(ZB_ROUTER_ROLE)
  case role_router:      name = CONFIG_ROLE_ROUTER;      break;
#endif
#if defined(ZB_ED_ROLE)
  case role_end_device:  name = CONFIG_ROLE_ED;          break;
#endif
  case role_unknown:     name = "unknown";               break;
  }

  return name;
}


/* Static shared function
 * convert behavior to string
 */
static char *config_get_behavior_str(zb_behavior_t behavior)
{
  char *name="???";

  switch(behavior)
  {
  case behavior_undef:  name = "undefined"; break;
  case behavior_r23:    name = "r23";       break;
  case behavior_r22:    name = "r22";       break;
  }

  return name;
}


/* Global function
 * check the stack is start
 */
zb_cfg_state_e config_get_state(void)
{
  return config.state;
}

static char *stateStr(zb_cfg_state_e state)
{
  char *str = "???";

  switch(state) {
    case STATE_INIT:     str="init";     break;
    case STATE_INITTING: str="initting"; break;
    case STATE_RUN:      str="run";      break;
    case STATE_RUNNING:  str="running";  break;
#ifdef ZB_ZBOSS_DEINIT
    case STATE_STOP:     str="stop";     break;
    case STATE_STOPPING: str="stopping"; break;
#endif
  }

  return str;
}

/* Global function
 * check the stack is start
 */
void config_set_state(zb_cfg_state_e state)
{
  if(config.state != state)
  {
    WCS_TRACE_DEBUG("STATE %s -> %s", stateStr(config.state), stateStr(state));
  }
  config.state = state;
}


/* Global function
 * check if the signal steering has been received from the stack
 */
zb_bool_t config_is_steering_received(void)
{
  return config.signal_steering;
}

/* Global function
 * get the lastest received permit join request duration
 */
zb_uint8_t config_get_permit_join_duration(void)
{
  return config.permit_join_duration;
}

void config_got_signal(zb_zdo_app_signal_type_t signal, zb_zdo_app_signal_hdr_t *signal_hdr)
{
  switch(signal) {
#ifdef ZB_MACSPLIT
    case ZB_MACSPLIT_DEVICE_BOOT:          /* result of message DEV_BOOT from firmware, not yet configured */
      break;
#endif

    case ZB_ZDO_SIGNAL_SKIP_STARTUP:       /* result of zboss_start_no_autostart(), firmware configured */
      /* Unblock config_start */
      config.signal_skip_startup = ZB_TRUE;
      /* Same as zboss_start_continue(), but does not form|join the network */
      bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
      //zboss_start_continue();
      break;

    case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
      if(config.manufacturer_code != ZB_DEFAULT_MANUFACTURER_CODE)
        zb_set_node_descriptor_manufacturer_code_req(config.manufacturer_code, NULL);
      break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START: /* result of bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION) */
      break;

    case ZB_BDB_SIGNAL_FORMATION:          /* result of bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION) */
      // This open the network:bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
      break;

    case ZB_BDB_SIGNAL_STEERING:           /* result of bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING) */
      /* Unblock network_open */
      config.signal_steering = ZB_TRUE;
      break;

    case ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS: /* result of bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING) */
      {                                    /* result of zb_zdo_mgmt_permit_joining_req() */
        zb_uint8_t *permit_duration = ZB_ZDO_SIGNAL_GET_PARAMS(signal_hdr, zb_uint8_t);
        config.permit_join_duration = *permit_duration;
      }
      break;

#ifdef ZB_ZBOSS_DEINIT
    case ZB_SIGNAL_READY_TO_SHUT:
      zboss_complete_shut();
      config_set_state(STATE_INIT);
      break;
#endif
   }
}

static zb_bool_t config_got_error(zb_uint8_t severity, zb_ret_t error_code, void *additional_info)
{
  zb_bool_t ret = ZB_FALSE;
  ZVUNUSED(additional_info);
  /* Unused without trace. */

  WCS_TRACE_ERROR("ERROR severity: %s, error code: %s", get_err_sev_str(severity), wcs_get_error_str(error_code));

  config.got_error = error_code;
  switch(error_code) {
#ifdef RET_CONFIGURATION_ERROR
    case RET_CONFIGURATION_ERROR:
      menu_printf("error_cb() invalid config");
      break;
#endif
    default:
      menu_printf("error_cb() severity: %s, error code: %s", get_err_sev_str(severity), wcs_get_error_str(error_code));
      break;
  }

  /* return TRUE to prevent default error handling by the stack: ZB_RESET_AUTORESTART */
  return ret;
}


#if defined(ZB_COORDINATOR_ROLE)
/* Global function
 * check the role is coordinator
 */
zb_bool_t config_is_coordinator(void)
{
  return (config.role == role_coordinator);
}
#endif


#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
/* Global function
 * check the concentrator mode
 */
zb_bool_t config_is_concentrator(void)
{
  return config.concentrator;
}
#endif


#if defined(ZB_ROUTER_ROLE)
/* Global function
 * check the role is router
 */
zb_bool_t config_is_router(void)
{
  return (config.role == role_router);
}
#endif


#if defined(ZB_ED_ROLE)
/* Global function
 * check the role is end device
 */
zb_bool_t config_is_end_device(void)
{
  return (config.role == role_end_device);
}
#endif

/* Global function
 * get the ieee_addr
 */
zb_ieee_addr_t *config_get_ieee_addr(void)
{
  return &config.ieee_addr;
}

#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
static zb_ret_t config_apply_role_and_channel(void);
/* Global function
 * config update
 */
zb_bool_t config_update(zb_uint8_t channel, zb_uint16_t panid, zb_ext_pan_id_t ext_panid)
{
  if((config.channel.type == CHANNEL_TYPE_NUMBER && config.channel.val.number != channel) ||
     (config.channel.type == CHANNEL_TYPE_MASK && config.channel.val.mask != (1l<< channel)))
  {
    menu_printf("config channel %d", channel);
    config.channel.type = CHANNEL_TYPE_NUMBER;
    config.channel.val.number = channel;
    config_apply_role_and_channel();
  }

  menu_printf("config panid 0x%04x", panid);
  zb_set_pan_id(panid);

  menu_printf("config expanid "TRACE_FORMAT_64, TRACE_ARG_64(ext_panid));
  zb_set_extended_pan_id(ext_panid);

  return config.nwk_distrib;
}
#endif

/* Static shared function
 * check the role & channel can be applied
 */
static zb_ret_t config_apply_role_and_channel(void)
{
  zb_ret_t ret = RET_OK;
  zb_uint32_t this_mask;

  if(config.channel.type == CHANNEL_TYPE_NUMBER)
    this_mask = (1l<<config.channel.val.number);
  else
    this_mask = config.channel.val.mask;

  switch(config.role)
  {
#if defined(ZB_COORDINATOR_ROLE)
  case role_coordinator:
    zb_set_network_coordinator_role(this_mask);
    break;
#endif
#if defined(ZB_ROUTER_ROLE)
  case role_router:
    zb_set_network_router_role(this_mask);
    break;
#endif
#if defined(ZB_ED_ROLE)
  case role_end_device:
    zb_set_network_ed_role(this_mask);
    break;
#endif
  default:
    ret = RET_INVALID_PARAMETER_1;
    break;
  }

  return ret;
}


/* Static command config
 * command ieee_addr
 *
 * config ieee_addr [IEEE]
 */
static zb_ret_t config_ieee_addr(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_ieee_addr_t new_ieee_addr = { 0 };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [IEEE] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 0, new_ieee_addr);

  memcpy(config.ieee_addr, new_ieee_addr, sizeof(zb_ieee_addr_t));

  zb_set_long_address(config.ieee_addr);
  return RET_OK;
}
static zb_ret_t help_ieee_addr(void)
{
  menu_printf("pre-start MANDATORY config");
  return RET_OK;
}


/* Static command config
 * command role
 *
 * config role [role]
 */
static zb_ret_t config_role(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_role = role_unknown;
  cli_tools_strval role_table[] = {
#if defined(ZB_COORDINATOR_ROLE)
    { "coordinator", role_coordinator },
#endif
#if defined(ZB_ROUTER_ROLE)
    { "router", role_router },
#endif
#if defined(ZB_ED_ROLE)
    { "end_device", role_end_device },
#endif
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [role] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, role_table, &new_role);
  if(new_role == role_unknown)
    return RET_INVALID_PARAMETER_1;

  config.role = (zb_role_t)new_role;

  if((config.channel.type == CHANNEL_TYPE_NUMBER && config_is_channel_number_valid(config.channel.val.number)) ||
     (config.channel.type == CHANNEL_TYPE_MASK && config_is_channel_mask_valid(config.channel.val.mask)))
    return config_apply_role_and_channel();

  /* Postponed to config_channel */
  return RET_OK;
}
static zb_ret_t help_role(void)
{
  menu_printf("pre-start MANDATORY config");
  return RET_OK;
}


/* Static command config
 * command channel
 *
 * config channel [channel]
 */
static zb_ret_t config_channel(int argc, char *argv[])
{
  zb_uint8_t new_channel_num = 0;
  zb_uint32_t new_channel_mask = 0;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [channel]: either an 8bits value as a channel number or a 32bits value  as a channel mask */
  if(tools_arg_get_uint8(argv, 0, &new_channel_num, ARG_DEC) == RET_OK && config_is_channel_number_valid(new_channel_num))
  {
    config.channel.type = CHANNEL_TYPE_NUMBER;
    config.channel.val.number = new_channel_num;
  }
  else if(tools_arg_get_uint32(argv, 0, &new_channel_mask, ARG_HEX) == RET_OK && config_is_channel_mask_valid(new_channel_mask))
  {
    config.channel.type = CHANNEL_TYPE_MASK;
    config.channel.val.mask = new_channel_mask;
  }
  else
    return RET_INVALID_PARAMETER_1;

  if(config.role != role_unknown)
    return config_apply_role_and_channel();

  /* Postponed to config_role */
  return RET_OK;
}
static zb_ret_t help_channel(void)
{
  menu_printf("pre-start MANDATORY config");
  menu_printf("channel can be either:");
  menu_printf(" - number between 11 to 26");
  menu_printf(" - 32 bits mask in hex");
  return RET_OK;
}


/* Static command config
 * command behavior
 *
 * config behavior [behavior]
 */
static zb_ret_t config_behavior(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_behavior = behavior_undef;
  cli_tools_strval behavior_table[] = {
    { "r22", behavior_r22 },
    { "r23", behavior_r23 },
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [behavior] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, behavior_table, &new_behavior);
  if(new_behavior == behavior_undef)
    return RET_INVALID_PARAMETER_1;

  config.behavior = (zb_behavior_t)new_behavior;

  switch(config.behavior)
  {
    case behavior_r23:    zboss_use_r23_behavior(); break;
    case behavior_r22:    zboss_use_r22_behavior(); break;
    case behavior_undef:                            break;
  }

  return RET_OK;
}
static zb_ret_t help_behavior(void)
{
  menu_printf("pre-start optional config");
  menu_printf("stack behavior can be either:");
  menu_printf(" r23 (default) - Enable using of r23 joining features: NWK Commissioning then DLK");
  menu_printf(" r22           - Switch to the legacy r22 joining by association, key update by Transport Key");
  return RET_OK;
}


/* Static command config
 * command nwk_distrib
 *
 * config nwk_distrib [distrib]
 */
static zb_ret_t config_nwk_distrib(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_nwk_distrib;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [distrib] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_nwk_distrib);
  if(new_nwk_distrib > 1)
    return RET_INVALID_PARAMETER_1;

  config.nwk_distrib = (new_nwk_distrib == 1);

#if defined(ZB_ROUTER_ROLE)
  zb_bdb_enable_distributed_formation(config.nwk_distrib);
#endif
#if defined(ZB_ED_ROLE)
  zb_enable_joining_to_distributed(config.nwk_distrib);
#endif
  return RET_OK;
}
static zb_ret_t help_nwk_distrib(void)
{
  menu_printf("pre-start OPTIONAL config");
  return RET_OK;
}


/* Static command config
 * command nwk_key
 *
 * config nwk_key [idx] [KEY128]
 */
static zb_ret_t config_nwk_key(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_nwk_key[ZB_CCM_KEY_SIZE];
  zb_uint8_t new_key_number;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [idx] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_key_number);
  if(new_key_number > 3)
    return RET_INVALID_PARAMETER_1;

  /* get [KEY128] */
  TOOLS_GET_ARG_HEXA(ret, nwk_key, argv, 1, new_nwk_key);
  memcpy(config.nwk_key[new_key_number], new_nwk_key, ZB_CCM_KEY_SIZE);

  zb_secur_setup_nwk_key(config.nwk_key[new_key_number], new_key_number);

  return RET_OK;
}
static zb_ret_t help_nwk_key(void)
{
  menu_printf("pre-start OPTIONAL config");
  return RET_OK;
}


#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
static void app_nwk_mgmt_change_panid_cb(zb_uint8_t param)
{
  zb_ret_t ret = RET_ERROR;
  zb_channel_panid_change_preparation_t *params = (zb_channel_panid_change_preparation_t *)zb_buf_begin(param);

  if (params->error_cnt == 0)
  {
    ret = zb_start_panid_change(param);
    if (ret == RET_OK)
    {
      /* Broadcast network key to decode packets in Wireshark after PAN ID change. Only for debugging */
#ifdef DEBUG
      ZB_SCHEDULE_APP_ALARM((void *)zb_debug_broadcast_nwk_key, 0, 10 * ZB_TIME_ONE_SECOND);
#endif
    }
    menu_printf("change_panid_cb() status: %s", wcs_get_error_str(ret));
  }
  else
  {
    menu_printf("change_panid_cb() Error counter: %d No change PANID performed", params->error_cnt);
  }
}

/* Static command config
 * command panid
 *
 * config panid [panid]
 */
static zb_ret_t config_panid(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_panid;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [panid] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &new_panid);

  config.panid = new_panid;

  if (config.state <= STATE_INITTING)
  {
    zb_set_pan_id(config.panid);
    ret = RET_OK;
  }
  else /* Change PANID after 1st network creation */
  {
    zb_bufid_t param = zb_buf_get_out();
    zb_panid_change_parameters_t *params = ZB_BUF_GET_PARAM(param, zb_panid_change_parameters_t);

    params->next_panid_change = config.panid;

    /* Send set_configuration_req to all devices for allow PAN ID change */
    ret = zb_prepare_network_for_panid_change(param, app_nwk_mgmt_change_panid_cb);

    menu_printf("Network preparation status: %s", wcs_get_error_str(ret));
  }
  return ret;
}

static zb_ret_t help_panid(void)
{
  menu_printf("Runtime OPTIONAL config");
  menu_printf("At runtime, check next Link status until it takes effect");
  return RET_OK;
}


/* Static command config
 * command extpanid
 *
 * config extpanid [extpanid]
 */
static zb_ret_t config_extpanid(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_ext_pan_id_t new_extpanid = {0};

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [extpanid] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 0, new_extpanid);

  memcpy(config.extpanid, new_extpanid, sizeof(zb_ext_pan_id_t));

  zb_set_extended_pan_id(config.extpanid);

  return RET_OK;
}
static zb_ret_t help_extpanid(void)
{
  menu_printf("pre-start OPTIONAL config");
  return RET_OK;
}


/* Static command config
 * command max_children
 *
 * config max_children [nb]
 */
static zb_ret_t config_max_children(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_max_children;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [nb] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_max_children);
  if(new_max_children > ZB_MAX_ED_CAPACITY_DEFAULT)
    return RET_INVALID_PARAMETER_1;

  config.max_children = new_max_children;

  zb_nwk_set_max_ed_capacity(config.max_children);

  return RET_OK;
}
static zb_ret_t help_max_children(void)
{
  menu_printf("pre-start OPTIONAL config, max value %u", ZB_MAX_ED_CAPACITY_DEFAULT);
  return RET_OK;
}


/* Static command config
 * command concentrator_mode
 *
 * config concentrator [time] [radius]
 */
static zb_ret_t config_concentrator(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint32_t disc_time; /* Default value is 0: No periodical route discoveries */
  zb_uint8_t radius;     /* Default value is 15 */

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [time] */
  TOOLS_GET_ARG(ret, uint32, argv, 0, &disc_time);

  /* get [radius] */
  TOOLS_GET_ARG(ret, uint8, argv, 1, &radius);

  if(!config_is_coordinator())
  {
    menu_printf("Concentrator mode supported on ZC only.");
    return RET_ERROR;
  }
  else
  {
    static zb_bool_t is_already_started = ZB_FALSE;
    if (is_already_started == ZB_FALSE)
    {
      menu_printf("Concentrator config: radius %hd, disc_time %lu", radius, disc_time);
      zb_start_concentrator_mode(radius, disc_time);
      is_already_started = ZB_TRUE;
      config.concentrator = ZB_TRUE;
      return RET_OK;
    }
    else
    {
      menu_printf("Concentrator mode already started");
      return RET_ERROR;
    }
  }
  return RET_OK;
}
static zb_ret_t help_concentrator(void)
{
  menu_printf("run-time OPTIONAL config");
  return RET_OK;
}
#endif


/* Static stack callback function
 * response for zb_get/set_tx_power_async
 */
static void tx_power_cb(zb_uint8_t param)
{
 zb_tx_power_params_t *resp = zb_buf_begin(param);

  switch (resp->status)
  {
    case RET_OK:
      config.tx_power = resp->tx_power;
      menu_printf("tx_power_cb() Ok: %d", config.tx_power);
      break;
    
    case RET_INVALID_PARAMETER_1:
      menu_printf("tx_power_cb() Error: INVALID_PARAM_1 (page %u): %d %d", resp->page, resp->tx_power, config.tx_power);
      break;

    case RET_INVALID_PARAMETER_2:
      menu_printf("tx_power_cb() Error: INVALID_PARAM_2 (channel %u): %d %d", resp->channel, resp->tx_power, config.tx_power);
      break;

    case RET_INVALID_PARAMETER_3:
      menu_printf("tx_power_cb() Error: INVALID_PARAM_3 (tx_power %d): %d", resp->tx_power, config.tx_power);
      break;
    
    default:
      menu_printf("tx_power_cb() Error: %s (req %u, %u %d): %d", wcs_get_error_str(resp->status), resp->page, resp->channel, resp->tx_power, config.tx_power);
      break;
  }

  zb_buf_free(param);
}

/* Static scheduled function
 * command get_tx_power
 */
static void get_tx_power(zb_uint8_t param)
{
  zb_tx_power_params_t *req;

//  req = ZB_BUF_GET_PARAM(param, zb_tx_power_params_t);
  req = zb_buf_initial_alloc(param, sizeof(zb_tx_power_params_t));
  req->page    = ZB_CHANNEL_PAGE0_2_4_GHZ;
  req->channel = zb_get_current_channel();
  req->cb      = tx_power_cb;

  ZB_SCHEDULE_APP_CALLBACK(zb_get_tx_power_async, param);
}

/* Static scheduled function
 * command set_tx_power
 */
static void set_tx_power(zb_uint8_t param)
{
  zb_tx_power_params_t *req;

//  req = ZB_BUF_GET_PARAM(param, zb_tx_power_params_t);
  req = zb_buf_initial_alloc(param, sizeof(zb_tx_power_params_t));
  req->page     = ZB_CHANNEL_PAGE0_2_4_GHZ;
  req->channel  = zb_get_current_channel();
  req->tx_power = config.tx_power;
  req->cb       = tx_power_cb;

  ZB_SCHEDULE_APP_CALLBACK(zb_set_tx_power_async, param);
}


/* Static command config
 * command tx_power
 *
 * config tx_power [power_value]
 */
static zb_ret_t config_tx_power(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_int8_t new_tx_power;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [power_value] */
  TOOLS_GET_ARG(ret, int8, argv, 0, &new_tx_power);

  config.tx_power = new_tx_power;

  if(config.state >= STATE_RUNNING)
    set_tx_power(zb_buf_get_out());
  else
    zb_buf_get_out_delayed(set_tx_power);

  return RET_OK;
}
static zb_ret_t help_tx_power(void)
{
  menu_printf("runtime config");
  return RET_OK;
}


/* Static command config
 * command factory_reset
 *
 * config factory_reset [factory_reset]
 */
static zb_ret_t config_factory_reset(int argc, char *argv[])
{
  return nvram_erase_at_start(argc, argv);
}
static zb_ret_t help_factory_reset(void)
{
  menu_printf("deprecated, replaced by nvram erase_at_start");
  return RET_OBSOLETE;
}

/* Static command config
 * command get_version
 *
 * config get_version [version_type]
 */
static zb_ret_t config_get_version(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t version_type;
  cli_tools_strval vtype_table[] = {
    { "host",     HOST_VERSION },
    { "stack",    STACK_VERSION },
    { "firmware", FIRMWARE_VERSION }
  };

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [version_type] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, vtype_table, &version_type);

  menu_printf("%s version: %s",argv[0], zb_get_version((zb_version_type_t)version_type));
  return RET_OK;
}

/* Static command config
 * command print
 *
 * config print
 */
static zb_ret_t config_print(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  menu_printf("manuf_code:       0x%04x", (config.state >= STATE_RUNNING)?(zb_get_node_descriptor_manufacturer_code()):(config.manufacturer_code));
  menu_printf("Host version:     %s", zb_get_version(HOST_VERSION));
  menu_printf("Stack version:    %s", zb_get_version(STACK_VERSION));
  menu_printf("Firmware version: %s", zb_get_version(FIRMWARE_VERSION));
  menu_printf("ieee_addr:        "TRACE_FORMAT_64, TRACE_ARG_64(config.ieee_addr));
  menu_printf("role:             %s",               config_get_role_str(config.role));
  if(config.channel.type == CHANNEL_TYPE_NUMBER)
    menu_printf("channel num:      %u",             config.channel.val.number);
  else
  {
    char chNumStr[256] = {0};
    int chNumLen = 0;

    for(int ch=0; ch<32; ch++)
      if((1l<<ch) & config.channel.val.mask)
        chNumLen += wcs_snprintf(chNumStr+chNumLen, 256-chNumLen, "%u ", ch);
    if(chNumLen > 0) chNumStr[chNumLen-1] = '\0'; /* remove last space */

    menu_printf("channel mask:     0x%08x (%s)",    config.channel.val.mask, chNumStr);
  }
  menu_printf("behavior:         %s",               config_get_behavior_str(config.behavior));
  menu_printf("nwk_distrib:      %u",               config.nwk_distrib);
  menu_printf("nwk_key 0:        "TRACE_FORMAT_128, TRACE_ARG_128(config.nwk_key[0]));
  menu_printf("nwk_key 1:        "TRACE_FORMAT_128, TRACE_ARG_128(config.nwk_key[1]));
  menu_printf("nwk_key 2:        "TRACE_FORMAT_128, TRACE_ARG_128(config.nwk_key[2]));
  menu_printf("nwk_key 3:        "TRACE_FORMAT_128, TRACE_ARG_128(config.nwk_key[3]));
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
  menu_printf("panid:            0x%x",               config.panid);
  menu_printf("extpanid:         "TRACE_FORMAT_64, TRACE_ARG_64(config.extpanid));
  menu_printf("max_children:     %u",               config.max_children);
  menu_printf("concentrator:     %s", (config.concentrator)?("enabled"):("disabled"));
#endif
  menu_printf("tx_power:         %d",               config.tx_power);
  menu_printf("state:            %u",               config.state);
#if defined(ZB_TRACE_LEVEL)
  menu_printf("trace:            %u 0x%08x", g_trace_level, g_trace_mask);
#endif
  menu_printf("dbgtty:           %u", config.dbgtty);
  return  RET_OK;
}


/* Static command config
 * command start
 *
 * config start
 */
static zb_ret_t config_start(int argc, char *argv[])
{
  zb_ret_t ret;

  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  if(config.state >= STATE_RUNNING)
    return RET_OK;

  if(!config_is_complete())
  {
    menu_printf("config start ERROR: incomplete config");
    return RET_INVALID_PARAMETER;
  }

  if(!config.once)
  {
    config.once = ZB_TRUE;

    config.got_error = RET_OK;
    zb_error_register_app_handler(config_got_error);

    /* Entry point for endpoints & clusters declaration to the stack */
    ZB_AF_REGISTER_DEVICE_CTX(&cli_ctx);

#ifdef RET_CONFIGURATION_ERROR
    /* If the error RET_CONFIGURATION_ERROR occurs:
     * - in release build, the application restarts after calling the registered error handler
     * - in debug build, the application asserts after calling the registered error handler
     * so in any case, we will never reach this point */
    if(config.got_error != RET_OK)
      return config.got_error;
#endif

    ZB_ZCL_REGISTER_DEVICE_CB(cluster_attributes_cb);
  }

  for(int i = 0; i < cli_ctx.ep_count; i++)
  {
    if(!cli_ctx.ep_desc_list[i])
      continue;

    cluster_init(cli_ctx.ep_desc_list[i]->ep_id);
  }

  if(config.tx_power == TX_POWER_NOT_INITIALIZED)
  {
    /* Request to get tx_power */
    zb_buf_get_out_delayed(get_tx_power);
  }

  /* It will be really started by the main thread */
  config_set_state(STATE_RUN);
  ret = RET_OK;

  /* Wait for ZB_ZDO_SIGNAL_SKIP_STARTUP */
  while(!config.signal_skip_startup)
    usleep(100);

  return ret;
}


/* Static command config
 * command status
 *
 * config status
 */
static zb_ret_t config_status(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  menu_printf("once:      %d", config.once);
  menu_printf("state:     %d", config.state);
  menu_printf("scheduler:        %s", (ZB_SCHEDULER_IS_STOP())?("stopped"):("running"));
  menu_printf("sig skip_startup: %d", config.signal_skip_startup);
  menu_printf("sig steering:     %d", config.signal_steering);
  menu_printf("joined:           %d", ZB_JOINED());

  return RET_OK;
}


#ifdef ZB_ZBOSS_DEINIT
/* Static command config
 * command stop
 *
 * config stop
 */
static zb_ret_t config_stop(int argc, char *argv[])
{
  zb_ret_t ret;

  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  if(config.state < STATE_RUNNING || config.state >= STATE_STOPPING)
    return RET_OK;

//  zb_bufid_t param = zb_buf_get_out();
//  zboss_shut_with_host_reset(param);
//  zboss_start_shut(param);

  /* It will be really stopped by the main thread */
  config_set_state(STATE_STOP);

  ret = RET_OK;

  return ret;
}
#endif

/* Static stack callback function
 * response for get_rx_sensitivity_cb
 */
static void get_rx_sensitivity_cb(zb_uint8_t param)
{
  zb_rx_sensitivity_params_t *resp = (zb_rx_sensitivity_params_t *)zb_buf_begin(param);

  menu_printf("get_rx_sensitivity_cb()");
  menu_printf("\tRX SENSITIVITY: %d", resp->rx_sensitivity);
  zb_buf_free(param);
}

/* Static scheduled function
 * command get_rx_sensitivity
 */
static void get_rx_sensitivity(zb_uint8_t param)
{
  zb_rx_sensitivity_params_t *req;

  req = zb_buf_initial_alloc(param, sizeof(zb_rx_sensitivity_params_t));
  req->cb = get_rx_sensitivity_cb;

  ZB_SCHEDULE_APP_CALLBACK(zb_get_rx_sensitivity, param);
}

/* Static command config
 * command rx_sensitivity
 *
 * config rx_sensitivity
 */
static zb_ret_t config_rx_sensitivity(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  if(config.state >= STATE_RUNNING)
    get_rx_sensitivity(zb_buf_get_out());
  else
    zb_buf_get_out_delayed(get_rx_sensitivity);

  return  RET_OK;
}

static zb_ret_t help_rx_sensitivity(void)
{
  menu_printf("runtime config");
  return RET_OK;
}


/* Static stack callback function
 * response for <get|set>_pan_channel
 */
static void get_pan_channel_cb(zb_bufid_t param)
{
  zb_pan_channel_params_t *channel_params = zb_buf_begin(param);

  if(channel_params->status != RET_OK)
  {
    menu_printf("get_pan_channel_cb() failed %s", wcs_get_error_str(channel_params->status));
  }
  else
  {
    menu_printf("get_pan_channel_cb() Ok: channel %u, page %u", channel_params->pan_channel, channel_params->page);
  }

  zb_buf_free(param);
}
static void set_pan_channel_cb(zb_bufid_t param)
{
  zb_pan_channel_params_t *channel_params = zb_buf_begin(param);

  if(channel_params->status != RET_OK)
  {
    menu_printf("set_pan_channel_cb() failed %s", wcs_get_error_str(channel_params->status));
  }
  else
  {
    menu_printf("set_pan_channel_cb() Ok: channel %u, page %u", channel_params->pan_channel, channel_params->page);
  }

  zb_buf_free(param);
}

/* Static command config
 * command pan_channel
 *
 * config pan_channel [get|set] <ch>
 */
static zb_ret_t config_pan_channel(int argc, char *argv[])
{
  zb_ret_t ret = RET_OPERATION_FAILED;
  zb_uint8_t get_nset_pan_ch;
  zb_uint8_t new_set_channel;
  cli_tools_strval cmd_table[] = {
    { "get", ZB_TRUE  },
    { "set", ZB_FALSE }
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &get_nset_pan_ch);

  if(get_nset_pan_ch == ZB_TRUE)
  {
    /* get: no second arg */
    if(argc != 1)
      return RET_INVALID_PARAMETER;
  }
  else
  {
    /* set: second arg is channel */
    if(argc != 2)
      return RET_INVALID_PARAMETER;

    /* get <ch> */
    if(tools_arg_get_uint8(argv, 1, &new_set_channel, ARG_DEC) != RET_OK || (new_set_channel < 11 || new_set_channel > 26))
      return RET_INVALID_PARAMETER_2;
  }

  if(config.state < STATE_RUNNING)
    return RET_UNAUTHORIZED;

  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_pan_channel_params_t *channel_params;

    buffer = zb_buf_get_out();
    channel_params = zb_buf_initial_alloc(buffer, sizeof(zb_pan_channel_params_t));
//    channel_params = ZB_BUF_GET_PARAM(buffer, zb_pan_channel_params_t);

    channel_params->page = ZB_CHANNEL_PAGE0_2_4_GHZ;
    if(get_nset_pan_ch == ZB_TRUE)
    {
      /* get pan channel */
      channel_params->pan_channel = -1;
      channel_params->cb = get_pan_channel_cb;
      ret = ZB_SCHEDULE_APP_CALLBACK(zb_get_pan_channel_async, buffer);
    }
    else
    {
      /* set pan channel */
      channel_params->pan_channel = new_set_channel;
      channel_params->cb =          set_pan_channel_cb;
      ret = ZB_SCHEDULE_APP_CALLBACK(zb_set_pan_channel_async, buffer);
    }
  }

  return ret;
}
static zb_ret_t help_pan_channel(void)
{
  menu_printf("pan_channel get:           read the pan channel");
  menu_printf("pan channel set [channel]: write the pan channel [11~26]");
  return RET_OK;
}


/* Static stack callback function
 * response for get_tx_power_capabilities
 */
static void get_tx_power_capa_cb(zb_uint8_t param)
{
  zb_tx_power_capabilities_params_t *resp = zb_buf_begin(param);

  menu_printf("get_tx_power_capa_cb()");
  for(zb_uint8_t i=0; i<16; i++)
  {
    menu_printf("\tCHANNEL[%u]: tx_power in { %d, %d }", 11+i, resp->min[i], resp->max[i]);
  }
  zb_buf_free(param);
}

/* Static scheduled function
 * command get_tx_power_capabilities
 */
static void get_tx_power_capa(zb_uint8_t param)
{
  zb_tx_power_capabilities_params_t *req;

  req = zb_buf_initial_alloc(param, sizeof(zb_tx_power_capabilities_params_t));
  req->cb = get_tx_power_capa_cb;

  ZB_SCHEDULE_APP_CALLBACK(zb_get_tx_power_capabilities, param);
}

/* Static command config
 * command power_capa
 *
 * config power_capa
 */
static zb_ret_t config_power_capa(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  if(config.state >= STATE_RUNNING)
    get_tx_power_capa(zb_buf_get_out());
  else
    zb_buf_get_out_delayed(get_tx_power_capa);

  return  RET_OK;
}
static zb_ret_t help_power_capa(void)
{
  menu_printf("runtime config");
  return RET_OK;
}


/* Static stack callback function
 * response for zb_set_node_descriptor_manufacturer_code_req
 */
static void manuf_code_cb(zb_ret_t status)
{
  menu_printf("manuf_code_cb() %s", wcs_get_error_str(status));
}

/* Static command config
 * command manuf_code
 *
 * config manuf_code [manuf_code]
 */
static zb_ret_t config_manuf_code(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_manuf_code;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [manuf_code] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &new_manuf_code);

  config.manufacturer_code = new_manuf_code;

  if(config.state > STATE_RUN)
    zb_set_node_descriptor_manufacturer_code_req(config.manufacturer_code, manuf_code_cb);

  return RET_OK;
}


#ifdef ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE
/* Static command config
 * command custom_msp
 *
 * config custom_msp [profile] [index]
 */
static zb_ret_t config_custom_msp(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_custom_profile;
  zb_uint8_t  new_index;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [profile] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &new_custom_profile);

  /* get [index] */
  TOOLS_GET_ARG(ret, uint8, argv, 1, &new_index);

  if(config.state < STATE_RUN)
    return RET_UNAUTHORIZED;
 
   return zb_zcl_register_custom_msp(new_custom_profile, new_index);
}
static zb_ret_t help_custom_msp(void)
{
  menu_printf("runtime config: register a custom manufacturer specific profile");
  menu_printf("                this allows the stack to handle zcl_frame for this MSP");
  menu_printf("- profile_id:  0x%04x~0xFFFF (only ones registered to the CSA)", ZB_MSP_INITIAL_PROFILE_ID);
  menu_printf("- index:       0~%d", ZB_MAX_EP_NUMBER);
  return RET_OK;
}
#endif /* ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE */


#ifdef ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER
/* Static command config
 * command frag_on_msc
 *
 * config frag_on_msc [cluster] [index]
 */
static zb_ret_t config_frag_on_msc(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t new_ms_cluster;
  zb_uint8_t  new_index;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [cluster] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 0, &new_ms_cluster);

  /* get [index] */
  TOOLS_GET_ARG(ret, uint8, argv, 1, &new_index);

  if(config.state < STATE_RUN)
    return RET_UNAUTHORIZED;

   return zb_zcl_register_custom_msc(new_ms_cluster, new_index, ZB_TRUE);
}
static zb_ret_t help_frag_on_msc(void)
{
  menu_printf("runtime config: register a manufacturer specific cluster allowing APS fragmentation");
  menu_printf("- cluster_id:  0x%04x~0xFFFF", ZB_MSC_INITIAL_CLUSTER_ID);
  menu_printf("- index:       0~%d", ZB_MAX_EP_NUMBER);
  return RET_OK;
}
#endif /* ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER */


#if defined(ZB_TRACE_LEVEL)
/* Static command config
 * command trace
 *
 * config trace [level] [mask]
 */
static zb_ret_t config_trace(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_level;
  zb_uint32_t new_mask;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [level] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_level);
  if(new_level > 4)
    return RET_INVALID_PARAMETER_1;

  /* get [mask] */
  TOOLS_GET_ARG_HEXA(ret, uint32, argv, 1, &new_mask);

  ZB_SET_TRACE_LEVEL(new_level);
  ZB_SET_TRACE_MASK(new_mask);

  return RET_OK;
}
static zb_ret_t help_trace(void)
{
  menu_printf("runtime config: change zboss log level");
  menu_printf("- none:    0 0x00000000");
  menu_printf("- release: 4 0x00000800");
  menu_printf("- debug:   4 0xFFFFFFFF");
  menu_printf("- custom:  4 0xXXXXXXXX, refer to include/zb_trace.h");
  return RET_OK;
}
#endif


/* Static command config
 * command dbgtty
 *
 * config dbgtty [level]
 */
static zb_ret_t config_dbgtty(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t new_dbgtty;
  
  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [level] */
  TOOLS_GET_ARG(ret, uint8, argv, 0, &new_dbgtty);
  if(new_dbgtty > 3)
    return RET_INVALID_PARAMETER_1;

  if(new_dbgtty != config.dbgtty)
  {
    char dumpTtyVal[] = "?";
    
    sprintf(dumpTtyVal, "%hhu", new_dbgtty);
    setenv("DUMP_TTY", dumpTtyVal, 1);

    config.dbgtty = new_dbgtty;
  }

  return RET_OK;
}
static zb_ret_t help_dbgtty(void)
{
  menu_printf("runtime config: change debug tty log level (display raw or interpreted uart data):");
  menu_printf("- 0: no log");
  menu_printf("- 1: log raw tty data (HDLC frame)");
  menu_printf("- 2: log interpreted tty data (SPINEL payload)");
  menu_printf("- 3: log raw & interpreted tty data");
  return RET_OK;
}

