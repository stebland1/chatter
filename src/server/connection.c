#include "server/client.h"
#include "server/hashtable.h"
#include "server/messaging.h"
#include "server/server.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <unistd.h>

void handle_delete_client(const void *client, const void *userdata) {
  return client_destroy((ServerContext *)userdata, (Client *)client);
}

/*
 * Performs some clean up after a client disconnects. Either by error or by
 * client request.
 */
void close_connection(int fd, ServerContext *ctx) {
  close(fd);
  ht_delete(ctx->connected_clients, &fd, handle_delete_client, ctx);
}

int handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                          socklen_t *clientaddr_size) {
  int connectionfd = accept(ctx->listenerfd, clientaddr, clientaddr_size);
  if (connectionfd < 0) {
    fprintf(stderr, "Failed to accept connection\n");
    return -1;
  }

  Client *client = client_create(ctx, connectionfd);
  if (!client) {
    return -1;
  }

  if (ht_set(ctx->connected_clients, &connectionfd, client) == -1) {
    fprintf(stderr, "Failed to set client in hashtable: %d\n", connectionfd);
    free(client);
    return -1;
  }

  struct kevent kev;
  EV_SET(&kev, connectionfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(ctx->kq, &kev, 1, NULL, 0, 0) < 0) {
    perror("kevent");
    free(client);
    return -1;
  }

  char broadcast_msg_buf[MAX_MSG_LEN];
  char usernamebuf[MAX_NICKNAME_LEN];
  client_get_username(usernamebuf, MAX_NICKNAME_LEN, client);
  snprintf(broadcast_msg_buf, MAX_MSG_LEN, "%s entered the chat.\n",
           usernamebuf);
  send_message(client->fd, broadcast_msg_buf, ctx);
  return 0;
}

void close_all_fds(ServerContext *ctx) {
  for (size_t i = 0; i < ctx->connected_client_ids.count; i++) {
    close(ctx->connected_client_ids.ids[i]);
  }
}
