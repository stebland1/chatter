#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <stddef.h>

#define MAX_NICKNAME_LEN 64

typedef struct {
  int fd;
  char nickname[MAX_NICKNAME_LEN];
} Client;

Client *client_create(int fd);
void client_destroy(Client *client);
size_t client_get_username(char *buffer, size_t buflen, Client *client);

#endif
