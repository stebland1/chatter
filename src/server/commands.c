#include "server/commands.h"
#include "server/client.h"
#include "server/messaging.h"
#include "server/server.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

CommandResult handle_command(ServerContext *ctx, char *msgbuf, Client *client,
                             char *username) {
  char *p = msgbuf;
  while (isspace(*p)) {
    p++;
  }

  if (*p != '/') {
    return CMD_NOT_A_COMMAND;
  }

  p++;
  if (strncmp(p, "nick ", 5) == 0) {
    p += 5;
    p[strcspn(p, "\n")] = '\0';

    while (isspace(*p)) {
      p++;
    }

    if (!*p) {
      char *errmsg = "[system]: You must supply a nickname.\n";
      sendall(client->fd, errmsg, strlen(errmsg));
      return CMD_ERROR;
    }

    // "<prev user name> changed their nickname to <p>"
    // TODO: potentially reduce the size of the broadcast messages.
    // 512 seems too large.
    char broadcast_msg_buf[MAX_MSG_LEN];
    snprintf(broadcast_msg_buf, MAX_MSG_LEN,
             "%s changed their nickname to %s.\n", username, p);
    send_message(client->fd, broadcast_msg_buf, ctx);
    strlcpy(client->nickname, p, sizeof(client->nickname));
    return CMD_SUCCESS;
  }

  return CMD_NOT_A_COMMAND;
}
