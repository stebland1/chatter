#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>

typedef struct {
  fd_set readfds;
  int maxfd;
} ClientContext;

int client_connect(const char *hostname, const char *port);
int client_run(int serverfd);
void client_shutdown(int serverfd);

#endif
