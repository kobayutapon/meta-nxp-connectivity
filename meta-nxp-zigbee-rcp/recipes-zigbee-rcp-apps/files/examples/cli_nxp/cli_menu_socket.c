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
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define ZBOSS_SOCKET_NAME "/run/zboss.sock"

#ifdef CLI_DEBUG
#define SOCKET_TRACE_DEBUG(format, ...)                WCS_TRACE_DEBUG(format, ##__VA_ARGS__)
#define SOCKET_TRACE_WARNING(format, ...)              WCS_TRACE_WARNING(format, ##__VA_ARGS__)
#define socket_print_buf(buffer, len, format, ...)     wcs_print_buf(buffer, len, format, ##__VA_ARGS__)
#else
#define SOCKET_TRACE_DEBUG(format, ...)                do {} while(0)
#define SOCKET_TRACE_WARNING(format, ...)              do {} while(0)
#define socket_print_buf(buffer, len, format, ...)     do {} while(0)
#endif


static int sock_server = -1;
static int sock_client = -1;

static int configure_socket_server(char *name, int nb_connections);
static char *read_socket_client(int socket);


zb_ret_t menu_init(void)
{
  sock_server = configure_socket_server(ZBOSS_SOCKET_NAME, 1);
  if(sock_server == -1)
  {
    WCS_TRACE_ERROR("Cannot initialize socket server: %s", strerror(errno));
    return RET_OPERATION_FAILED;
  }

  return RET_OK;
}

void menu_run(void)
{
  while(!osif_is_term_sig_received())
  {
    char *line;

    if(sock_client == -1)
    {
      sock_client = accept(sock_server, NULL, NULL);
      if(sock_client == -1)
      {
        usleep(100*1000);
        continue;
      }
      else
      {
        SOCKET_TRACE_DEBUG("accept: %d, %s", sock_client, strerror(errno));
      }

      WCS_TRACE_DBGREL("cli-client connected");
    }

    line = read_socket_client(sock_client);
    if(!line)
    {
      WCS_TRACE_DBGREL("cli-client disconnected");
      close(sock_client);
      sock_client = -1;
      continue;
    }

    if(line && strlen(line) > 0)
    {
      char *savep;
      char *p = strtok_r(line, "\n", &savep);
      while(p)
      {
        WCS_TRACE_DBGREL("cli-client> %s", p);
        menu_process_line(p);
        p = strtok_r(0, "\n", &savep);
      }
    }

    free(line);
  }

  pthread_exit(NULL);
}

void menu_add_history(char *line)
{
  /* Nothing to do */
  ZVUNUSED(line);
}

void menu_shutdown(void)
{
  if(sock_client != -1)  close(sock_client);
  if(sock_server != -1)  close(sock_server);
}

void menu_printf(const char *format , ...)
{
  va_list args;
  char msgStr[1024];

  va_start (args, format);
  vsnprintf(msgStr, 1024, format, args);
  va_end (args);

  if(sock_client == -1)
  {
    WCS_TRACE_DBGREL("lost: %s", msgStr);
  }
  else
  {
    dprintf(sock_client, "%s\r\n", msgStr);
  }
}


/* Configure socket server for zb_cli-ctl */
static int configure_socket_server(char *name, int nb_connections)
{
  int fd;
  int ret;
  struct sockaddr_un sockaddr;

  /* Create listening socket */
  fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
  if(fd < 0)
  {
    WCS_TRACE_WARNING("Cannot create socket: %s", strerror(errno));
    return -1;
  }

  sockaddr.sun_family = AF_UNIX;
  snprintf(sockaddr.sun_path, UNIX_PATH_MAX, "%s", name);
  /* Remove file if already exist */
  unlink(name);

  /* Bind socket to file */
  ret = bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
  if(ret < 0)
  {
    WCS_TRACE_WARNING("Cannot bind socket %s: %s", sockaddr.sun_path, strerror(errno));
    close(fd);
    return -1;
  }

  /* Accept nb_connections */
  ret = listen(fd, nb_connections);
  if(ret < 0)
  {
    WCS_TRACE_WARNING("Cannot listen socket %s (use only console): %s", name, strerror(errno));
    close(fd);
    return -1;
  }

  return fd;
}


static char *read_socket_client(int socket)
{
  int maxSockLen = 1024;
  int len = 0;
  char *sockLine = malloc(maxSockLen);

  do
  {
    fd_set read_fds;
    fd_set except_fds;
    struct timeval timeout = {
      .tv_sec = 1,
      .tv_usec = 0,
    };
    int ret;

    FD_ZERO(&read_fds);   FD_SET(socket, &read_fds);
    FD_ZERO(&except_fds); FD_SET(socket, &except_fds);

    SOCKET_TRACE_DEBUG("select socket entry");
    ret = select(socket+1, &read_fds, NULL, &except_fds, &timeout);
    SOCKET_TRACE_DEBUG("select socket exit: ret %d read %d except %d", ret, FD_ISSET(socket, &read_fds), FD_ISSET(socket, &except_fds));
    if(ret == -1)
    {
      SOCKET_TRACE_WARNING("select socket failed: %s", strerror(errno));
      free(sockLine);
      return NULL;
    }
    if(FD_ISSET(socket, &except_fds))
    {
      SOCKET_TRACE_WARNING("select socket exception: %s", strerror(errno));
      free(sockLine);
      return NULL;
    }
    if(FD_ISSET(socket, &read_fds))
    {
      ret = read(socket, sockLine+len, maxSockLen-len-1);
      if(ret > 0)
      {
        len += ret;
        sockLine[len] = '\0';
        socket_print_buf((zb_uint8_t *)sockLine, len, "read socket: ret %d", ret);
        return sockLine;
      }
      else
      {
        /* Error management */
        if(errno == EINTR)
          continue;

        SOCKET_TRACE_WARNING("Cannot read  socket "ZBOSS_SOCKET_NAME": %s", strerror(errno));
        free(sockLine);
        return NULL;
      }
    }
  } while(!osif_is_term_sig_received());

  if(osif_is_term_sig_received())
  {
    menu_printf("signal SIGTERM received, exiting...");
    shutdown(socket, SHUT_RDWR);
  }

  free(sockLine);
  return NULL;
}

