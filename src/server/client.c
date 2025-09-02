#include "server/client.h"
#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIENT_IDS_GROWTH_FACTOR

Client *client_create(ServerContext *ctx, int fd) {
  Client *client = malloc(sizeof(Client));
  if (!client) {
    fprintf(stderr, "Failed to allocate memory for client: %d\n", fd);
    return NULL;
  }

  if (ctx->connected_client_ids.capacity == ctx->connected_client_ids.count) {
    size_t new_capacity = ctx->connected_client_ids.capacity *= 2;
    int *new_client_ids = realloc(ctx->connected_client_ids.ids, new_capacity);
    if (!new_client_ids) {
      fprintf(stderr,
              "Failed to reallocate memory for connected client ids: %d\n", fd);
      return NULL;
    }
    ctx->connected_client_ids.ids = new_client_ids;
    ctx->connected_client_ids.capacity = new_capacity;
  }

  ctx->connected_client_ids.ids[ctx->connected_client_ids.count] = fd;

  client->fd = fd;
  client->nickname[0] = '\0';
  client->id_index = ctx->connected_client_ids.count++;
  return client;
}

void client_destroy(ServerContext *ctx, Client *client) {
  ctx->connected_client_ids.ids[client->id_index] =
      ctx->connected_client_ids.ids[ctx->connected_client_ids.count - 1];
  ctx->connected_client_ids.count--;

  free(client);
}

size_t client_get_username(char *buffer, size_t buflen, Client *client) {
  if (client->nickname[0]) {
    return strlcpy(buffer, client->nickname, buflen);
  } else {
    return snprintf(buffer, buflen, "user-%d", client->fd);
  }
}
