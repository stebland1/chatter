#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define MAX_MSG_LEN 512

int sendall(int fd, void *msgbuf, size_t len);

#endif
