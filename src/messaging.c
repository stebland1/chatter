#include "messaging.h"
#include "connection.h"
#include "server.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int handle_client_message(int senderfd, ServerContext *ctx) {
  char buf[MAX_MSG_LEN];
  int bytes_received = recv(senderfd, buf, MAX_MSG_LEN, 0);
  if (bytes_received <= 0) {
    if (bytes_received == 0) {
      // TODO: Relay user X left the chat.
      printf("user-%d left the chat.\n", senderfd);
    }

    perror("recv");
    close_connection(senderfd, ctx);
    return -1;
  }

  buf[bytes_received] = '\0';
  char *msg = malloc(strlen(buf) + 1 /* null */ + 7 + get_num_digits(senderfd));
  if (!msg) {
    fprintf(stderr, "Failed to allocate memory for message\n");
    return -1;
  }
  sprintf(msg, "user-%d: %s", senderfd, buf);
  send_message(senderfd, msg, ctx);
  free(msg);
  return 0;
}

void send_message(int senderfd, char *msg, ServerContext *ctx) {
  // TODO: Apparently send buffers messages
  // So we could end up with bytes still to send.
  for (int i = 0; i <= ctx->maxfd; i++) {
    if (FD_ISSET(i, &ctx->masterfds) && i != senderfd && i != ctx->listenerfd) {
      int bytes_sent = send(i, msg, strlen(msg), 0);
      if (bytes_sent == -1) {
        // TODO: Try to recover on specific errorno's
        close_connection(i, ctx);
      }
    }
  }
}
