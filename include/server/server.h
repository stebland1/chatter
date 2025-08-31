#ifndef SERVER_H
#define SERVER_H

#include "poll.h"
#include "server/client.h"
#include "server/hashtable.h"
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
  int listenerfd;
  HashTable *connected_clients;
  Poll *poll;
} ServerContext;

typedef struct {
  Client *clients;
  size_t count;
  size_t capacity;
} ConnectedClients;

int server_init(ServerContext *ctx, const char *hostname, const char *port);
void server_run(ServerContext *ctx);
void server_shutdown(ServerContext *ctx);

#endif
