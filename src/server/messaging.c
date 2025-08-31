#include "server/messaging.h"
#include "poll.h"
#include "server/client.h"
#include "server/commands.h"
#include "server/connection.h"
#include "server/hashtable.h"
#include "server/server.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int handle_client_message(int poll_index, ServerContext *ctx) {
  int senderfd = ctx->poll->pollfds[poll_index].fd;

  char msgbuf[MAX_MSG_LEN];
  int bytes_received = recv(senderfd, msgbuf, MAX_MSG_LEN, 0);

  Client *client = ht_get(ctx->connected_clients, &senderfd);
  char usernamebuf[MAX_NICKNAME_LEN];
  client_get_username(usernamebuf, MAX_NICKNAME_LEN, client);

  if (bytes_received <= 0) {
    close_connection(poll_index, ctx);
    if (bytes_received == 0) {
      char broadcast_msg_buf[MAX_MSG_LEN];
      snprintf(broadcast_msg_buf, MAX_MSG_LEN, "%s has left the chat.\n",
               usernamebuf);
      send_message(client->fd, broadcast_msg_buf, ctx);
      return 0;
    }

    perror("recv");
    return -1;
  }

  msgbuf[bytes_received] = '\0';
  if (handle_command(ctx, msgbuf, client, usernamebuf) != CMD_NOT_A_COMMAND) {
    return 0; // don't broadcast commands.
  }

  size_t delimiter_len = 2;
  char *msg = malloc(strlen(usernamebuf) + delimiter_len + strlen(msgbuf) +
                     /* null */ 1);
  if (!msg) {
    fprintf(stderr, "Failed to allocate memory for message\n");
    return -1;
  }

  sprintf(msg, "%s: %s", usernamebuf, msgbuf);
  send_message(senderfd, msg, ctx);
  free(msg);
  return 0;
}

void send_message(int senderfd, char *msg, ServerContext *ctx) {
  // TODO: Apparently 'send' buffers messages
  // So we could end up with bytes still to send.
  for (size_t i = 0; i < ctx->poll->count; i++) {
    int fd = ctx->poll->pollfds[i].fd;
    if (fd != senderfd && fd != ctx->listenerfd) {
      int bytes_sent = sendall(fd, msg, strlen(msg));
      if (bytes_sent == -1) {
        // TODO: Try to recover on specific errorno's
        close_connection(i, ctx);
      }
    }
  }
}
