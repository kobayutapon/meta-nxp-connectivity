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


#include <limits.h>
#include <errno.h>

/* Client config */
#define OTA_UPGRADE_DATA_SIZE 64 /* Max value accepted by the stack, refer to ZB_ZCL_OTA_UPGRADE_QUERY_IMAGE_BLOCK_DATA_SIZE_MAX */

/* Server config */
/* Use fake time in case the IMX is not synchronized by NTP */
#define OTA_UPGRADE_TEST_CURRENT_TIME       0x6660DFE0 /* indicates the current time of the OTA server */
#define OTA_UPGRADE_TEST_UPGRADE_TIME       OTA_UPGRADE_TEST_CURRENT_TIME+1 /* indicates the time that the client SHALL upgrade to running new image */
/* Delta between the 2: duration of Count down state = delay between end of download & finish state */

//#define MAX_OTA_CLIENTS 64
#define MAX_OTA_FILES 20

static zb_uint8_t cluster_ota_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER OTA Upgrade
 *
 * ----------------------------------------------------------------------------------- */

/* -------------------------------- Attributes OTA Upgrade --------------------------- */

/* variable hidden  in macro ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST or ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST_SERVER */
static zb_uint16_t cluster_revision_ota_upgrade_attr_list = ZB_ZCL_OTA_UPGRADE_CLUSTER_REVISION_DEFAULT;

/* Server short address attribute */
static zb_uint16_t server_addr;
/* Server endpoint attribute */
static zb_uint8_t server_ep;

/* Client specific */
zb_zcl_ota_upgrade_client_variable_t client_var_ota_upgrade_attr_list = {
  0,
  ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF,
  1,
  0x0101, /* hardware version */
  OTA_UPGRADE_DATA_SIZE,
  0,
  0
};


/* Server specific */
/* OTA Upgrade server cluster attributes */
static zb_uint8_t query_jitter = ZB_ZCL_OTA_UPGRADE_QUERY_JITTER_MAX_VALUE; /* QueryJitter value */
static zb_uint32_t current_time = OTA_UPGRADE_TEST_CURRENT_TIME;            /* CurrentTime */

/* variable hidden  in macro ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST_SERVER */
static zb_zcl_ota_upgrade_data_t table_ota_upgrade_attr_list[MAX_OTA_FILES];
static zb_zcl_ota_upgrade_server_variable_t server_var_ota_upgrade_attr_list = {
  &query_jitter,
  /* 0, 0,*/
  &current_time,
  /*ZB_ZCL_OTA_UPGRADE_UPGRADE_TIME_DEF_VALUE,*/
  MAX_OTA_FILES,
  table_ota_upgrade_attr_list
};



/* This table is extracted from macro
 * CLIENT:
 *   ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST(ota_upgrade_attr_list, &upgrade_server, 
 *                                                                 &file_offset,
 *                                                                 &file_version,
 *                                                                 &stack_version,
 *                                                                 &downloaded_file_ver,
 *                                                                 &downloaded_stack_ver,
 *                                                                 &image_status,
 *                                                                 &manufacturer,
 *                                                                 &image_type,
 *                                                                 &min_block_period,
 *                                                                 &image_stamp,
 *                                                                 &server_addr,
 *                                                                 &server_ep,
 *                                                                 0x0101, OTA_UPGRADE_DATA_SIZE, ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(ota_upgrade_attr_list, ZB_ZCL_OTA_UPGRADE)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ID                  , &upgrade_server)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_FILE_OFFSET_ID             , &file_offset)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_FILE_VERSION_ID            , &file_version)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_STACK_VERSION_ID           , &stack_version)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_ID , &downloaded_file_ver)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_STACK_VERSION_ID, &downloaded_stack_ver)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STATUS_ID            , &image_status)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_MANUFACTURE_ID             , &manufacturer)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_TYPE_ID              , &image_type)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_MIN_BLOCK_REQUE_ID         , &min_block_period)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STAMP_ID             , &image_stamp)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ADDR_ID             , &server_addr)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ENDPOINT_ID         , &server_ep)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_CLIENT_DATA_ID,         &client_var_ota_upgrade_attr_list)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 * SERVER:
 *   ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST_SERVER(ota_upgrade_attr_list, &query_jitter, &current_time, MAX_OTA_FILES);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(ota_upgrade_attr_list, ZB_ZCL_OTA_UPGRADE)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_DATA_ID, &(server_var_ota_upgrade_attr_list))
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0019[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,              ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&cluster_revision_ota_upgrade_attr_list },
  /* Client attributes */
  { ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ID,                   ZB_ZCL_ATTR_TYPE_IEEE_ADDR, ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.upgrade_server       },
  { ZB_ZCL_ATTR_OTA_UPGRADE_FILE_OFFSET_ID,              ZB_ZCL_ATTR_TYPE_U32,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.file_offset          },
  { ZB_ZCL_ATTR_OTA_UPGRADE_FILE_VERSION_ID,             ZB_ZCL_ATTR_TYPE_U32,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.file_version         },
  { ZB_ZCL_ATTR_OTA_UPGRADE_STACK_VERSION_ID,            ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.stack_version        },
  { ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_ID,  ZB_ZCL_ATTR_TYPE_U32,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.downloaded_file_ver  },
  { ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_STACK_VERSION_ID, ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.downloaded_stack_ver },
  { ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STATUS_ID,             ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.image_status         },
  { ZB_ZCL_ATTR_OTA_UPGRADE_MANUFACTURE_ID,              ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.manufacturer         },
  { ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_TYPE_ID,               ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.image_type           },
  { ZB_ZCL_ATTR_OTA_UPGRADE_MIN_BLOCK_REQUE_ID,          ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.min_block_period     },
  { ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STAMP_ID,              ZB_ZCL_ATTR_TYPE_U32,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&g_general_ota_upgrade_attr.image_stamp          },
  { ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ADDR_ID,              ZB_ZCL_ATTR_TYPE_U16,       ACC_READ_ONLY, NO_MANUF_SPE, (void*)&server_addr                            },
  { ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ENDPOINT_ID,          ZB_ZCL_ATTR_TYPE_U8,        ACC_READ_ONLY, NO_MANUF_SPE, (void*)&server_ep                              },
  /* Client specific private attributes */
  { ZB_ZCL_ATTR_OTA_UPGRADE_CLIENT_DATA_ID,              ZB_ZCL_ATTR_TYPE_NULL,      ACC_INTERNAL,  NO_MANUF_SPE, (void*)&client_var_ota_upgrade_attr_list       },
    /* => used in zb_zcl_ota_upgrade_command.c: function schedule_resend_buffer() */
  /* Server specific private attributes */
  { ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_DATA_ID,              ZB_ZCL_ATTR_TYPE_NULL,      ACC_INTERNAL,  NO_MANUF_SPE, (void*)&server_var_ota_upgrade_attr_list       },
    /* => used in zb_zcl_ota_upgrade_command.c: function get_upgrade_server_variables() */
  /* End of table */
  { ZB_ZCL_NULL_ID,                                      0,                          0,             NO_MANUF_SPE, NULL                                           }
};


