#ifndef SERVER_H
#define SERVER_H

#include "server/hashtable.h"
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
  int *ids;
  size_t count;
  size_t capacity;
} ConnectedClientIds;

typedef struct {
  int listenerfd;
  HashTable *connected_clients;
  ConnectedClientIds connected_client_ids;
  int kq;
} ServerContext;

int server_init(ServerContext *ctx, const char *hostname, const char *port);
void server_run(ServerContext *ctx);
void server_shutdown(ServerContext *ctx);

#endif
