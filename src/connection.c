#include "server.h"
#include <stdio.h>
#include <unistd.h>

void close_connection(int fd, ServerContext *ctx) {
  close(fd);
  FD_CLR(fd, &ctx->masterfds);

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

void handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                           socklen_t *clientaddr_size) {
  int connectionfd = accept(ctx->listenerfd, clientaddr, clientaddr_size);
  if (connectionfd < 0) {
    return;
  }

  // TODO: Relay user X entered the chat.
  printf("user-%d entered the chat.\n", connectionfd);

  FD_SET(connectionfd, &ctx->masterfds);
  if (connectionfd > ctx->maxfd) {
    ctx->maxfd = connectionfd;
  }
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
