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

#include "cli_menu.c"
#include <readline/readline.h>
#include <readline/history.h>


zb_ret_t menu_init(void)
{
  /* Nothing to do */
  return RET_OK;
}

void menu_run(void)
{
  while(!osif_is_term_sig_received())
  {
    char *line;

    line = readline("zbcli> ");

    if(line && strlen(line) > 0)
    {
      char *savep;
      char *p = strtok_r(line, "\n", &savep);
      while(p)
      {
        menu_process_line(p);
        p = strtok_r(0, "\n", &savep);
      }
    }

    free(line);
  }
  menu_printf("signal SIGTERM received, exiting...");
  pthread_exit(NULL);
}

void menu_add_history(char *line)
{
  add_history(line);
}

void menu_shutdown(void)
{
  /* Nothing to do */
}

void menu_printf(const char *format , ...)
{
  va_list args;
  char msgStr[1024];

  va_start (args, format);
  vsnprintf(msgStr, 1024, format, args);
  va_end (args);

  printf("%s\r\n", msgStr);
}

