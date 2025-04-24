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

#ifndef CLI_NXP_MENU_H
#define CLI_NXP_MENU_H 1

#include "zboss_api.h"

#undef CLI_DEBUG

typedef zb_ret_t (*cli_menu_fct)(int argc, char *argv[]);
typedef zb_ret_t (*cli_help_fct)(void);

typedef struct {
  char         *name;   /* name of the command */
  char         *args;   /* list of args */
  char         *align;  /* spaces to add to align description */
  cli_menu_fct func;    /* function to call */
  cli_help_fct help;    /* function to help */
  char         *descr;  /* description */
} cli_menu_cmd;


void menu_process(void);
void menu_printf(const char *format , ...);

zb_ret_t help_empty(void);


#endif /* CLI_NXP_MENU_H */

