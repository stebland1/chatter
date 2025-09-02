#include "client/input.h"
#include "client/messaging.h"
#include "utils.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
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
  InputBuffer ib = {0};
  int stdinfd = fileno(stdin);

  struct kevent kev[2];
  EV_SET(&kev[0], stdinfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  EV_SET(&kev[1], serverfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  int kq = kqueue();
  struct timespec ts;
  ts.tv_nsec = ts.tv_sec = 0;

  if (kevent(kq, kev, 2, NULL, 0, &ts) < 0) {
    perror("kevent");
    return -1;
  }

  while (1) {
    int nev;
    if ((nev = kevent(kq, NULL, 0, kev, 2, NULL)) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("kevent");
      return -1;
    }

    for (int i = 0; i < nev; i++) {
      if (kev[i].ident == (uintptr_t)stdinfd &&
          handle_user_input(serverfd, &ib) == -1) {
        return -1;
      }

      if (kev[i].ident == (uintptr_t)serverfd &&
          handle_receive_message(serverfd, &ib) == -1) {
        return -1;
      }
    }
  }

  return 0;
}

void client_shutdown(int serverfd) { close(serverfd); }
