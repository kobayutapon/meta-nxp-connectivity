/*
 * Copyright 2025 NXP
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

 #define ZB_TRACE_FILE_ID 33622
 #include <stdlib.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <readline/readline.h>
 #include <readline/history.h>
 #include <sys/socket.h>
 #include <linux/un.h>
 #include <unistd.h>
 #include <sys/select.h>
 #include <fcntl.h>
 #include <errno.h>
 #include <pthread.h>


 #define ZBOSS_SOCKET_NAME "/run/zboss.sock"
 #define ZBOSS_CLI_PROMPT  "zbcli> "
 #define BUFFER_SIZE 4096


typedef struct {
  int sock_fd;
  bool wait_for_cmd_resp;
  bool running;
  bool reconnect;
} appli_context_t;


static int connect_socket(char *name, bool verbose);


static int single_cmd(appli_context_t* context, int argc, char *argv[])
{
  int ret;
  char bufData[BUFFER_SIZE] = {0};
  int  bufLen = 0;

  /* If single command execution, concatenate argv */
  for (int i = 1; i < argc; i++)
  {
    strcat(bufData, argv[i]);
    if (i < argc - 1)
      strcat(bufData, " ");
  }
  strcat(bufData, "\n");

  /* Connect to zb_daemon */
  context->sock_fd = connect_socket(ZBOSS_SOCKET_NAME, true);
  if(context->sock_fd == -1)
    return -1;

  /* Send command to server */
  if(!strcmp(bufData,"exit\n"))
    goto out;

  ret = send(context->sock_fd, bufData, strlen(bufData)+1 , 0);
  if (ret == -1)
  {
    perror("send");
    goto out;
  }

  if(!strcmp(bufData,"quit\n"))
    goto out;

  do {
    /* Receive server's response */
    bufLen = 0;
    bufData[bufLen] = '\0';
    while(!strstr(bufData, "\r\n"))
    {
      ret = recv(context->sock_fd, bufData + bufLen, 1, 0);
      if(ret < 0)
      {
        if(errno == EINTR)
          continue;

        goto out;
      }
      bufLen += ret;
      bufData[bufLen] = '\0';
      ret = 0;
    }

    printf("%s", bufData);

    if(strstr(bufData, "Ok\r\n") == bufData || strstr(bufData, "Error: ") == bufData)
      break;
  } while(true);

out:
  if(context->sock_fd)
    close(context->sock_fd);
  context->sock_fd = -1;

  return ret;
}