static zb_cluster_def cluster_0019 = {
  cluster_attr_0019,
  sizeof(cluster_attr_0019)/sizeof(zb_zcl_attr_t),
  zb_zcl_ota_upgrade_init_srv,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_ota_upgrade_init_cli,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  cluster_ota_commands_handler,
};


/* -------------------------------- Cli commands OTA Upgrade --------------------------- */


static zb_ret_t cluster_ota_srv_add_file(int argc, char *argv[]);
static zb_ret_t cluster_ota_clt_get_file(int argc, char *argv[]);
static zb_ret_t cluster_ota_clt_accept_file(int argc, char *argv[]);
static zb_ret_t cluster_ota_clt_status(int argc, char *argv[]);
#ifdef ZB_STACK_REGRESSION_TESTING_API
static zb_ret_t cluster_ota_clt_auto(int argc, char *argv[]);
static zb_ret_t cluster_ota_clt_tempo(int argc, char *argv[]);
#endif


/* Static command cluster
 * submenu ota_server
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_ota_srv_cmds(void);
#endif
static zb_ret_t help_ota_srv_cmds_detailed(char *subcommand);
static zb_ret_t cluster_ota_srv_submenu(int argc, char *argv[])
{
  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_ota_srv_cmds_detailed(argv[0]);

  /* Search submenu */
  if(!strcmp(argv[0], "add_file"))
    return cluster_ota_srv_add_file(argc-1, &argv[1]);
  /* ... */

  /* not found, print help */
  menu_printf("cluster ota_server %s: unknown subcommand", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_ota_srv_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_ota_srv_cmds_detailed(char *subcommand)
{
  /* SUBMENU OTA Upgrade Server */
  if(!subcommand || !strcmp(subcommand, "add_file"))
  {
    menu_printf("cluster ota_server add_file [endpoint] [file]:");
    menu_printf("\tadd on endpoint id [0-255], file [<path>/<MMMM:manuf>-<TTTT:type>-<VVVVVVVV-version>-<name>.zigbee]");
    if(subcommand) menu_printf("runtime config: insert a new file into Server upgrade table");
  }
  return RET_OK;
}
static zb_ret_t help_ota_srv_cmds(void) { return help_ota_srv_cmds_detailed(NULL); }

/* Static command cluster
 * submenu ota_client
 */
static zb_ret_t help_ota_clt_cmds_detailed(char *subcommand);
static zb_ret_t cluster_ota_clt_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "get_file",    ZB_ZCL_CMD_OTA_NXP_GET_FILE    },
    { "accept_file", ZB_ZCL_CMD_OTA_NXP_ACCEPT_FILE },
    { "status",      ZB_ZCL_CMD_OTA_NXP_STATUS      },
#ifdef ZB_STACK_REGRESSION_TESTING_API
    { "auto",        ZB_ZCL_CMD_OTA_NXP_AUTO        },
    { "tempo",       ZB_ZCL_CMD_OTA_NXP_TEMPO       }
#endif
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
    return help_ota_clt_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_OTA_NXP_GET_FILE:
        return cluster_ota_clt_get_file(argc-1, &argv[1]);
      case ZB_ZCL_CMD_OTA_NXP_ACCEPT_FILE:
        return cluster_ota_clt_accept_file(argc-1, &argv[1]);
      case ZB_ZCL_CMD_OTA_NXP_STATUS:
        return cluster_ota_clt_status(argc-1, &argv[1]);
