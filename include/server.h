#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <sys/types.h>

#define MAX_MSG_LEN 512
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
  fd_set masterfds;
  fd_set readfds;
  int maxfd;
  int listenerfd;
} ServerContext;

void send_message(int senderfd, char *msg, ServerContext *ctx);
void close_all_fds(ServerContext *ctx);
int get_listener_socket(const char *hostname, const char *port);
int handle_client_message(int senderfd, ServerContext *ctx);
void handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                           socklen_t *clientaddr_size);

#endif
