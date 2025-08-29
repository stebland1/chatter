#include "server/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

size_t client_get_username(char *buffer, size_t buflen, Client *client) {
  if (client->nickname[0]) {
    return strlcpy(buffer, client->nickname, buflen);
  } else {
    return snprintf(buffer, buflen, "user-%d", client->fd);
  }
}