#ifdef ZB_STACK_REGRESSION_TESTING_API
      case ZB_ZCL_CMD_OTA_NXP_AUTO:
        return cluster_ota_clt_auto(argc-1, &argv[1]);
      case ZB_ZCL_CMD_OTA_NXP_TEMPO:
        return cluster_ota_clt_tempo(argc-1, &argv[1]);
#endif
    }
  }
  /* ... */

  /* not found, print help */
  menu_printf("cluster ota_client %s: unknown subcommand", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_ota_clt_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_ota_clt_cmds_detailed(char *subcommand)
{
  /* SUBMENU OTA Upgrade Client */
  if(!subcommand || !strcmp(subcommand, "get_file"))
  {
    menu_printf("cluster ota_client get_file [addr] [ep] [manuf] [type]:");
    menu_printf("\tget file to server_addr [0xSSSS], endpoint_id [0-255] manufacturer [0xMMMM], image_type [0xTTTT]");
    if(subcommand) menu_printf("runtime config: request OTA upgrade of a specific file to a server");
  }
  if(!subcommand || !strcmp(subcommand, "accept_file"))
  {
    menu_printf("cluster ota_client accept_file [manuf] [type]:");
    menu_printf("\taccept file manufacturer [0xMMMM], image_type [0xTTTT] from frame ZCL OTA Image Notify");
  }
  if(!subcommand || !strcmp(subcommand, "status"))
  {
    menu_printf("cluster ota_client status:");
    menu_printf("\tget status of ota_upgrade");
  }
#ifdef ZB_STACK_REGRESSION_TESTING_API
  if(!subcommand || !strcmp(subcommand, "auto"))
  {
    menu_printf("cluster ota_client auto [enable]:");
    menu_printf("\tdo upgrade on reception of OTA Image Notify if enabled [0|1(default)]");
  }
  if(!subcommand || !strcmp(subcommand, "tempo"))
  {
    menu_printf("cluster ota_client tempo [duration]:");
    menu_printf("\ttemporisation in seconds before starting upgrade, tempo [1-%d(default)-255]",
      ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_INTERVAL/ZB_TIME_ONE_SECOND);
  }
#endif
  return RET_OK;
}
static zb_ret_t help_ota_clt_cmds(void) { return help_ota_clt_cmds_detailed(NULL); }


#define MAX_OTA_CLIENTS 64
#define MAX_OTA_FILES 20

typedef struct zb_zcl_ota_srv_file_s
{
  /* <manufacture code>-<Image Type>-<Version>-<Name>.zigbee */
#define OTA_SRV_NAME_MAX      4+1+4+1+8+1+32+1+6
  char name[PATH_MAX+OTA_SRV_NAME_MAX+1];
  int  size;
  zb_uint8_t *bufptr;
  zb_uint8_t bufsize;
  struct timespec start;
  zb_zcl_ota_upgrade_file_header_t   header;
  zb_zcl_ota_upgrade_file_header_optional_t optional;
  /* ... */
} zb_zcl_ota_srv_file_t;

static zb_zcl_ota_srv_file_t ota_srv_files[MAX_OTA_FILES] = {0};
static zb_uint8_t ota_srv_nb_files = 0;

/* Static command cluster
 * command ota_server
 *
 * cluster ota_server add_file [endpoint] [file]
 */
static zb_ret_t cluster_ota_srv_add_file(int argc, char *argv[])
{
  zb_ret_t ret = RET_ERROR;
  char *new_file;
  zb_af_endpoint_desc_t *this_ep = NULL;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /*  get [endpoint] & validate it has cluster ota_upgrade server */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &this_ep, ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_SERVER_ROLE);

  /* get [file] */
  new_file = argv[1];
  if(access(new_file, F_OK) != 0)
    return RET_INVALID_PARAMETER_2;

  if(ota_srv_nb_files >= MAX_OTA_FILES)
  {
    menu_printf("cannot add more than %d files, abort", MAX_OTA_FILES);
    return RET_NO_RESOURCES;
  }

  /* Do it */
  {
    char *tmp;
    char *file_name;
    zb_zcl_ota_srv_file_t *ota_file = &ota_srv_files[ota_srv_nb_files];
    int iRet;
    FILE *fp;

    file_name = new_file;
    while((tmp = strstr(file_name, "/")) != NULL)
      file_name = tmp+1;

    memset(&ota_file->header, 0, sizeof(ota_file->header));
    iRet = sscanf(file_name, "%04hX-%04hX-%08X-%s.zigbee", &ota_file->header.manufacturer_code, &ota_file->header.image_type, &ota_file->header.file_version, ota_file->header.header_string);
    if(iRet != 4)
    {
      menu_printf("Error: invalid file name \'%s\', should be <MMMM:manuf>-<TTTT:type>-<VVVVVVVV-version>-<name>.zigbee", file_name);
      return RET_INVALID_PARAMETER_2;
    }
    if(strlen(new_file) >= PATH_MAX+OTA_SRV_NAME_MAX)
    {
      menu_printf("Error: file path too long");
      return RET_INVALID_PARAMETER_2;
    }

    /* Check if that file has already been added */
    for(zb_uint8_t i=0; i<ota_srv_nb_files; i++)
    {
      if(ota_file->header.manufacturer_code == ota_srv_files[i].header.manufacturer_code &&
         ota_file->header.image_type        == ota_srv_files[i].header.image_type        &&
         ota_file->header.file_version      == ota_srv_files[i].header.file_version)
      {
        menu_printf("cannot add %s, file already added, abort", file_name);
        return RET_ALREADY_EXISTS;
      }
    }

    snprintf(ota_file->name, sizeof(ota_file->name), "%s", new_file);

    /* Get file size */
    fp = fopen(ota_file->name, "r");
    if(!fp)
    {
      menu_printf("Error %s, cannot open it: %s", file_name, strerror(errno));
      return RET_INVALID_PARAMETER_2;
    }
    fseek(fp, 0L, SEEK_END);
    ota_file->size = ftell(fp);
    fclose(fp);

    /* remove .zigbee in string header */
    tmp = strstr(ota_file->header.header_string, ".zigbee");
    if(tmp) *tmp = '\0';


    /* fill out remain ota header */
    ota_file->header.file_id           = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_ID;
    ota_file->header.header_version    = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_VERSION;
    ota_file->header.header_length     = sizeof(zb_zcl_ota_upgrade_file_header_t);
    ota_file->header.fc                = 0x0000;

    /* Fill here header optional, if any */

    /* bit0: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER)
    {
      zb_uint8_t *credential_version = (zb_uint8_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);

      *credential_version = 0;// Fill here the real value
      ota_file->header.header_length += sizeof(zb_uint8_t);
    }
    /* bit1: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC)
    {
      zb_ieee_addr_t *dst_addr = (zb_ieee_addr_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);

      memset(dst_addr, 0, sizeof(zb_ieee_addr_t));// Fill here the real value
      ota_file->header.header_length += sizeof(zb_ieee_addr_t);
    }
    /* bit2: ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER */
    if(ota_file->header.fc & ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER)
    {
      zb_uint16_t *min_hw_version = (zb_uint16_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length);
      zb_uint16_t *max_hw_version = (zb_uint16_t *)(((zb_uint8_t *)&ota_file->header) + ota_file->header.header_length + sizeof(zb_uint16_t));

      *min_hw_version = 0;// Fill here the real value
      *max_hw_version = 0;// Fill here the real value
      ota_file->header.header_length += 2*sizeof(zb_uint16_t);
    }

    ota_file->header.total_image_size  = ota_file->header.header_length + ota_file->size;

    {
      zb_uint8_t bufid = zb_buf_get_any();
      zb_zcl_ota_upgrade_server_insert_file_t *file_data = ZB_BUF_GET_PARAM(bufid, zb_zcl_ota_upgrade_server_insert_file_t);

      file_data->endpoint     = this_ep->ep_id;
      file_data->index        = ota_srv_nb_files;
      file_data->ota_file     = (zb_uint8_t*)(&ota_file->header);
      file_data->upgrade_time = OTA_UPGRADE_TEST_UPGRADE_TIME;
      file_data->notify_on    = ZB_TRUE;
      ret                     = zb_zcl_ota_upgrade_insert_file(bufid);
    }

    if(ret == RET_OK)
      ota_srv_nb_files++;
  }

  return ret;
}

