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

#define ZB_TRACE_FILE_ID 33612
#include <ctype.h>
#include "zboss_api.h"
#include "cli_menu.h"
#include "cli_config.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"
#include "cli_tools.h"


#define MAX_CLUSTERS 256


// Local define to simply lisibility:
#define NO_MANUF_SPE        ZB_ZCL_NON_MANUFACTURER_SPECIFIC

#define ACC_READ_ONLY       ZB_ZCL_ATTR_ACCESS_READ_ONLY
#define ACC_WRITE_ONLY      ZB_ZCL_ATTR_ACCESS_WRITE_ONLY
#define ACC_READ_WRITE      ZB_ZCL_ATTR_ACCESS_READ_WRITE
#define ACC_REPORTING       ZB_ZCL_ATTR_ACCESS_REPORTING
#define ACC_WRITE_OPTIONAL  ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL
#define ACC_SINGLETON       ZB_ZCL_ATTR_ACCESS_SINGLETON
#define ACC_SCENE           ZB_ZCL_ATTR_ACCESS_SCENE
#define ACC_MANUF_SPEC      ZB_ZCL_ATTR_MANUF_SPEC
#define ACC_INTERNAL        ZB_ZCL_ATTR_ACCESS_INTERNAL


static zb_uint8_t dummy_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);


#include "../ota_upgrade_nxp/ota_nxp_definitions.h"

/* Define clusters attributes */
#include "cli_cluster-defs.c"

/* Implement clusters */
#include "cli_cluster-general.c"
#include "cli_cluster-measur-lvlsensor.c"
#include "cli_cluster-ota-upgrading.c"
#include "cli_cluster-hvac.c"
#include "cli_cluster-manuf-spe.c"

/* Manage clusters attributes */
#include "cli_cluster-attributes.c"

