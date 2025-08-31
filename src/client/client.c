#include "client/input.h"
#include "client/messaging.h"
#include "utils.h"
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int client_connect(const char *hostname, const char *port) {
  struct addrinfo hints, *clientinfo;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int rv = getaddrinfo(hostname, port, &hints, &clientinfo);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  struct addrinfo *p;
  for (p = clientinfo; p != NULL; p = p->ai_next) {
    int sockfd;
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sockfd);
      continue;
    }

    freeaddrinfo(clientinfo);
    return sockfd;
  }

  return -1;
}

int client_run(int serverfd) {
  int stdinfd = fileno(stdin);
  InputBuffer ib = {0};

  Poll *pl = poll_create();
  if (poll_insert_fd(pl, stdinfd) < 0 || poll_insert_fd(pl, serverfd) < 0) {
    return -1;
  }

  while (1) {
    if (poll(pl->pollfds, pl->count, -1) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("poll");
      return -1;
    }

    for (int i = 0; i < (int)pl->count; i++) {
      if (pl->pollfds[i].revents & POLLIN) {
        int fd = pl->pollfds[i].fd;
        if (fd == stdinfd && handle_user_input(serverfd, &ib) == -1) {
          return -1;
        }

        if (fd == serverfd && handle_receive_message(serverfd, &ib) == -1) {
          return -1;
        }
      }
    }
  }

  return 0;
}

void client_shutdown(int serverfd) { close(serverfd); }
