/*
 * Copyright 2024 NXP
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

#ifndef CLI_NXP_ENDPOINT_H
#define CLI_NXP_ENDPOINT_H 1

#include "zboss_api.h"
#include "cli_menu.h"


typedef struct {
  zb_uint16_t         id;
  char               *name;
  char               *initials;
  char               *align;
} zb_endpoint_profile;

typedef struct {
  zb_uint16_t  id;
  char        *group;
  char        *name;
  char        *initials;
  char        *align;
} zb_endpoint_device;

extern zb_endpoint_profile table_profiles[];
extern zb_endpoint_device endpoint_devices[];

void help_profiles(void);


extern cli_menu_cmd menu_endpoint[];

/* Context containing endpoints table, each endpoint containing clusters table */
extern zb_af_device_ctx_t cli_ctx;

zb_af_endpoint_desc_t *endpoint_get_by_id(zb_uint8_t ep_id);


#endif /* CLI_NXP_ENDPOINT_H */