/* Updated based on 07-5123-08 Zigbee Cluster Library */
zb_cluster_entry table_clusters[] = {
  /* General */
  { 0x0000, "General",             "Basic",                               "GB", "    ", &cluster_0000 },
  { 0x0001, "General",             "Power Configuration",                 "GPWC", "  ", NULL          },
  { 0x0002, "General",             "Device Temperature Configuration",    "GDTC", "  ", NULL          },
  { 0x0003, "General",             "Identify",                            "GI", "    ", &cluster_0003 },
  { 0x0004, "General",             "Groups",                              "GG", "    ", &cluster_0004 },
  { 0x0005, "General",             "Scenes",                              "GS", "    ", &cluster_0005 },
  { 0x0006, "General",             "ON/OFF",                              "GOF", "   ", &cluster_0006 },
  { 0x0007, "General",             "ON/OFF Switch Configuration",         "GOFSC", " ", &cluster_0007 },
  { 0x0008, "General",             "Level Control",                       "GLC", "   ", NULL          },
  { 0x0009, "General",             "Alarms",                              "GA", "    ", NULL          },
  { 0x000A, "General",             "Time",                                "GT", "    ", NULL          },
  { 0x000B, "General",             "RSSI Location",                       "GRL", "   ", NULL          },
  { 0x000C, "General",             "Analog Input",                        "GAI", "   ", NULL          },
  { 0x000D, "General",             "Analog Output",                       "GAO", "   ", NULL          },
  { 0x000E, "General",             "Analogue Value",                      "GAV", "   ", NULL          },
  { 0x000F, "General",             "Binary Input" ,                       "GBI", "   ", NULL          },
  { 0x0010, "General",             "Binary Output",                       "GBO", "   ", NULL          },
  { 0x0011, "General",             "Binary Value",                        "GBV", "   ", NULL          },
  { 0x0012, "General",             "Multistate Input",                    "GMI", "   ", NULL          },
  { 0x0013, "General",             "Multistate Output",                   "GMO", "   ", NULL          },
  { 0x0014, "General",             "Multistate Value",                    "GMV", "   ", NULL          },
  { 0x0020, "General",             "Poll Control",                        "GPLC", "  ", NULL          },
  { 0x0025, "General",             "Keep Alive",                          "GKA", "   ", NULL          },
  { 0x001A, "General",             "Power Profile",                       "GPP", "   ", NULL          },
  { 0x001C, "General",             "Pulse Width Modulation",              "GPWM", "  ", NULL          },
  { 0x0B01, "General",             "Meter Identification",                "GMI", "   ", NULL          },
  { 0x0B05, "General",             "Diagnostics",                         "GD", "    ", NULL          },
  /* Measurement and Level Sensing */
  { 0x0400, "Measur. & Lvl Sens.", "Illuminance Measurement",             "MIM", "   ", NULL          },
  { 0x0401, "Measur. & Lvl Sens.", "Illuminance Level Sensing",           "MILS", "  ", NULL          },
  { 0x0402, "Measur. & Lvl Sens.", "Temperature Measurement",             "MTM", "   ", &cluster_0402 },
  { 0x0403, "Measur. & Lvl Sens.", "Pressure Measurement",                "MPM", "   ", NULL          },
  { 0x0404, "Measur. & Lvl Sens.", "Flow Measurement",                    "MFM", "   ", NULL          },
  { 0x0405, "Measur. & Lvl Sens.", "Relative Humidity Measurement",       "MRHM", "  ", NULL          },
  { 0x0406, "Measur. & Lvl Sens.", "Occupancy Sensing",                   "MOS", "   ", NULL          },
  { 0x0407, "Measur. & Lvl Sens.", "Leaf Wetness",                        "MLW", "   ", NULL          },
  { 0x0408, "Measur. & Lvl Sens.", "Soil Moisture",                       "MSM", "   ", NULL          },
  { 0x0409, "Measur. & Lvl Sens.", "pH Measurement",                      "MSM", "   ", NULL          },
  { 0x040A, "Measur. & Lvl Sens.", "Electrical Conductivity",             "MEC", "   ", NULL          },
  { 0x040B, "Measur. & Lvl Sens.", "Wind Speed Measurement",              "MWSM", "  ", NULL          },
  { 0x040C, "Measur. & Lvl Sens.", "Carbon Monoxide  Measurement",        "MCMM", "  ", NULL          },
  { 0x040D, "Measur. & Lvl Sens.", "Carbon Dioxide Measurement",          "MCDM", "  ", NULL          },
  { 0x040E, "Measur. & Lvl Sens.", "Ethylene Measurement",                "METM", "  ", NULL          },
  { 0x040F, "Measur. & Lvl Sens.", "Ethylene Oxide Measurement",          "MEOM", "  ", NULL          },
  { 0x0410, "Measur. & Lvl Sens.", "Hydrogen Measurement",                "MHM", "   ", NULL          },
  { 0x0411, "Measur. & Lvl Sens.", "Hydrogen Sulfide Measurement",        "MHSM", "  ", NULL          },
  { 0x0412, "Measur. & Lvl Sens.", "Nitric Oxide Measurement",            "MNOM", "  ", NULL          },
  { 0x0413, "Measur. & Lvl Sens.", "Nitrogen Dioxide Measurement",        "MNDM", "  ", NULL          },
  { 0x0414, "Measur. & Lvl Sens.", "Oxygen Measurement",                  "MO2M", "  ", NULL          },
  { 0x0415, "Measur. & Lvl Sens.", "Ozone Measurement",                   "MO3M", "  ", NULL          },
  { 0x0416, "Measur. & Lvl Sens.", "Sulfur Dioxide Measurement",          "MSDM", "  ", NULL          },
  { 0x0417, "Measur. & Lvl Sens.", "Dissolved Oxygen Measurement",        "MDOM", "  ", NULL          },
  { 0x0418, "Measur. & Lvl Sens.", "Bromate Measurement",                 "MBRM", "  ", NULL          },
  { 0x0419, "Measur. & Lvl Sens.", "Chloramines Measurement",             "MCHM", "  ", NULL          },
  { 0x041A, "Measur. & Lvl Sens.", "Chlorine Measurement",                "MCLM", "  ", NULL          },
  { 0x041B, "Measur. & Lvl Sens.", "Fecal coliform & E.Coli Measurement", "MFCM", "  ", NULL          },
  { 0x041C, "Measur. & Lvl Sens.", "Fluoride Measurement",                "MFLM", "  ", NULL          },
  { 0x041D, "Measur. & Lvl Sens.", "Haloacetic Acids Measurement",        "MHAM", "  ", NULL          },
  { 0x041E, "Measur. & Lvl Sens.", "Total Trihalomethanes Measurement",   "MTTM", "  ", NULL          },
  { 0x041F, "Measur. & Lvl Sens.", "Total Coliform Bacteria Measurement", "MTCM", "  ", NULL          },
  { 0x0420, "Measur. & Lvl Sens.", "Turbidity Measurement",               "MTRM", "  ", NULL          },
  { 0x0421, "Measur. & Lvl Sens.", "Copper Measurement",                  "MCOM", "  ", NULL          },
  { 0x0422, "Measur. & Lvl Sens.", "Lead Measurement",                    "MLDM", "  ", NULL          },
  { 0x0423, "Measur. & Lvl Sens.", "Manganese Measurement",               "MMNM", "  ", NULL          },
  { 0x0424, "Measur. & Lvl Sens.", "Sulfate Measurement",                 "MSFM", "  ", NULL          },
  { 0x0425, "Measur. & Lvl Sens.", "Bromodichloromethane Measurement",    "MBDM", "  ", NULL          },
  { 0x0426, "Measur. & Lvl Sens.", "Bromoform Measurement",               "MBFM", "  ", NULL          },
  { 0x0427, "Measur. & Lvl Sens.", "Chlorodibromomethane Measurement",    "MCBM", "  ", NULL          },
  { 0x0428, "Measur. & Lvl Sens.", "Chloroform Measurement",              "MCRM", "  ", NULL          },
  { 0x0429, "Measur. & Lvl Sens.", "Sodium Measurement",                  "MSOM", "  ", NULL          },
  { 0x042A, "Measur. & Lvl Sens.", "PM2.5 Measurement",                   "MPMM", "  ", NULL          },
  { 0x042B, "Measur. & Lvl Sens.", "Formaldehyde Measurement",            "MFDM", "  ", NULL          },
  { 0x0B04, "Measur. & Lvl Sens.", "Electrical Measurement",              "MELM", "  ", NULL          },
  /* Lighting */
  { 0x0300, "Lighting",            "Color Control",                       "LCC", "   ", NULL          },
  { 0x0301, "Lighting",            "Ballast Configuration",               "LBC", "   ", NULL          },
  /* HVAC */
  { 0x0200, "HVAC",                "Pump Configuration and Control",      "HPCC", "  ", NULL          },
  { 0x0201, "HVAC",                "Thermostat",                          "HT", "    ", &cluster_0201 },
  { 0x0202, "HVAC",                "Fan Control",                         "HFC", "   ", NULL          },
  { 0x0203, "HVAC",                "Dehumidification Control",            "HDC", "   ", NULL          },
  { 0x0204, "HVAC",                "Thermostat User Interface Config",    "HTUIC", " ", NULL          },
  /* Closures */
  { 0x0100, "Closures",            "Shade Configuration",                 "CSH", "   ", NULL          },
  { 0x0101, "Closures",            "Door Lock",                           "CDL", "   ", NULL          },
  { 0x0102, "Closures",            "Window Covering",                     "CWC", "   ", NULL          },
  { 0x0103, "Closures",            "Barrier Control",                     "CBC", "   ", NULL          },
  /* Security and Safety */
  { 0x0500, "Security & Safety",   "IAS Zone",                            "SIZ", "   ", NULL          },
  { 0x0501, "Security & Safety",   "IAS ACE",                             "SIACE", " ", NULL          },
  { 0x0502, "Security & Safety",   "IAS WD",                              "SIWD", "  ", NULL          },
  /* Protocol Interfaces */
  { 0x0016, "Protocol Interfaces", "Partition",                           "PP", "    ", NULL          },
  { 0x0600, "Protocol Interfaces", "Generic Tunnel",                      "PGT", "   ", NULL          },
  { 0x0601, "Protocol Interfaces", "BACnet Protocol Tunnel",              "PBACT", " ", NULL          },
  { 0x0602, "Protocol Interfaces", "Analog Input (BACnet regular)",       "PAIR", "  ", NULL          },
  { 0x0603, "Protocol Interfaces", "Analog Input (BACnet extended)",      "PAIE", "  ", NULL          },
  { 0x0604, "Protocol Interfaces", "Analog Output (BACnet regular)",      "PAOR", "  ", NULL          },
  { 0x0605, "Protocol Interfaces", "Analog Output (BACnet extended)",     "PAOE", "  ", NULL          },
  { 0x0606, "Protocol Interfaces", "Analog Value (BACnet regular)",       "PAVR", "  ", NULL          },
  { 0x0607, "Protocol Interfaces", "Analog Value (BACnet extended)",      "PAVE", "  ", NULL          },
  { 0x0608, "Protocol Interfaces", "Binary Input (BACnet regular)",       "PBIR", "  ", NULL          },
  { 0x0609, "Protocol Interfaces", "Binary Input (BACnet extended)",      "PBIE", "  ", NULL          },
  { 0x060A, "Protocol Interfaces", "Binary Output (BACnet regular)",      "PBOR", "  ", NULL          },
  { 0x060B, "Protocol Interfaces", "Binary Output (BACnet extended)",     "PBOE", "  ", NULL          },
  { 0x060C, "Protocol Interfaces", "Binary Value (BACnet regular)",       "PBVR", "  ", NULL          },
  { 0x060D, "Protocol Interfaces", "Binary Value (BACnet extended)",      "PBVE", "  ", NULL          },
  { 0x060E, "Protocol Interfaces", "Multistate Input (BACnet regular)",   "PMIR", "  ", NULL          },
  { 0x060F, "Protocol Interfaces", "Multistate Input (BACnet extended)",  "PMIE", "  ", NULL          },
  { 0x0610, "Protocol Interfaces", "Multistate Output (BACnet regular)",  "PMIR", "  ", NULL          },
  { 0x0611, "Protocol Interfaces", "Multistate Output (BACnet extended)", "PMIE", "  ", NULL          },
  { 0x0612, "Protocol Interfaces", "Multistate Value (BACnet regular)",   "PMVR", "  ", NULL          },
  { 0x0613, "Protocol Interfaces", "Multistate Value (BACnet extended)",  "PMVE", "  ", NULL          },
  { 0x0614, "Protocol Interfaces", "11073 Protocol Tunnel",               "P73T", "  ", NULL          },
  { 0x0615, "Protocol Interfaces", "ISO7816 Tunnel",                      "PISOT", " ", NULL          },
  /* Smart Energy */
  { 0x0700, "Smart Energy",        "Price",                               "SEPC", "  ", NULL          },
  { 0x0701, "Smart Energy",        "Demand Response and Load Control",    "SEDRL", " ", NULL          },
  { 0x0702, "Smart Energy",        "Metering",                            "SEMTR", " ", NULL          },
  { 0x0703, "Smart Energy",        "Messaging",                           "SEMSG", " ", NULL          },
  { 0x0704, "Smart Energy",        "Tunneling",                           "SET", "   ", NULL          },
  { 0x0705, "Smart Energy",        "Prepayment",                          "SEPP", "  ", NULL          },
  { 0x0707, "Smart Energy",        "Calendar",                            "SECD", "   ", NULL         },
  { 0x0708, "Smart Energy",        "Device Management",                   "SEDM", "   ", NULL         },
  { 0x0709, "Smart Energy",        "Events",                              "SEEV", "   ", NULL         },
  { 0x070B, "Smart Energy",        "Sub-GHz",                             "SESG", "   ", NULL         },
  { 0x0800, "Smart Energy",        "Key Establishment",                   "SEKE", "  ", NULL          },
  { 0x0801, "Smart Energy",        "Meter Identification",                "SEMI", "   ", NULL         },
  /* Over-The-Air Upgrading */
  { 0x0019, "OTA Upgrading",       "OTA Upgrade",                         "OTA", "   ", &cluster_0019 },
  /* Telecom */
  { 0x0900, "Telecom",             "Information",                         "TI", "    ", NULL          },
  { 0x0905, "Telecom",             "Chatting",                            "TC", "    ", NULL          },
  { 0x0904, "Telecom",             "Voice Over ZigBee",                   "TVOZ", "  ", NULL          },
  /* Commissioning */
  { 0x0015, "Commissioning",       "Commissioning",                       "CC", "    ", NULL          },
  { 0x1000, "Commissioning",       "Touchlink",                           "CTL", "   ", NULL          },
  /* Retail */
  { 0x0617, "Retail",              "Retail Tunnel Cluster",               "RTC", "   ", NULL          },
  { 0x0022, "Retail",              "Mobile Device Configuration Cluster", "RMDC", "  ", NULL          },
  { 0x0023, "Retail",              "Neighbor Cleaning Cluster",           "RNC", "   ", NULL          },
  { 0x0024, "Retail",              "Nearest Gateway Cluster",             "RNG", "   ", NULL          },
  /* Appliances */
  { 0x001B, "Appliances",          "EN50523 Appliance Control",           "AAC", "   ", NULL          },
  { 0x0B00, "Appliances",          "EN50523 Appliance Identification",    "AAI", "   ", NULL          },
  { 0x0B02, "Appliances",          "EN50523 Appliance Events and Alerts", "AEA", "   ", NULL          },
  { 0x0B03, "Appliances",          "EN50523 Appliance Statistics",        "AS", "    ", NULL          },
  /* Manuf Specific */
  { 0xFC02, "Custom",             "Test NXP",                             "MSNXP","  ", &cluster_fc02 },
  /* ... */
  { 0xFFFF, NULL,                  NULL,                                  NULL, NULL,   NULL,         }
};


