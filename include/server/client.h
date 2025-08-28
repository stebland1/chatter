#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#define MAX_NICKNAME_LEN 64

typedef struct {
  int fd;
  char nickname[MAX_NICKNAME_LEN];
} Client;

Client *client_create(int fd);
void client_destroy(Client *client);

#endif
