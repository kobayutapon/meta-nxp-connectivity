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

#ifndef CLI_NXP_CLUSTER_H
#define CLI_NXP_CLUSTER_H 1

#include "zboss_api.h"


/* if it returns false, the command will be manage by the stack
 * if it returns true, the command will be ignore by the stack */
typedef zb_uint8_t (*commands_handler_t)(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);


typedef struct {
  zb_zcl_attr_t         *attr_desc_list;
  zb_uint16_t            attr_count;
  zb_zcl_cluster_init_t  init_server;
  zb_zcl_cluster_init_t  init_client;
  commands_handler_t     commands_handler;
} zb_cluster_def;

typedef struct {
  zb_uint16_t     id;
  char           *domain;
  char           *name;
  char           *initials;
  char           *align;
  zb_cluster_def *definition;
} zb_cluster_entry;


extern zb_cluster_entry table_clusters[];

extern cli_menu_cmd menu_cluster[];

void help_clusters(void);

void cluster_init(uint8_t ep_id);

void cluster_attributes_cb(zb_uint8_t param);

zb_uint8_t cluster_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

int wcs_snprintf_attr_val(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value);
int wcs_snprintf_attr_raw(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value);


#endif /* CLI_NXP_CLUSTER_H */