void help_clusters(void)
{
  for(int i=0; table_clusters[i].id != 0xFFFF; i++)
    menu_printf("  0x%04x - %s: %s[%s] %s %s",
      table_clusters[i].id,
      table_clusters[i].initials,
      table_clusters[i].align,
      table_clusters[i].domain,
      table_clusters[i].name,
      (table_clusters[i].definition)?(""):(" => (not implemented)"));
}


/* Global endpoint function
 * init cluster by ep_id
 */
void cluster_init(uint8_t ep_id)
{
  zb_af_endpoint_desc_t *this_ep = endpoint_get_by_id(ep_id);

  if(!this_ep)
    return;

  for(int i=0; i<this_ep->cluster_count; i++)
  {
    /* Init OTA Server */
    if(this_ep->cluster_desc_list[i].cluster_id == ZB_ZCL_CLUSTER_ID_OTA_UPGRADE &&
       this_ep->cluster_desc_list[i].role_mask & ZB_ZCL_CLUSTER_SERVER_ROLE)
      zb_zcl_ota_upgrade_init_server(ep_id, cluster_ota_srv_next_data_ind_cb);
  }
}


/* Commands cluster handlers */
static zb_ret_t cluster_create(int argc, char *argv[]);        static zb_ret_t help_create(void);
static zb_ret_t cluster_bind(int argc, char *argv[]);
static zb_ret_t cluster_unbind(int argc, char *argv[]);
static zb_ret_t cluster_disc_attr(int argc, char *argv[]);
static zb_ret_t cluster_read_attr(int argc, char *argv[]);
static zb_ret_t cluster_write_attr(int argc, char *argv[]);
static zb_ret_t cluster_read_local(int argc, char *argv[]);
static zb_ret_t cluster_write_local(int argc, char *argv[]);
static zb_ret_t cluster_raw_cmd(int argc, char *argv[]);
static zb_ret_t cluster_profile_cmd(int argc, char *argv[]);
static zb_ret_t cluster_zcl_aps_cmd(int argc, char *argv[]);
static zb_ret_t cluster_print(int argc, char *argv[]);

