#ifndef SERVER_MESSAGING_H
#define SERVER_MESSAGING_H

#include "client/input.h"

int handle_receive_message(int serverfd, InputBuffer *ib);

#endif
