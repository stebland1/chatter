#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "server/server.h"
#include <stddef.h>

#define MAX_NICKNAME_LEN 64

typedef struct {
  int fd;
  char nickname[MAX_NICKNAME_LEN];
  size_t id_index;
} Client;

Client *client_create(ServerContext *ctx, int fd);
void client_destroy(ServerContext *ctx, Client *client);
size_t client_get_username(char *buffer, size_t buflen, Client *client);

#endif