/* Static command cluster
 * command ota_client
 *
 * cluster ota_client get_file [addr] [ep] [manuf] [type]
 */
static zb_ret_t cluster_ota_clt_get_file(int argc, char *argv[])
{
  zb_ret_t ret;
  uint16_t new_short_addr;
  uint8_t  new_endpoint_id;
  uint16_t new_manufacturer;
  uint16_t new_image_type;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 4)
    return RET_INVALID_PARAMETER;

  /* get [addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 0, &new_short_addr);

  /* get [endpoint] */
  TOOLS_GET_ARG(ret, uint8,   argv, 1, &new_endpoint_id);

  /* get [manuf] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 2, &new_manufacturer);

  /* get [type] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 3, &new_image_type);

  if(g_general_ota_upgrade_attr.image_status != ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE)
    return RET_BUSY;

  /* To be exact we should check if the cluster has been created... */
  g_general_ota_upgrade_attr.manufacturer = new_manufacturer;
  g_general_ota_upgrade_attr.image_type   = new_image_type;

  zb_zcl_ota_upgrade_start_client(new_endpoint_id, new_short_addr);
  /* after ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_INTERVAL, it will start zb_zcl_ota_upgrade_request_server() */

  return RET_OK;
}


/* Static command cluster
 * command ota_client
 *
 * cluster ota_client accept_file [manuf] [type]
 */
