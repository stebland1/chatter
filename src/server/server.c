#include "server/connection.h"
#include "server/messaging.h"
#include "server/socket.h"
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int server_init(ServerContext *ctx, const char *hostname, const char *port) {
  ctx->listenerfd = get_listener_socket(hostname, port);
  if (ctx->listenerfd < 0) {
    return -1;
  }

  if (poll_insert_fd(ctx->poll, ctx->listenerfd) < 0) {
    return -1;
  }

  return 0;
}

void server_run(ServerContext *ctx) {
  while (1) {
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    if (poll(ctx->poll->pollfds, ctx->poll->count, -1) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("poll");
      break;
    }

    for (int i = 0; i < (int)ctx->poll->count; i++) {
      if (ctx->poll->pollfds[i].revents & POLLIN) {
        if (ctx->poll->pollfds[i].fd == ctx->listenerfd) {
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

void server_shutdown(ServerContext *ctx) {
  close_all_fds(ctx);
  poll_destroy(ctx->poll);
}
