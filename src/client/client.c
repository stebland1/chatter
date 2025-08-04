#include "client/input.h"
#include "client/messaging.h"
#include "utils.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
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
  fd_set readfds;
  InputBuffer ib = {0};

  while (1) {
    FD_ZERO(&readfds);
    FD_SET(serverfd, &readfds);
    FD_SET(stdinfd, &readfds);

    int maxfd = stdinfd > serverfd ? stdinfd : serverfd;

    // TODO: rewrite to use poll, which is more performant.
    if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("select");
      return -1;
    }

    if (FD_ISSET(stdinfd, &readfds) && handle_user_input(serverfd, &ib) == -1) {
      return -1;
    }

    if (FD_ISSET(serverfd, &readfds) &&
        handle_receive_message(serverfd, &ib) == -1) {
      return -1;
    }
  }

  return 0;
}

void client_shutdown(int serverfd) { close(serverfd); }