static zb_ret_t cluster_ota_clt_accept_file(int argc, char *argv[])
{
  zb_ret_t ret;
  uint16_t new_manufacturer;
  uint16_t new_image_type;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 2)
    return RET_INVALID_PARAMETER;

  /* get [manuf] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 0, &new_manufacturer);

  /* get [type] */
  TOOLS_GET_ARG_HEXA(ret, uint16,  argv, 1, &new_image_type);

  if(g_general_ota_upgrade_attr.image_status != ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE)
    return RET_BUSY;

  /* To be exact we should check if the cluster has been created... */
  g_general_ota_upgrade_attr.manufacturer = new_manufacturer;
  g_general_ota_upgrade_attr.image_type   = new_image_type;

  return RET_OK;
}


/* Static command cluster
 * command ota_client
 *
 * cluster ota_client status
 */
static zb_ret_t cluster_ota_clt_status(int argc, char *argv[])
{
  zb_uint8_t endpoint;
  zb_zcl_attr_t *attr_desc;
//  zb_uint8_t ota_ep;
  zb_uint8_t upgrade_status;
  char *upgrade_status_str = "unknown";

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  ZVUNUSED(argv);

  endpoint = get_endpoint_by_cluster(ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_CLIENT_ROLE);
/*
  attr_desc = zb_zcl_get_attr_desc_a(endpoint, ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ENDPOINT_ID);
  if(!attr_desc)
    return RET_UNINITIALIZED;
  ota_ep = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);
*/
  attr_desc = zb_zcl_get_attr_desc_a(endpoint, ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STATUS_ID);
  if(!attr_desc)
    return RET_UNINITIALIZED;
  upgrade_status = ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc);

  switch(upgrade_status) {
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL:                        upgrade_status_str = "Normal";                            break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DOWNLOADING:                   upgrade_status_str = "Download in progress";              break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DOWNLOADED:                    upgrade_status_str = "Download complete";                 break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_WAITING_UPGRADE:               upgrade_status_str = "Waiting to upgrade";                break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_COUNT_DOWN:                    upgrade_status_str = "Count down";                        break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_WAIT_FOR_MORE:                 upgrade_status_str = "Wait for more";                     break;
  case ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_WAIT_TO_UPGRADE_VIA_EXT_EVENT: upgrade_status_str = "Waiting to Upgrade via Ext. Event"; break;
  }

  /* Note, upgrade_status should be identical to ther cluster attribute image_status */
  menu_printf("ota_client status: %d-%s", upgrade_status, upgrade_status_str);

  return RET_OK;
}

#ifdef ZB_STACK_REGRESSION_TESTING_API
/* Static command cluster
 * command ota_client
 *
 * cluster ota_client auto [enable]
 */
static zb_ret_t cluster_ota_clt_auto(int argc, char *argv[])
{
  zb_ret_t ret;
  uint8_t  new_auto_upgrade;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [enable] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &new_auto_upgrade);
  if(new_auto_upgrade > 1)
    return RET_INVALID_PARAMETER_1;

  zb_zcl_ota_upgrade_client_set_custom_query_jitter((new_auto_upgrade)?(0):(255));
  return RET_OK;
}


/* Static command cluster
 * command ota_client
 *
 * cluster ota_client tempo [duration]
 */
static zb_ret_t cluster_ota_clt_tempo(int argc, char *argv[])
{
  zb_ret_t ret;
  uint8_t  new_tempo;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [duration] */
  TOOLS_GET_ARG(ret, uint8,  argv, 0, &new_tempo);
  if(new_tempo < 1)
    return RET_INVALID_PARAMETER_1;

  zb_zcl_ota_upgrade_client_set_custom_query_timer_interval(new_tempo*ZB_TIME_ONE_SECOND);
  return RET_OK;
}
#endif


/* -------------------------------- Commands OTA Upgrade --------------------------- */

static char *get_src_addr(zb_zcl_addr_t *source)
{
  static char addrStr[128] = "unknown";

  switch(source->addr_type)
  {
  case ZB_ZCL_ADDR_TYPE_SHORT:      snprintf(addrStr, sizeof(addrStr), "%04X", source->u.short_addr);                       break;
  case ZB_ZCL_ADDR_TYPE_SRC_ID_GPD: snprintf(addrStr, sizeof(addrStr), "%08X", source->u.src_id);                           break;
  case ZB_ZCL_ADDR_TYPE_IEEE_GPD:   snprintf(addrStr, sizeof(addrStr), TRACE_FORMAT_64, TRACE_ARG_64(source->u.ieee_addr)); break;
  case ZB_ZCL_ADDR_TYPE_IEEE:       snprintf(addrStr, sizeof(addrStr), TRACE_FORMAT_64, TRACE_ARG_64(source->u.ieee_addr)); break;
  }

  return addrStr;
}


