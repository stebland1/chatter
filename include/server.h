#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

void close_all_fds(int maxfds, fd_set *fds);

#endif