static int connect_socket(char *name, bool verbose)
{
  struct sockaddr_un sockaddr;
  int fd, ret = 0;

  memset(&sockaddr, 0, sizeof(struct sockaddr_un));

  /* Socket bound to file */
  fd = socket(AF_UNIX, SOCK_STREAM , 0);
  if (fd < 0)
  {
    if(verbose) fprintf(stderr, "Socket failed: %s\n", strerror(errno));
    return -1;
  }

  sockaddr.sun_family = AF_UNIX;
  ret = snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "%s", name);

  if(ret <= 0 || (size_t)ret < strlen(sockaddr.sun_path))
  {
    if(verbose) fprintf(stderr, "%s sun_path too long: %s\n", name, strerror(errno));
    close(fd);
    return -1;
  }

  /* Connect to server */
  ret = connect(fd, (const struct sockaddr *)(&sockaddr), sizeof(struct sockaddr_un));
  if(ret < 0)
  {
    if(verbose) fprintf(stderr, "Connect failed: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  return fd;
}


static void reconnect_socket(appli_context_t* context)
{
  close(context->sock_fd);
  context->sock_fd = -1;
  printf("reconnect daemon...\r\n");
  sleep(2); /* Let application re-start */
  while(context->sock_fd == -1)
  {
    context->sock_fd = connect_socket(ZBOSS_SOCKET_NAME, false);
    usleep(500*1000);
  }
  context->reconnect = 0;
}


static int thread_socket(appli_context_t* context)
{
  int ret = 0;
  char bufData[BUFFER_SIZE];

  /* Connect to zb_daemon */
  context->sock_fd = connect_socket(ZBOSS_SOCKET_NAME, true);
  if(context->sock_fd == -1)
  {
    context->running = false;
    return -1;
  }

  while(context->running)
  {
    size_t bufLen = 0;

    bufData[bufLen] = '\0';

    if (context->reconnect)
      reconnect_socket(context);

    do
    {
      ret = recv(context->sock_fd, bufData+bufLen, 1, 0);
      /* Break on errors */
      if(ret == -1)
      {
        if(errno == EAGAIN)
          continue;

        if(!context->running)
          goto out;

        /*fprintf(stderr, "socket recv(%d,%d) : %s", context->running, context->reconnect, strerror(errno));*/
        continue;
      }
      bufLen += ret;
      bufData[bufLen] = '\0';
    } while(!strstr(bufData, "\r\n"));


    printf("%s", bufData);
    if(context->wait_for_cmd_resp == true)
    {
      if(strstr(bufData, "Ok\r\n") == bufData || strstr(bufData, "Error: ") == bufData)
        context->wait_for_cmd_resp = false;
    }

    if(strstr(bufData, "signal SIGTERM received"))
    {
      context->running = false;
      goto out;
    }
  }

out:
  if(context->sock_fd)
    close(context->sock_fd);
  context->sock_fd = -1;
  return ret;
}


void* thread_readline(void* arg)
{
  appli_context_t* context = (appli_context_t*)arg;
  int ret = 0;
  char buffer[BUFFER_SIZE] = {0};
  char *line, *savep, *p;

  /* Wait socket connection before reading lines from stdin */
  while(context->running && context->sock_fd == -1)
  {
    usleep(200*1000);
  }

  while(context->running)
  {
    /* If multiple command execution, get line */
    line = readline(ZBOSS_CLI_PROMPT);
    if ((line != NULL) && strlen(line) > 0)
    {
      p = strtok_r(line, "\n", &savep);
      while(p)
      {
        if(p[0] == '#')
        {
          /* Ignore line */
          p = strtok_r(0, "\n", &savep);
          continue;
        }

        /* Terminate program if exit command */
        if(!strcmp(p,"exit"))
        {
          context->running = false;
          /* Close socket to exit recv() */
          shutdown(context->sock_fd, SHUT_RDWR);
          close(context->sock_fd);
          context->sock_fd = -1;
          ret = 0;
          break;
        }

        if(!strcmp(p,"quit"))
          context->reconnect = true;

        /* Add line to history */
        add_history(p);
        snprintf(buffer, sizeof(buffer), "%s\n", p);

        /* Send command to server */
        ret = send(context->sock_fd, buffer, strlen(buffer), 0);
        if (ret == -1)
        {
          perror("send");
          context->running = false;
          break;
        }

        if(context->reconnect)
        {
          /* Command quit does not have a response */
          while(context->reconnect)
            usleep(500*1000);
        }
        else
        {
          context->wait_for_cmd_resp = true;
          while(context->wait_for_cmd_resp == true)
            usleep(1000);
        }

        p = strtok_r(0, "\n", &savep);
      }
    }
    if(line)
      free(line);
  }

  pthread_exit((void*)ret);
}


int main(int argc, char *argv[])
{
  int             ret;
  pthread_t       readline;
  appli_context_t context = {
    .sock_fd            = -1,
    .wait_for_cmd_resp  = false,
    .running            = true,
    .reconnect          = false,
  };

  if(argc > 1)
    return single_cmd(&context, argc, argv);

  /* Readline thread creation */
  ret = pthread_create(&readline, NULL, thread_readline, &context);
  if (ret != 0)
  {
    perror("Failed to create socket thread");
    return ret;
  }

  /* Read from server */
  ret = thread_socket(&context);
  pthread_join(readline, (void**)&ret);

  return ret;
}
