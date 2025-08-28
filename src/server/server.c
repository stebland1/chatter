#include "server/connection.h"
#include "server/messaging.h"
#include "server/socket.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int server_init(ServerContext *ctx, const char *hostname, const char *port) {
  ctx->listenerfd = get_listener_socket(hostname, port);
  if (ctx->listenerfd < 0) {
    return -1;
  }

  FD_ZERO(&ctx->masterfds);
  FD_ZERO(&ctx->readfds);
  FD_SET(ctx->listenerfd, &ctx->masterfds);
  ctx->maxfd = ctx->listenerfd;
  return 0;
}

void server_run(ServerContext *ctx) {
  while (1) {
    ctx->readfds = ctx->masterfds;
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    // TODO: rewrite to use poll, which is more performant.
    if (select(ctx->maxfd + 1, &ctx->readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("select");
      break;
    }

    for (int i = 0; i <= ctx->maxfd; i++) {
      if (FD_ISSET(i, &ctx->readfds)) {
        if (i == ctx->listenerfd) {
          if (handle_new_connection(ctx, (struct sockaddr *)&clientaddr,
                                    &clientaddr_size) == -1) {
            fprintf(stderr, "Failed to connect\n");
          }
        } else if (handle_client_message(i, ctx) < 0) {
          fprintf(stderr, "Failed to handle message from fd %d\n", i);
          return;
        }
      }
    }
  }
}

void server_shutdown(ServerContext *ctx) { close_all_fds(ctx); }
