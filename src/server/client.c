#include "server/client.h"
#include <stdio.h>
#include <stdlib.h>

Client *client_create(int fd) {
  Client *client = malloc(sizeof(Client));
  if (!client) {
    fprintf(stderr, "Failed to allocate memory for client: %d\n", fd);
    return NULL;
  }

  client->fd = fd;
  client->nickname[0] = '\0';
  return client;
}

void client_destroy(Client *client) { free(client); }