/* Menu cluster */
cli_menu_cmd menu_cluster[] = {
  /* name,                             args,            align, function,                help,              description */
  { "create", " [endpoint] [cluster] [role] <manuf_code>", "                 ", cluster_create,          help_create,       "\r\n\tcreate a cluster on endpoint id [0-255], cluster [0xCCCC or initials] role [server|client], optional manuf_code <0xMMMM>" },
  { "bind", " [endpoint] [ieee] [short] [ep] [cluster] [mode]", "            ", cluster_bind,            help_empty,        "\r\n\tbind a cluster on endpoint id [0-255] to a device at addr [IEEE] [0xSSSS] endpoint [0-255] cluster [0xCCCC or initials] mode [ieee|group]" },
  { "unbind", " [endpoint] [ieee] [short] [ep] [cluster] [mode]", "          ", cluster_unbind,          help_empty,        "\r\n\tunbind a cluster on endpoint id [0-255] to a device at addr [IEEE] [0xSSSS] endpoint [0-255] cluster [0xCCCC or initials] mode [ieee|group]" },
  { "disc_attr", " [endpoint] [addr] [ep] [cluster] [start] [max]", "        ", cluster_disc_attr,       help_empty,        "\r\n\tdiscover attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] starting attr_id [0xAAAA] max number [0-255]" },
  { "read_attr", " [endpoint] [addr] [ep] [cluster] [attr]", "               ", cluster_read_attr,       help_empty,        "\r\n\tread attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] attribute [0xAAAA]" },
  { "write_attr", " [endpoint] [addr] [ep] [cluster] [attr] [type] [val]", " ", cluster_write_attr,      help_empty,        "\r\n\twrite attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] attribute [0xAAAA] type [0xTT] val in hex [VV:VV....VV:VV]" },
  { "read_local", " [endpoint] [cluster] [role] [attr]", "                   ", cluster_read_local,      help_empty,        "\r\n\tread local attribute on endpoint id [0-255] cluster [0xCCCC or initials] role [client|server] attribute [0xAAAA]" },
  { "write_local", " [endpoint] [cluster] [role] [attr] [type] [val]", "     ", cluster_write_local,     help_empty,        "\r\n\twrite local attribute on endpoint id [0-255] cluster [0xCCCC or initials] role [client|server] attribute [0xAAAA] type [0xTT] val in hex [VV:VV....VV:VV]" },
  { "raw_cmd", " [endpoint] [addr] [ep] [cluster] [cmd] <payload>", "        ", cluster_raw_cmd,         help_empty,        "\r\n\tsend ZCL Cluster command on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] command [0xCC] optional payload <P1:P2....Pn>" },
  { "profile_cmd", " [endpoint] [addr] [ep] [cluster] [cmd] <payload>", "    ", cluster_profile_cmd,     help_empty,        "\r\n\tsend ZCL Profile command on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] command [0xCC] optional payload <P1:P2....Pn>" },
  { "aps_cmd", " [endpoint] [addr] [ep] [cluster] [type] [cmd] <payload>", " ", cluster_zcl_aps_cmd,     help_empty,        "\r\n\tsend APS ZCL command on endpoint id [0-255] to a device at dest_addr [0xAAAA] dest_ep [0-255] cluster [0xCCCC or initials] cmd type [0x00|0x01] command [0xCC] optional payload <P1:P2....Pn>" },
  { "onoff_cmd", "", "                                                       ", cluster_onoff_submenu,   help_onoff_cmds,   "SUBMENU ON/OFF Commands" },
  { "groups_cmd", "", "                                                      ", cluster_groups_submenu,  help_groups_cmds,  "SUBMENU GROUPS Commands" },
  { "scenes_cmd", "", "                                                      ", cluster_scenes_submenu,  help_scenes_cmds,  "SUBMENU SCENES Commands" },
  { "thermostat_cmd", "", "                                                  ", cluster_thermo_submenu,  help_thermo_cmds,  "SUBMENU THERMOSTAT Commands" },
  { "custom_nxp_cmd", "", "                                                  ", cluster_custnxp_submenu, help_custnxp_cmds, "SUBMENU CUSTOM NXP Commands" },
  /* Cluster OTA Upgrade commands */
  { "ota_server", "", "                                                      ", cluster_ota_srv_submenu, help_ota_srv_cmds, "SUBMENU OTA Upgrade Server" },
  { "ota_client", "", "                                                      ", cluster_ota_clt_submenu, help_ota_clt_cmds, "SUBMENU OTA Upgrade Client" },
  { "print", " [endpoint]", "                                                ", cluster_print,           help_empty,        "print the cluster table on endpoint id [0-255]" },
  /* Add new commands above here */
  { NULL, NULL,                                                           NULL, NULL,                    NULL,              NULL }
};


/* Static command cluster
 * command create
 *
 * cluster create [endpoint] [cluster] [role] <manuf_code>
 */
static zb_ret_t cluster_create(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t manuf_code = ZB_ZCL_MANUF_CODE_INVALID;
  zb_cluster_entry *new_cluster = NULL;
  zb_uint8_t new_role = 0;
  zb_af_endpoint_desc_t *this_ep = NULL;
  cli_tools_strval role_table[] = {
    { "server", ZB_ZCL_CLUSTER_SERVER_ROLE },
    { "client", ZB_ZCL_CLUSTER_CLIENT_ROLE }
  };

  if(config_is_started())
    return RET_UNAUTHORIZED;

  if(argc < 3 || argc > 4)
    return RET_INVALID_PARAMETER;

  /* get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 1, &new_cluster, ZB_TRUE);

  /* get [role] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 2, role_table, &new_role);

  /* get <manuf_code> */
  if(argc == 4){
    TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 3, &manuf_code);
  }

  /* Args are good, validate them */

  /* Check if that cluster & role already exists */
  for(int i=0; i<this_ep->cluster_count; i++)
    if(this_ep->cluster_desc_list[i].cluster_id == new_cluster->id &&
       this_ep->cluster_desc_list[i].role_mask & new_role)
      return RET_ALREADY_EXISTS;

  /* Check if that cluster can fit in this_ep->simple_desc->app_cluster_list */
  if( this_ep->simple_desc->app_input_cluster_count + this_ep->simple_desc->app_output_cluster_count == MAX_CLUSTERS)
    return RET_NO_RESOURCES;

  /* All servers must be registered before clients */
  if( new_role & ZB_ZCL_CLUSTER_SERVER_ROLE && this_ep->simple_desc->app_output_cluster_count > 0)
    return RET_UNAUTHORIZED;

  /* Params are good, insert that new endpoint in our context */
  {
    zb_zcl_cluster_desc_t *new_cl;

    new_cl = &this_ep->cluster_desc_list[this_ep->cluster_count];
    memset(new_cl, 0, sizeof(zb_zcl_cluster_desc_t));
    new_cl->cluster_id     = new_cluster->id;
    new_cl->attr_count     = new_cluster->definition->attr_count;
    new_cl->attr_desc_list = new_cluster->definition->attr_desc_list;
    new_cl->role_mask      = new_role;
    new_cl->manuf_code     = manuf_code;
    new_cl->cluster_init   = (new_role == ZB_ZCL_CLUSTER_SERVER_ROLE)?(new_cluster->definition->init_server):(new_cluster->definition->init_client);

    this_ep->simple_desc->app_cluster_list[this_ep->cluster_count] = new_cluster->id;

    if(new_role & ZB_ZCL_CLUSTER_SERVER_ROLE)
      this_ep->simple_desc->app_input_cluster_count++;
    if(new_role & ZB_ZCL_CLUSTER_CLIENT_ROLE)
      this_ep->simple_desc->app_output_cluster_count++;
    this_ep->cluster_count++;
  }

  return RET_OK;
}
static zb_ret_t help_create(void)
{
  menu_printf("endpoint: existing endpoints:");
  for(int i=0; i < 256; i++)
  {
    zb_af_endpoint_desc_t *this_ep = endpoint_get_by_id(i);
    if(!this_ep)
      continue;
    menu_printf("  endpoint %u: profile: 0x%04x %s, device: 0x%04x %s, version: %u, %u clusters", 
      this_ep->ep_id,
      this_ep->profile_id,                           /* Also stored in this_ep->simple_desc->app_profile_id */
      get_endpoint_profile_id_str(this_ep->profile_id),
      this_ep->simple_desc->app_device_id,
      get_endpoint_device_id_str(this_ep->simple_desc->app_device_id),
      this_ep->simple_desc->app_device_version,
      this_ep->cluster_count                         /* Should be equal to this_ep->simple_desc->app_input_cluster_count + this_ep->simple_desc->app_output_cluster_count */
      );
  }
  menu_printf("cluster: either its 16bits id value or its initial in:");
  help_clusters();
  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_bind_req
 */
static void bind_cb(zb_uint8_t param)
{
  zb_zdo_bind_resp_t *resp = (zb_zdo_bind_resp_t*)zb_buf_begin(param);

  menu_printf("bind_cb() %s, status %u, tsn %u", (resp->status == ZB_ZDP_STATUS_SUCCESS)?("Ok"):("failed"), resp->status, resp->tsn);

  zb_buf_free(param);
}

/* Static command cluster
 * command bind
 *
 * cluster bind [endpoint] [ieee] [short] [ep] [cluster] [mode]
 */
static zb_ret_t cluster_bind(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t dest_ep_id;
  zb_ieee_addr_t dest_ieee;
  zb_uint16_t dest_short;
  zb_uint8_t dst_addr_mode;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *this_cluster = NULL;
  cli_tools_strval mode_table[] = {
    { "ieee", ZB_BIND_DST_ADDR_MODE_64_BIT_EXTENDED },
    { "group", ZB_BIND_DST_ADDR_MODE_16_BIT_GROUP }
  };

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [ieee] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 1, dest_ieee);

  /* get [short] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 2, &dest_short);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8, argv, 3, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 4, &this_cluster, ZB_TRUE);

  /* get [mode] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 5, mode_table, &dst_addr_mode);

  /* Params are good */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_bind_req_param_t *req;
    uint8_t status;

    buffer = zb_buf_get_out();
//    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_bind_req_param_t));
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_bind_req_param_t);
    ZB_IEEE_ADDR_COPY(req->src_address, dest_ieee);
    req->src_endp      = dest_ep_id;
    req->cluster_id    = this_cluster->id;
    req->dst_addr_mode = dst_addr_mode;
    ZB_IEEE_ADDR_COPY(req->dst_address.addr_long, config_get_ieee_addr());
    req->dst_endp      = this_ep->ep_id;
    req->req_dst_addr  = dest_short;

    status = zb_zdo_bind_req(buffer, bind_cb);
    if(status == ZB_ZDO_INVALID_TSN)
    {
      zb_buf_free(buffer);
      return RET_ERROR;
    }
  }

  return RET_OK;
}