static zb_ret_t cluster_ota_srv_next_data_ind_cb(zb_uint8_t index, zb_zcl_parsed_hdr_t* zcl_hdr, zb_uint32_t offset, zb_uint8_t size, zb_uint8_t** data)
{
/*
  menu_printf("Recv from 0x%04x ota_server next_data_ind index %d, offset %d, size %d",
    get_src_addr(&zcl_hdr->addr_data.common_data.source),
    index,
    offset,
    size);
*/
  zb_zcl_ota_srv_file_t *ota_file;
  FILE *fp;
  zb_uint32_t file_offset = 0;
  size_t      size_to_read = 0;
  zb_uint8_t *file_ptr = NULL;
  size_t read;

  if(index >= ota_srv_nb_files)
  {
    menu_printf("cluster_ota_srv_next_data_ind_cb(%d, ...) ERROR: invalid index (max %d)", index, ota_srv_nb_files);
    return RET_OUT_OF_RANGE;
  }

  ota_file = &ota_srv_files[index];

  /* Buffer to be freed after ZB_ZCL_OTA_UPGRADE_REMOVE_FILE */
  if(!ota_file->bufptr)
  {
    ota_file->bufptr  = malloc(size);
    ota_file->bufsize = size;
  }
  else if(ota_file->bufsize < size)
  {
    ota_file->bufptr  = realloc(ota_file->bufptr, size);
    ota_file->bufsize = size;
  }

  if(!ota_file->bufptr)
  {
    menu_printf("cluster_ota_srv_next_data_ind_cb(%d, ...) ERROR: cannot allocate %d bytes", index, size);
    return RET_NO_MEMORY;
  }

  /* Get header only */
  if(offset+size <= ota_file->header.header_length )
  {
    zb_uint8_t *src = (zb_uint8_t *)&ota_file->header;

    memcpy(ota_file->bufptr, src+offset, size);

    menu_printf("OTA Upgrade Server file %d: deliver to %s header @%d (remains %d)",
      index,
      get_src_addr(&zcl_hdr->addr_data.common_data.source),
      offset,
      ota_file->header.header_length - offset - size);
  }
  /* Get remaining header and beginnning of data */
  else if(offset <= ota_file->header.header_length)
  {
    zb_uint8_t *src = (zb_uint8_t *)&ota_file->header;

    memcpy(ota_file->bufptr, src+offset, ota_file->header.header_length - offset);

    file_offset    = 0;
    size_to_read   = size - (ota_file->header.header_length - offset);
    file_ptr       = ota_file->bufptr + (ota_file->header.header_length - offset);

    menu_printf("OTA Upgrade Server file %d: deliver to %s header @%d (remains %d), %s @%d (remains %d)",
      index,
      get_src_addr(&zcl_hdr->addr_data.common_data.source),
      offset,
      ota_file->header.header_length - offset - size,
      ota_file->header.header_string,
      file_offset,
      ota_file->size - file_offset - size_to_read);
  }
  /* Get remaining of data */
  else
  {
    static size_t lastlog = 0;
    size_t percentage, step;
    file_offset    = offset - ota_file->header.header_length;
    size_to_read   = size;
    file_ptr       = ota_file->bufptr;

    percentage = (file_offset * 100) / ota_file->size;
    if(ota_file->size > 1024*1024) step = 1;
    else if(ota_file->size > 1024) step = 10;
    else                           step = 20;

    if( percentage / step != lastlog || ota_file->size - file_offset - size_to_read == 0)
    {
      struct timespec now, elapsed;
      size_t baud_rate;
      time_t eta;
      zb_uint8_t lqi = ZB_MAC_LQI_UNDEFINED;
      zb_int8_t rssi = ZB_MAC_RSSI_UNDEFINED;

      clock_gettime(CLOCK_BOOTTIME, &now);
      elapsed.tv_sec = now.tv_sec - ota_file->start.tv_sec;
      elapsed.tv_nsec = now.tv_nsec - ota_file->start.tv_nsec;
      if(elapsed.tv_nsec < 0)
      {
        elapsed.tv_sec -= 1;
        elapsed.tv_nsec += 1000000000;
      }
      baud_rate = file_offset / elapsed.tv_sec;
      eta = (ota_file->size - file_offset)/baud_rate;

      zb_zdo_get_diag_data(server_addr, &lqi, &rssi);
      menu_printf("OTA Upgrade Server file %d: deliver to %s %s @%d (remains %d) %d%%  %dB/s  %02d:%02d, lqi: %hd, rssi: %hd",
        index,
        get_src_addr(&zcl_hdr->addr_data.common_data.source),
        ota_file->header.header_string,
        file_offset,
        ota_file->size - file_offset - size_to_read,
        percentage,
        baud_rate,
        eta/60, eta%60,
        lqi,
        rssi);

      lastlog = percentage / step;
    }
  }

  if(size_to_read > 0)
  {
    if(file_offset == 0)
      clock_gettime(CLOCK_BOOTTIME, &ota_file->start);

    fp = fopen(ota_file->name, "r");
    if(!fp)
      return RET_NOT_FOUND;

    fseek(fp, file_offset, SEEK_SET);
    read = fread(file_ptr, 1, size_to_read, fp);
    fclose(fp);
    if(read != size_to_read)
    {
      menu_printf("cluster_ota_srv_next_data_ind_cb(%d, ...) ERROR: cannot read %d bytes (%d)", index, size_to_read, read);
      return RET_NO_MEMORY;
    }
  }

  *data = ota_file->bufptr;

  return RET_OK;
}


typedef struct zb_zcl_ota_clt_file_s
{
  /* OTA-FILE-<date>-<time>-<Name> */
#define OTA_CLT_NAME_MAX      9+6+1+4+1+32
  char filename[PATH_MAX+OTA_CLT_NAME_MAX+1];
  size_t written;
  size_t size;
  FILE *fp;
  struct timespec start;
  zb_zcl_ota_upgrade_file_header_t header;
  zb_zcl_ota_upgrade_file_header_optional_t optional;
} zb_zcl_ota_clt_file_t;

