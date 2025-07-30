#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
  fd_set masterfds;
  fd_set readfds;
  int maxfd;
  int listenerfd;
} ServerContext;

int server_init(ServerContext *ctx, const char *hostname, const char *port);
void server_run(ServerContext *ctx);
void server_shutdown(ServerContext *ctx);

#endif