/* Static stack callback function
 * response for zb_zdo_unbind_req
 */
static void unbind_cb(zb_uint8_t param)
{
  zb_zdo_bind_resp_t *resp = (zb_zdo_bind_resp_t*)zb_buf_begin(param);

  menu_printf("unbind_cb() %s, status %u, tsn %u", (resp->status == ZB_ZDP_STATUS_SUCCESS)?("Ok"):("failed"), resp->status, resp->tsn);

  zb_buf_free(param);
}

/* Static command cluster
 * command unbind
 *
 * cluster unbind [endpoint] [ieee] [short] [ep] [cluster] [mode]
 */
static zb_ret_t cluster_unbind(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t dest_ep_id;
  zb_ieee_addr_t dest_ieee;
  zb_uint16_t dest_short;
  zb_uint8_t dst_addr_mode;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *this_cluster = NULL;
  cli_tools_strval mode_table[] = {
    { "ieee", ZB_BIND_DST_ADDR_MODE_64_BIT_EXTENDED },
    { "group", ZB_BIND_DST_ADDR_MODE_16_BIT_GROUP }
  };

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [ieee] */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 1, dest_ieee);

  /* get [short] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 2, &dest_short);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8, argv, 3, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 4, &this_cluster, ZB_TRUE);

  /* get [mode] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 5, mode_table, &dst_addr_mode);

  /* Params are good */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_zdo_bind_req_param_t *req;
    uint8_t status;

    buffer = zb_buf_get_out();
//    req = zb_buf_initial_alloc(buffer, sizeof(zb_zdo_bind_req_param_t));
    req = ZB_BUF_GET_PARAM(buffer, zb_zdo_bind_req_param_t);
    ZB_IEEE_ADDR_COPY(req->src_address, dest_ieee);
    req->src_endp      = dest_ep_id;
    req->cluster_id    = this_cluster->id;
    req->dst_addr_mode = dst_addr_mode;
    ZB_IEEE_ADDR_COPY(req->dst_address.addr_long, config_get_ieee_addr());
    req->dst_endp      = this_ep->ep_id;
    req->req_dst_addr  = dest_short;

    status = zb_zdo_unbind_req(buffer, unbind_cb);
    if(status == ZB_ZDO_INVALID_TSN)
    {
      zb_buf_free(buffer);
      return RET_ERROR;
    }
  }

  return RET_OK;
}


/* Static stack callback function
 * response for write_attr
 */
static void write_attr_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("write_attr_cb() %d: %s", cmd_send_status->status, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}


/* Static command cluster
 * command write_attr
 *
 * cluster write_attr [endpoint] [addr] [ep] [cluster] [attr] [type] [val]
 */
static zb_ret_t cluster_write_attr(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *dest_cluster = NULL;
  zb_uint16_t dest_attr_id;
  zb_uint8_t dest_attr_type;
  zb_uint_t dest_attr_len;
  zb_uint8_t *dest_attr_val;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 7)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &dest_cluster, ZB_FALSE);

  /* get [attr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 4, &dest_attr_id);

  /* get [type] */
  TOOLS_GET_ARG_HEXA(ret, uint8, argv, 5, &dest_attr_type);

  /* get [val len] */
  TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 6, &dest_attr_len);
  dest_attr_val = malloc(dest_attr_len);
  if(!dest_attr_val)
    return RET_NO_MEMORY;
  /* get [val data] */
  ret = tools_arg_get_hex_array_data(argv, 6, dest_attr_len, dest_attr_val);
  if(ret != RET_OK)
  {
    free(dest_attr_val);
    return ret;
  }

  if(dest_attr_len != zb_zcl_get_attribute_size(dest_attr_type, dest_attr_val))
  {
    free(dest_attr_val);
    return RET_INVALID_PARAMETER_7;
  }

  if (!ZB_JOINED())
  {
    free(dest_attr_val);
    return RET_UNAUTHORIZED;
  }


  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_uint8_t *cmd_ptr;

    buffer = zb_buf_get_out();

    /* @brief Initialize Read Attribute Request command
       @params buffer, cmd_ptr, direction, def_resp */
    ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_A(buffer, cmd_ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
    /* @brief Add attribute id to command payload
       @params cmd_ptr, attr_id, attr_type, attr_val */
    ZB_ZCL_GENERAL_ADD_VALUE_WRITE_ATTR_REQ(cmd_ptr, dest_attr_id, dest_attr_type, dest_attr_val);
    /* @brief Sends Write attribute command
       @params buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb */
    ZB_ZCL_GENERAL_SEND_WRITE_ATTR_REQ(buffer, cmd_ptr, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, dest_cluster->id, write_attr_cb);
  }

  free(dest_attr_val);

  return RET_OK;
}