zb_zcl_ota_clt_file_t ota_rx_file;


static void create_ota_clt_file_name(zb_zcl_ota_clt_file_t *ota_file)
{
  struct timespec ts = {0};
  struct tm rtm = {0};

  clock_gettime(CLOCK_REALTIME, &ts);
  localtime_r(&ts.tv_sec, &rtm);

  snprintf(ota_file->filename, sizeof(ota_file->filename),"OTA-FILE-%02d%02d%02d-%02d%02d-%s",
    rtm.tm_mon+1,
    rtm.tm_mday,
    rtm.tm_year-100,
    rtm.tm_hour,
    rtm.tm_min,
    ota_file->header.header_string);
}

static zb_ret_t cluster_ota_clt_device_value_cb(zb_zcl_device_callback_param_t *device_cb_param)
{
  zb_zcl_ota_upgrade_value_param_t *ota_upgrade_value = &(device_cb_param->cb_param.ota_value_param);
  zb_uint32_t file_offset  = 0;
  size_t      written_size = 0;
  zb_uint8_t *file_ptr     = NULL;

  switch (ota_upgrade_value->upgrade_status)
  {
    case ZB_ZCL_OTA_UPGRADE_STATUS_START:

      menu_printf("Device OTA Value Start: manufacturer %04x, type %04x, version %08x, len %d",
        ota_upgrade_value->upgrade.start.manufacturer,
        ota_upgrade_value->upgrade.start.image_type,
        ota_upgrade_value->upgrade.start.file_version,
        ota_upgrade_value->upgrade.start.file_length
        );
      /* Start OTA upgrade. */
      if (g_general_ota_upgrade_attr.image_status == ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL)
      {
        /* Accept image */
        ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
      }
      else
      {
        /* Another download is in progress, deny new image */
        ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_BUSY;
      }
      break;
    case ZB_ZCL_OTA_UPGRADE_STATUS_RECEIVE:
      /* If packet size < header size : Receive header only */
      if(ota_upgrade_value->upgrade.receive.file_offset + ota_upgrade_value->upgrade.receive.data_length < sizeof(zb_zcl_ota_upgrade_file_header_t))
      {
        zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;
        memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_upgrade_value->upgrade.receive.data_length);

        menu_printf("Device OTA Value Recv: header @%d (remains %d)",
          ota_upgrade_value->upgrade.receive.file_offset,
          ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
      }
      /* If packet size > header size : Receive header and beginnning of data */
      else if(ota_upgrade_value->upgrade.receive.file_offset == 0 &&
              ota_upgrade_value->upgrade.receive.file_offset + ota_upgrade_value->upgrade.receive.data_length > sizeof(zb_zcl_ota_upgrade_file_header_t))
      {
        zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;

        memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_upgrade_value->upgrade.receive.data_length);

        create_ota_clt_file_name(&ota_rx_file);
        ota_rx_file.fp = fopen(ota_rx_file.filename,"w+b");

        ota_rx_file.written = 0;
        file_offset         = 0;
        ota_rx_file.size    = ota_rx_file.header.total_image_size - ota_rx_file.header.header_length;
        written_size        = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
        file_ptr            = ota_upgrade_value->upgrade.receive.block_data + (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);            

        menu_printf("Device OTA Value Recv: header @%d (remains %d), %s @%d (remains %d)",
          ota_upgrade_value->upgrade.receive.file_offset,
          ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset,
          ota_rx_file.header.header_string,
          file_offset,
          ota_rx_file.size - file_offset);
      }
      /* If packet size < header size : Receive remaining header and beginnning of data */
      else if(ota_upgrade_value->upgrade.receive.file_offset < ota_rx_file.header.header_length)
      {
        zb_uint8_t *dest = (zb_uint8_t *)&ota_rx_file.header;

        memcpy(dest + ota_upgrade_value->upgrade.receive.file_offset, ota_upgrade_value->upgrade.receive.block_data, ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
        file_offset   = 0;
        written_size  = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
        file_ptr      = ota_upgrade_value->upgrade.receive.block_data + (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);

        create_ota_clt_file_name(&ota_rx_file);
        ota_rx_file.fp      = fopen(ota_rx_file.filename,"w+b");
        ota_rx_file.written = 0;

        menu_printf("Device OTA Value Recv: header @%d (remains %d), %s @%d (remains %d)",
          ota_upgrade_value->upgrade.receive.file_offset,
          ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset,
          ota_rx_file.header.header_string,
          file_offset,
          ota_rx_file.size - file_offset);
      }
      /* Receive remaining of data */
      else
      {
        static size_t lastlog = 0;
        size_t percentage, step;

        file_offset   = ota_upgrade_value->upgrade.receive.file_offset - ota_rx_file.header.header_length;
        written_size  = ota_upgrade_value->upgrade.receive.data_length - (ota_rx_file.header.header_length - ota_upgrade_value->upgrade.receive.file_offset);
        file_ptr      = ota_upgrade_value->upgrade.receive.block_data;

        percentage = (file_offset * 100) / ota_rx_file.size;
        if(ota_rx_file.size > 1024*1024) step = 1;
        else if(ota_rx_file.size > 1024) step = 10;
        else                             step = 20;

        if( percentage / step != lastlog || ota_rx_file.size - file_offset - ota_upgrade_value->upgrade.receive.data_length == 0)
        {
          struct timespec now, elapsed;
          size_t baud_rate;
          time_t eta;
          zb_uint8_t lqi = ZB_MAC_LQI_UNDEFINED;
          zb_int8_t rssi = ZB_MAC_RSSI_UNDEFINED;

          clock_gettime(CLOCK_BOOTTIME, &now);
          elapsed.tv_sec = now.tv_sec - ota_rx_file.start.tv_sec;
          elapsed.tv_nsec = now.tv_nsec - ota_rx_file.start.tv_nsec;
          if(elapsed.tv_nsec < 0)
          {
            elapsed.tv_sec -= 1;
            elapsed.tv_nsec += 1000000000;
          }
          baud_rate = file_offset / elapsed.tv_sec;
          eta = (ota_rx_file.size - file_offset)/baud_rate;

          zb_zdo_get_diag_data(server_addr, &lqi, &rssi);
          menu_printf("Device OTA Value Recv: %s @%d (remains %d) %d%%  %dB/s  %02d:%02d, lqi: %hd, rssi: %hd",
            ota_rx_file.header.header_string,
            file_offset,
            ota_rx_file.size - file_offset - ota_upgrade_value->upgrade.receive.data_length,
            percentage,
            baud_rate,
            eta/60, eta%60,
            lqi,
            rssi);

          lastlog = percentage / step;
        }
      }
      if(written_size)
      {
        if(file_offset == 0)
          clock_gettime(CLOCK_BOOTTIME, &ota_rx_file.start);

        if(file_offset != ota_rx_file.written)
        {
          menu_printf("Device OTA Value Recv: Bad offset %s, should be %d", file_offset, ota_rx_file.written);
        }
        ota_rx_file.written += fwrite(file_ptr, 1, written_size-file_offset, ota_rx_file.fp);
        if(ota_rx_file.written != written_size)
        {
          menu_printf("Device OTA Value Recv: Partial write: %d/%d", ota_rx_file.written, written_size);
        }
      }
      /* Process image block. */
      ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
      break;
    case ZB_ZCL_OTA_UPGRADE_STATUS_CHECK:
      /* Downloading is finished, do additional checks if needed etc before Upgrade End Request. */
      fclose(ota_rx_file.fp);
      if(ota_rx_file.written == ota_rx_file.size)
        menu_printf("Device OTA Value Check: file %s OK", ota_rx_file.filename);
      else
        menu_printf("Device OTA Value Check: file %s is incomplete, missing %d bytes", ota_rx_file.filename, ota_rx_file.size - ota_rx_file.written);

      ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
      break;
    case ZB_ZCL_OTA_UPGRADE_STATUS_APPLY:
      menu_printf("Device OTA Value Apply");
      /* Upgrade End Resp is ok, ZCL checks for manufacturer, image type etc are ok.
          Last step before actual upgrade. */
      ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
      break;
    case ZB_ZCL_OTA_UPGRADE_STATUS_FINISH:
      menu_printf("Device OTA Value Finish");
      /* It is time to upgrade FW. */
      /* Restore cluser attributes: */
      {
        zb_ieee_addr_t default_server = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE;

        memcpy(g_general_ota_upgrade_attr.upgrade_server, default_server, sizeof(default_server));
        g_general_ota_upgrade_attr.file_offset          = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE;
        g_general_ota_upgrade_attr.file_version         = 0x00000000; /* Make sure own version is different from the server to get the file */
        g_general_ota_upgrade_attr.stack_version        = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO;
        g_general_ota_upgrade_attr.downloaded_file_ver  = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE;
        g_general_ota_upgrade_attr.downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE;
        g_general_ota_upgrade_attr.image_status         = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE;
        g_general_ota_upgrade_attr.manufacturer         = OTA_UPGRADE_DEFAULT_MANUFACTURER;
        g_general_ota_upgrade_attr.image_type           = OTA_UPGRADE_DEFAULT_IMAGE_TYPE;
        g_general_ota_upgrade_attr.min_block_period     = 0;
        g_general_ota_upgrade_attr.image_stamp          = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE;
      }
      break;
    case ZB_ZCL_OTA_UPGRADE_STATUS_SERVER_NOT_FOUND:
      menu_printf("Device OTA Value Server not found");
      break;
    default:
      menu_printf("Device OTA Value Unknown device_cb_id %d", ota_upgrade_value->upgrade_status);
      break;
  }

  return RET_OK;
}

static zb_uint8_t cluster_ota_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  ZVUNUSED(cmd_info);
  ZVUNUSED(param);
  /* Nothing to do:
   * Server: it goes throught zb_zcl_ota_upgrade_init_server -> next_data_ind_cb
   * Client: it goes throught ZB_ZCL_REGISTER_DEVICE_CB -> case ZB_ZCL_REGISTER_DEVICE_CB
   */

  return ZB_FALSE;
}

