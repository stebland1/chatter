#ifndef SERVER_MESSAGING_H
#define SERVER_MESSAGING_H

#include "server/server.h"

int handle_client_message(int senderfd, ServerContext *ctx);
void send_message(int senderfd, char *msg, ServerContext *ctx);

#endif