/* Static stack callback function
 * response for disc_attr
 */
static void disc_attr_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  if(cmd_send_status->status != RET_OK)
  {
    menu_printf("disc_attr_cb() failed: %s", wcs_get_error_str(cmd_send_status->status));
  }
  else
  {
    /* Attributes list is not present here,
     * it can be found in endpoint_commands_handler() case ZB_ZCL_CMD_DISC_ATTRIB_RESP
     * that will occurs just after this callback (meanning info not yet available
     */
    if(cmd_send_status->dst_addr.addr_type == ZB_ZCL_ADDR_TYPE_SHORT)
      menu_printf("disc_attr_cb()  status: %s, dest_addr 0x%04x",
        wcs_get_error_str(cmd_send_status->status),
        cmd_send_status->dst_addr.u.short_addr);
    else if(cmd_send_status->dst_addr.addr_type == ZB_ZCL_ADDR_TYPE_IEEE)
      menu_printf("disc_attr_cb()  status: %s, dest_addr "TRACE_FORMAT_64"",
        wcs_get_error_str(cmd_send_status->status),
        TRACE_ARG_64(cmd_send_status->dst_addr.u.ieee_addr));
    else
      menu_printf("disc_attr_cb()  status: %s, dest_addr 0x%08x",
        wcs_get_error_str(cmd_send_status->status),
        cmd_send_status->dst_addr.u.src_id);
  }
  zb_buf_free(param);
}

/* Static command cluster
 * command disc_attr
 *
 * cluster disc_attr [endpoint] [addr] [ep] [cluster] [start] [max]
 */
static zb_ret_t cluster_disc_attr(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *dest_cluster = NULL;
  zb_uint16_t new_start_index;
  zb_uint8_t new_max_number;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &dest_cluster, ZB_FALSE);

  /* get [start] */
  TOOLS_GET_ARG(ret, uint16,  argv, 4, &new_start_index);

  /* get [max] */
  TOOLS_GET_ARG(ret, uint8,  argv, 5, &new_max_number);

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;


  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_uint8_t *cmd_ptr;

    buffer = zb_buf_get_out();

    ZVUNUSED(cmd_ptr);
    /* @brief Discover attribute command
      @params buffer, def_resp, start_attr_id, max_len, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb */
    ZB_ZCL_GENERAL_DISC_READ_ATTR_REQ(buffer, ZB_ZCL_ENABLE_DEFAULT_RESPONSE,
                                      new_start_index,
                                      new_max_number,
                                      dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                                      dest_ep_id,
                                      this_ep->ep_id,
                                      this_ep->profile_id,
                                      dest_cluster->id,
                                      disc_attr_cb);
  }

  return RET_OK;
}


/* Static stack callback function
 * response for read_attr
 */
static void read_attr_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("read_attr_cb() %d: %s", cmd_send_status->status, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}


/* Static command cluster
 * command read_attr
 *
 * cluster read_attr [endpoint] [addr] [ep] [cluster] [attr]
 */
static zb_ret_t cluster_read_attr(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *dest_cluster = NULL;
  zb_uint16_t dest_attr_id;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 5)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &dest_cluster, ZB_FALSE);

  /* get [attr] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 4, &dest_attr_id);

  if (!ZB_JOINED())
    return RET_UNAUTHORIZED;


  /* Do it */
  {
    zb_bufid_t buffer = ZB_BUF_INVALID;
    zb_uint8_t *cmd_ptr;

    buffer = zb_buf_get_out();

    /* @brief Initialize Read Attribute Request command
       @params buffer, cmd_ptr, direction, def_resp */
    ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ_A(buffer, cmd_ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
    /* @brief Add attribute id to command payload
       @params cmd_ptr, attr_id */
    ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, dest_attr_id);
    /* @brief Sends Read attribute command
       @params buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb */
    ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ(buffer, cmd_ptr, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, dest_cluster->id, read_attr_cb);
  }

  return RET_OK;
}


/* Static command cluster
 * command read_local (attribute)
 *
 * cluster read_local [endpoint] [cluster] [role] [attr]
 */
static zb_ret_t cluster_read_local(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t my_ep_id;
  zb_cluster_entry *my_cluster = NULL;
  zb_uint8_t my_cl_role = 0;
  zb_uint16_t my_attr_id;
  cli_tools_strval role_table[] = {
    { "server", ZB_ZCL_CLUSTER_SERVER_ROLE },
    { "client", ZB_ZCL_CLUSTER_CLIENT_ROLE }
  };

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 4)
    return RET_INVALID_PARAMETER;

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &my_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 1, &my_cluster, ZB_FALSE);

  /* get [role] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 2, role_table, &my_cl_role);

  /* get [attr] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 3, &my_attr_id);

  /* Do it */
  {
    zb_zcl_attr_t *attr_desc;
    char AttrInfo[256] = {""};
    int AttrLen = 0;

    attr_desc = zb_zcl_get_attr_desc_a(my_ep_id, my_cluster->id, my_cl_role, my_attr_id);
    if(!attr_desc)
      return RET_NOT_FOUND;

    switch(attr_desc->type) {
      case ZB_ZCL_ATTR_TYPE_U8:     AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "U8: %hhu", ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc));        break;
      case ZB_ZCL_ATTR_TYPE_S8:     AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "S8: %hhd", ZB_ZCL_GET_ATTRIBUTE_VAL_S8(attr_desc));       break;
      case ZB_ZCL_ATTR_TYPE_U16:    AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "U16: %hu", ZB_ZCL_GET_ATTRIBUTE_VAL_16(attr_desc));       break;
      case ZB_ZCL_ATTR_TYPE_S16:    AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "S16: %hd", ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc));      break;
      case ZB_ZCL_ATTR_TYPE_U32:    AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "U32: %u", ZB_ZCL_GET_ATTRIBUTE_VAL_32(attr_desc));        break;
      case ZB_ZCL_ATTR_TYPE_S32:    AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "S32: %d", ZB_ZCL_GET_ATTRIBUTE_VAL_S32(attr_desc));       break;
      case ZB_ZCL_ATTR_TYPE_SINGLE: AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "SINGLE: %f", ZB_ZCL_GET_ATTRIBUTE_VAL_SINGLE(attr_desc)); break;
      default: /* No other macros ZB_ZCL_GET_ATTRIBUTE_VAL_xxx */
        AttrLen += wcs_snprintf(AttrInfo+AttrLen, 256-AttrLen, "VAL: ");
        AttrLen += wcs_snprintf_attr_val(AttrInfo+AttrLen, 256-AttrLen, attr_desc->type, (zb_uint8_t *)attr_desc->data_p);
        break;
    }

    menu_printf("\tRead Local 0x%04x:%s: %s",
      my_attr_id,
      get_cluster_attr_id_str(my_cluster->id, my_attr_id),
      AttrInfo);
  }

  return RET_OK;
}


