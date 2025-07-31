#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "server/server.h"

void close_connection(int fd, ServerContext *ctx);
void handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                           socklen_t *clientaddr_size);
void close_all_fds(ServerContext *ctx);

#endif
