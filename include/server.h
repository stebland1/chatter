#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

#define MAX_MSG_LEN 512
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void send_message(int listenerfd, char *msg, int from, int maxfd, fd_set *fds);
void close_all_fds(int maxfds, fd_set *fds);
int get_listener_socket(const char *hostname, const char *port);

#endif