/* Static command cluster
 * command write_local (attribute)
 *
 * cluster write_local [endpoint] [cluster] [role] [attr] [type] [val]
 */
static zb_ret_t cluster_write_local(int argc, char *argv[])
{
  zb_ret_t ret = RET_OK;
  zb_uint8_t my_ep_id;
  zb_cluster_entry *my_cluster = NULL;
  zb_uint8_t my_cl_role = 0;
  zb_uint16_t my_attr_id;
  zb_uint8_t my_attr_type;
  zb_uint_t my_attr_len;
  zb_uint8_t *my_attr_val;
  cli_tools_strval role_table[] = {
    { "server", ZB_ZCL_CLUSTER_SERVER_ROLE },
    { "client", ZB_ZCL_CLUSTER_CLIENT_ROLE }
  };

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 6)
    return RET_INVALID_PARAMETER;

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &my_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 1, &my_cluster, ZB_FALSE);

  /* get [role] */
  TOOLS_GET_ARG_STRVAL(ret, argv, 2, role_table, &my_cl_role);

  /* get [attr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 3, &my_attr_id);

  /* get [type] */
  TOOLS_GET_ARG_HEXA(ret, uint8, argv, 4, &my_attr_type);

  /* get [val len] */
  TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 5, &my_attr_len);
  my_attr_val = malloc(my_attr_len);
  if(!my_attr_val)
    return RET_NO_MEMORY;
  /* get [val data] */
  ret = tools_arg_get_hex_array_data(argv, 5, my_attr_len, my_attr_val);
  if(ret != RET_OK)
  {
    free(my_attr_val);
    return ret;
  }

  /* Do it */
  {
    zb_zcl_status_t status;
    zb_zcl_attr_t *attr_desc;

    /* Get attr_desc just to test attr_type */
    attr_desc = zb_zcl_get_attr_desc_a(my_ep_id, my_cluster->id, my_cl_role, my_attr_id);
    if(!attr_desc)
    {
      free(my_attr_val);
      return RET_NOT_FOUND;
    }

    if(attr_desc->type != my_attr_type)
      status = ZB_ZCL_STATUS_INVALID_TYPE; /* Type not provided to the API */
    else
      status = zb_zcl_set_attr_val(my_ep_id, my_cluster->id, my_cl_role, my_attr_id, my_attr_val, ZB_TRUE);

     menu_printf("\tWrite Local 0x%04x:%s: Status: %s",
      my_attr_id,
      get_cluster_attr_id_str(my_cluster->id, my_attr_id),
      get_zcl_status_str(status));

      free(my_attr_val);
      
      ret = (status == ZB_ZCL_STATUS_SUCCESS)?(RET_OK):(RET_OPERATION_FAILED);
  }

  return ret;
}


/* Static stack callback function
 * response for raw_cmd
 */
static void raw_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("raw_cmd_cb() %d: %s", cmd_send_status->status, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}


/* Static command cluster
 * command raw_cmd
 *
 * cluster raw_cmd [endpoint] [addr] [ep] [cluster] [cmd] <payload>
 */
static zb_ret_t cluster_raw_cmd(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *dest_cluster = NULL;
  zb_uint16_t dest_raw_cmd;
  zb_uint_t dest_payload_len = 0;
  zb_uint8_t *dest_payload_data = NULL;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 5 && argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &dest_cluster, ZB_FALSE);

  /* get [cmd] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 4, &dest_raw_cmd);

  /* get <payload> */
  if(argc == 6)
  {
    /* get <payload len> */
    TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 5, &dest_payload_len);
    dest_payload_data = malloc(sizeof(zb_uint8_t)*dest_payload_len);
    if(!dest_payload_data)
      return RET_NO_MEMORY;
    /* get <payload data> */
    ret = tools_arg_get_hex_array_data(argv, 5, dest_payload_len, dest_payload_data);
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
    zb_uint8_t *cmd_ptr;

    buffer = zb_buf_get_out();

    cmd_ptr = ZB_ZCL_START_PACKET_REQ(buffer) \
      ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE) \
      ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), dest_raw_cmd);

    if(dest_payload_len > 0)
      for(zb_uint8_t i=0; i<dest_payload_len; i++)
      {
        ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, dest_payload_data[i]);
      }
    ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr) \
      ZB_ZCL_SEND_COMMAND_SHORT(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, dest_cluster->id, raw_cmd_cb);
  }

  if(dest_payload_data) free(dest_payload_data);

  return RET_OK;
}


/* Static stack callback function
 * response for profile_cmd
 */
static void profile_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

  menu_printf("profile_cmd_cb() %d: %s", cmd_send_status->status, wcs_get_error_str(cmd_send_status->status));

  zb_buf_free(param);
}


/* Static command cluster
 * command profile_cmd
 *
 * cluster profile_cmd [endpoint] [addr] [ep] [cluster] [cmd] <payload>
 */
static zb_ret_t cluster_profile_cmd(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint16_t dest_short_addr;
  zb_uint8_t  dest_ep_id;
  zb_af_endpoint_desc_t *this_ep = NULL;
  zb_cluster_entry *dest_cluster = NULL;
  zb_uint16_t dest_raw_cmd;
  zb_uint_t dest_payload_len = 0;
  zb_uint8_t *dest_payload_data = NULL;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 5 && argc != 6)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &dest_cluster, ZB_FALSE);

  /* get [cmd] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 4, &dest_raw_cmd);

  /* get <payload> */
  if(argc == 6)
  {
    /* get <payload len> */
    TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 5, &dest_payload_len);
    dest_payload_data = malloc(sizeof(zb_uint8_t)*dest_payload_len);
     if(!dest_payload_data)
      return RET_NO_MEMORY;
   /* get <payload data> */
    ret = tools_arg_get_hex_array_data(argv, 5, dest_payload_len, dest_payload_data);
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
    zb_uint8_t *cmd_ptr;

    buffer = zb_buf_get_out();

    cmd_ptr = ZB_ZCL_START_PACKET(buffer);
      ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_FALSE, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
      ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), dest_raw_cmd);

    if(dest_payload_len > 0)
      for(zb_uint8_t i=0; i<dest_payload_len; i++)
      {
        ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, dest_payload_data[i]);
      }
    ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr) \
      ZB_ZCL_SEND_COMMAND_SHORT(buffer, dest_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep_id, this_ep->ep_id, this_ep->profile_id, dest_cluster->id, profile_cmd_cb);
  }

  if(dest_payload_data) free(dest_payload_data);

  return RET_OK;
}


