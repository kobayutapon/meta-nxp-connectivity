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

#define ZB_TRACE_FILE_ID 33617
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "zboss_api.h"
#include "zboss_api_error.h"
#include "cli_menu.h"
#include "cli_config.h"
#include "cli_network.h"
#include "cli_endpoint.h"
#include "cli_cluster.h"
#include "cli_zdo.h"
#include "cli_installcode.h"
#include "cli_nvram.h"
#include "cli_tools.h"


#define COMMAND_MAX_ARGS 256


typedef struct {
  char         *name;   /* menu name */
  char         *align;  /* spaces to add to align description */
  cli_menu_cmd *entry;  /* menu entry */
  char         *descr;  /* description */
} cli_menu;


static zb_ret_t menu_help(int argc, char *argv[]);  static zb_ret_t help_help(void);
static zb_ret_t menu_sleep(int argc, char *argv[]); static zb_ret_t help_sleep(void);
static zb_ret_t menu_quit(int argc, char *argv[]);

static cli_menu_cmd menu_direct[] = {
  /* name, args,          align, function,   help,       description */
  { "help", "", "             ", menu_help,  help_help,  "this help"             },
  { "sleep", " [duration]", " ", menu_sleep, help_sleep, "delay in seconds"      },
  { "quit", "", "             ", menu_quit,  help_empty, "leave the application" },
  /* Add new commands above here */
  { NULL, NULL,            NULL, NULL,       NULL,       NULL                    }
};

static cli_menu menu[] = {
  { "config",   "             ", menu_config,      "MENU stack configuration & start" },
  { "network",   "            ", menu_network,     "MENU network" },
  { "endpoint",   "           ", menu_endpoint,    "MENU endpoint" },
  { "cluster",   "            ", menu_cluster,     "MENU cluster" },
  { "zdo",   "                ", menu_zdo,         "MENU zdo" },
  { "installcode",   "        ", menu_installcode, "MENU installcode" },
  { "nvram",   "              ", menu_nvram,       "MENU nvram" },
  /* Add new menus above here */
  { NULL,                  NULL, NULL,          NULL }
};

static void menu_process_line(char *line);
static zb_ret_t menu_handle(int argc, char *argv[]);
static zb_ret_t call_cmd_help(char *menu_name, cli_menu_cmd *this_cmd);

void menu_printf(const char *format , ...)
{
  va_list args;
  char msgStr[1024];

  va_start (args, format);
  vsnprintf(msgStr, 1024, format, args);
  va_end (args);

  printf("%s\r\n", msgStr);
}


void menu_process(void)
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


zb_ret_t help_empty(void)
{
  return RET_OK;
}


static void menu_process_line(char *line)
{
  int argc = 0;
  char *argv[COMMAND_MAX_ARGS] = {0};
  char *p;
  zb_ret_t ret;

#ifdef CLI_DEBUG
//  wcs_print_buf((zb_uint8_t *)line, strlen(line), "process_line");
#endif
  if(line[0] == '#')
  {
#ifdef CLI_DEBUG
    WCS_TRACE_DEBUG("ignore line %s", line);
#endif
    return;
  }

  add_history(line);
  /* echo the command */
  menu_printf("command: %s", line);

  p = strtok(line, " ");
  while(p && argc < COMMAND_MAX_ARGS)
  {
    argv[argc++] = p;
    p = strtok(0, " ");
  }

#ifdef CLI_DEBUG
  for(int i=0; i<argc; i++)
    WCS_TRACE_DEBUG("  argv[%d]: %s", i, argv[i]);
#endif

  ret = menu_handle(argc, argv);
  if(ret != RET_OK)
    menu_printf("Error: %s", wcs_get_error_str(ret));
  else
    menu_printf("Ok");
}


static zb_ret_t menu_handle(int argc, char *argv[])
{
  if(argc < 1)
    return RET_INVALID_PARAMETER;

  /* First parse menu_direct */
  for(int j=0; menu_direct[j].func; j++)
  {
    if(!strcmp(argv[0], menu_direct[j].name))
    {
      if(tools_arg_help(argc, argv, 1) == RET_OK)
        return call_cmd_help(menu_direct[j].name, &menu_direct[j]);
      else
        return menu_direct[j].func(argc-1, &argv[1]);
    }
  }

  /* Then parse each menu to match the function name */
  for(int i=0; menu[i].entry; i++)
  {
    /* search for the entry */
    if(!strcmp(argv[0], menu[i].name))
    {
      /* We should have at least <menu> <name> */
      if(argc < 2)
        return RET_INVALID_PARAMETER;

      for(int j=0; menu[i].entry[j].func; j++)
      {
        /* is it help command? */
        if(tools_arg_help(argc, argv, 1) == RET_OK)
          return menu_help(1, argv);

        /* search for the command */
        if(!strcmp(argv[1], menu[i].entry[j].name))
        {
          if(tools_arg_help(argc, argv, 2) == RET_OK)
            return call_cmd_help(menu[i].name, &menu[i].entry[j]);
          else
            return menu[i].entry[j].func(argc-2, &argv[2]);
        }
      }

      /* not found, print help */
      menu_printf("%s %s: unknown command", menu[i].name, argv[1]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
      menu_printf("");
      menu_help(1, argv);
#endif
    }
  }

  return RET_NO_MATCH;
}


static zb_ret_t call_cmd_help(char *menu_name, cli_menu_cmd *this_cmd)
{
  menu_printf("%s %s%s: %s", menu_name, this_cmd->name, this_cmd->args, this_cmd->descr);
  return this_cmd->help();
}


static zb_ret_t menu_help(int argc, char *argv[])
{
  if(argc > 1)
    return RET_INVALID_PARAMETER;

  if(argc == 0)
  {
    for(int i=0; menu[i].entry; i++)
      menu_printf("%s:%s%s", menu[i].name, menu[i].align, menu[i].descr);
    for(int j=0; menu_direct[j].func; j++)
      menu_printf("%s %s:%s %s", menu_direct[j].name, menu_direct[j].args, menu_direct[j].align, menu_direct  [j].descr);
  }
  else
  {
    for(int i=0; menu[i].entry; i++)
      if(!strcmp(argv[0], menu[i].name))
        for(int j=0; menu[i].entry[j].func; j++)
          menu_printf("%s %s%s:%s %s", menu[i].name, menu[i].entry[j].name, menu[i].entry[j].args, menu[i].entry[j].align, menu[i].entry[j].descr);
  }

  return RET_OK;
}
static zb_ret_t help_help(void)
{
  menu_printf("display the list of MENUs & direct commands");
  return RET_OK;
}


/* Static command menu
 * command sleep
 *
 * sleep [delay]
 */
static zb_ret_t menu_sleep(int argc, char *argv[])
{
  zb_ret_t ret;
  unsigned int delay;

  if(argc != 1)
    return RET_INVALID_PARAMETER;

  /* get [delay] */
  TOOLS_GET_ARG(ret, uint, argv, 0, &delay);

  sleep(delay);

  return RET_OK;
}
static zb_ret_t help_sleep(void)
{
  menu_printf("delay for a specified amount of time is seconds");
  return RET_OK;
}


static zb_ret_t menu_quit(int argc, char *argv[])
{
  ZVUNUSED(argv);

  if(argc != 0)
    return RET_INVALID_PARAMETER;

  exit(0);

  return RET_OK;
}
