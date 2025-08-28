#include "server/client.h"
#include "server/hashtable.h"
#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_delete_client(const void *client) {
  return client_destroy((Client *)client);
}

/*
 * Performs some clean up after a client disconnects. Either by error or by
 * client request.
 */
void close_connection(int fd, ServerContext *ctx) {
  close(fd);
  FD_CLR(fd, &ctx->masterfds);

  ht_delete(ctx->connected_clients, &fd, handle_delete_client);

  /*
   * Every time we close a connection, we should recalculate the maximum fd.
   * Not ideal, but the select API requires it.
   */
  if (fd == ctx->maxfd) {
    ctx->maxfd = ctx->listenerfd;
    for (int i = 0; i < ctx->maxfd; i++) {
      if (FD_ISSET(i, &ctx->masterfds) && i > ctx->maxfd) {
        ctx->maxfd = i;
      }
    }
  }
}

int handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                          socklen_t *clientaddr_size) {
  int connectionfd = accept(ctx->listenerfd, clientaddr, clientaddr_size);
  if (connectionfd < 0) {
    fprintf(stderr, "Failed to accept connection\n");
    return -1;
  }

  // TODO: Relay user X entered the chat.
  printf("user-%d entered the chat.\n", connectionfd);

  Client *client = client_create(connectionfd);
  if (!client) {
    return -1;
  }

  if (ht_set(ctx->connected_clients, &connectionfd, client) == -1) {
    fprintf(stderr, "Failed to set client in hashtable: %d\n", connectionfd);
    free(client);
    return -1;
  }

  FD_SET(connectionfd, &ctx->masterfds);
  if (connectionfd > ctx->maxfd) {
    ctx->maxfd = connectionfd;
  }

  return 0;
}

void close_all_fds(ServerContext *ctx) {
  for (int i = 0; i <= ctx->maxfd; i++) {
    if (FD_ISSET(i, &ctx->masterfds)) {
      close(i);
    }
  }

  /*
   * The program will most likely terminate after this fn call
   *
   * But just for consistency.
   */
  ctx->maxfd = ctx->listenerfd;
}