/* Static command cluster
* command aps_cmd
*
* cluster aps_cmd [endpoint] [addr] [ep] [cluster] [type] [cmd] <payload>
*/
static zb_ret_t cluster_zcl_aps_cmd(int argc, char *argv[])
{
  zb_ret_t ret = RET_ERROR;
  zb_uint16_t dest_short_addr = 0xFFFF;
  zb_ieee_addr_t dest_ieee_addr = { 0 };
  zb_uint8_t new_ep_id;
  zb_uint8_t dest_ep_id;
  zb_uint16_t new_profile;
  zb_cluster_entry *this_cluster = NULL;
  zb_uint16_t dest_raw_cmd;
  zb_uint_t dest_payload_len = 0;
  zb_uint8_t *dest_payload_data = NULL;

  typedef zb_uint8_t zb_cmd_type_t;
  enum
  {
    common           = 0x00,
    cluster_specific = 0x01
  };
  zb_cmd_type_t cmd_type;

  if(argc < 6)
    return RET_INVALID_PARAMETER;

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &new_ep_id);

  /* get [addr] short or ieee */
  if((ret = tools_arg_get_uint16(argv, 1, &dest_short_addr, ARG_HEX)) != RET_OK &&
     (ret = tools_arg_get_ieee  (argv, 1, dest_ieee_addr,   ARG_HEX)) != RET_OK)
  {
   return ret;
  }

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &dest_ep_id);

  /* get [cluster] */
  TOOLS_GET_ARG_CLUSTER(ret, argv, 3, &this_cluster, ZB_FALSE);

  /* get [cmd_type] */
  TOOLS_GET_ARG_HEXA(ret, uint8, argv, 4, &cmd_type);

  /* get [cmd] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 5, &dest_raw_cmd);

  /* get <payload> */
  if(argc == 7)
  {
    /* get <payload len> */
    TOOLS_GET_ARG_HEX_ARRAY_LEN(ret, argv, 6, &dest_payload_len);
    dest_payload_data = malloc(sizeof(zb_uint8_t)*dest_payload_len);
    if(!dest_payload_data)
      return RET_NO_MEMORY;
    /* get <payload data> */
    ret = tools_arg_get_hex_array_data(argv, 6, dest_payload_len, dest_payload_data);
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
    zb_uint8_t cmd_header_size = 3;
    zb_uint8_t *cmd_ptr;

    cmd_ptr = malloc(cmd_header_size + dest_payload_len);
    if(!cmd_ptr)
      return RET_NO_MEMORY;

    buffer = zb_buf_get_out();

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

    if(cmd_type == common)
      ZB_ZCL_SET_FRAME_TYPE(*cmd_ptr, ZB_ZCL_FRAME_TYPE_COMMON);
    else if(cmd_type == cluster_specific)
      ZB_ZCL_SET_FRAME_TYPE(*cmd_ptr, ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC);
    else
      return RET_INVALID_PARAMETER_5;

    cmd_ptr++;

    /* Add command ID */
    /* ZB_ZCL_CONSTRUCT_COMMAND_HEADER increments pointer */
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER((cmd_ptr), ZB_ZCL_GET_SEQ_NUM(), dest_raw_cmd);

    memcpy(cmd_ptr, dest_payload_data, dest_payload_len);

    /* get profile id from endpoint */
    new_profile = get_profile_id_by_endpoint(new_ep_id);

    ret = zb_aps_send_user_payload(
      buffer,
      addr_addr,                           /* dst_addr */
      new_profile,                         /* profile id */
      this_cluster->id,                    /* cluster id */
      dest_ep_id,                          /* destination endpoint */
      new_ep_id,                           /* source endpoint */
      addr_mode,                           /* addressing mode */
      ZB_TRUE,                             /* Ack enabled */
      cmd_ptr - cmd_header_size,           /* payload */
      cmd_header_size + dest_payload_len);

    if(cmd_ptr) free(cmd_ptr-cmd_header_size);
  }

  if(dest_payload_data) free(dest_payload_data);

  return RET_OK;
}


/* Static command endpoint
 * command print
 *
 * cluster print [endpoint]
 */
static zb_ret_t cluster_print(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_af_endpoint_desc_t *this_ep = NULL;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, 0xFFFF, 0);

  menu_printf("endpoint %u: profile 0x%04x=\'%s\', device 0x%04x=\'%s\', version %u, %u clusters",
    this_ep->ep_id,
    this_ep->profile_id,                           /* Also stored in this_ep->simple_desc->app_profile_id */
    get_endpoint_profile_id_str(this_ep->profile_id),
    this_ep->simple_desc->app_device_id,
    get_endpoint_device_id_str(this_ep->simple_desc->app_device_id),
    this_ep->simple_desc->app_device_version,
    this_ep->cluster_count                         /* Should be equal to this_ep->simple_desc->app_input_cluster_count + this_ep->simple_desc->app_output_cluster_count */
    );

  for(int i=0; i < this_ep->cluster_count; i++)
  {
    zb_zcl_cluster_desc_t *this_cluster = &this_ep->cluster_desc_list[i];

    menu_printf("  cluster %u: %s%s, id 0x%04x=\'%s\', manufacturer 0x%04x, %d attributes",
      i,
      (this_cluster->role_mask & ZB_ZCL_CLUSTER_CLIENT_ROLE)?("Client "):(""),
      (this_cluster->role_mask & ZB_ZCL_CLUSTER_SERVER_ROLE)?("Server "):(""),
      this_cluster->cluster_id,
      get_cluster_id_str(this_cluster->cluster_id),
      this_cluster->manuf_code,
      this_cluster->attr_count-1 /* do not count ZB_ZCL_NULL_ID end of table */
      );

    for(int j=0; j <this_cluster->attr_count-1/* do not count ZB_ZCL_NULL_ID end of table */; j++)
    {
      struct zb_zcl_attr_s *this_attr = &this_cluster->attr_desc_list[j];

      menu_printf("    attr %u: id 0x%04x=\'%s\', type 0x%02x=\'%s\', access 0x%02x=\'%s\', manufacturer 0x%04x",
        j,
        this_attr->id,     get_cluster_attr_id_str(this_cluster->cluster_id, this_attr->id),
        this_attr->type,   get_attr_type_str(this_attr->type),
        this_attr->access, get_attr_access_str(this_attr->access),
        this_attr->manuf_code
        );
    }
  }

  return RET_OK;
}

zb_uint8_t cluster_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* Here, we already know this cluster has been created for this cmd_info->cmd_direction */
  for(int i=0; table_clusters[i].id != 0xFFFF; i++)
  {
    if(table_clusters[i].id == cmd_info->cluster_id)
      return table_clusters[i].definition->commands_handler(cmd_info, param);
  }

  return ZB_FALSE;
}

static zb_uint8_t dummy_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  ZVUNUSED(param);

  menu_printf("Cluster Cmd recv from 0x%04x endpoint %u to %u:%s: DUMMY HANDLER",
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
    ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
    (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"));

  menu_printf("\tcluster 0x%04x:\'%s\' profil 0x%04x:\'%s\' seq_num %d, cmd: 0x%04x:\'%s\'",
    cmd_info->cluster_id, get_cluster_id_str(cmd_info->cluster_id),
    cmd_info->profile_id, get_endpoint_profile_id_str(cmd_info->profile_id),
    cmd_info->seq_number,
    cmd_info->cmd_id, get_cmd_id_str(cmd_info->is_common_command, cmd_info->cluster_id, cmd_info->cmd_id));

  return ZB_FALSE;
}


