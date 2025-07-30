#ifndef MESSAGING_H
#define MESSAGING_H

#include "server.h"

#define MAX_MSG_LEN 512

int handle_client_message(int senderfd, ServerContext *ctx);
void send_message(int senderfd, char *msg, ServerContext *ctx);

#endif
