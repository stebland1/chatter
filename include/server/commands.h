#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include "server/client.h"
#include "server/server.h"

typedef enum {
  CMD_NOT_A_COMMAND = 0,
  CMD_SUCCESS = 1,
  CMD_ERROR = 2,
} CommandResult;

CommandResult handle_command(ServerContext *ctx, char *msgbuf, Client *client,
                             char *username);

#endif
