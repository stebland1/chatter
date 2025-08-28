#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int get_listener_socket(const char *hostname, const char *port) {
  struct addrinfo hints;
  struct addrinfo *servinfo;
  int yes = 1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (!hostname) {
    hints.ai_flags = AI_PASSIVE;
  }

  int rv = getaddrinfo(hostname, port, &hints, &servinfo);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  struct addrinfo *p;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    int listenerfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listenerfd == -1) {
      continue;
    }

    setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listenerfd, p->ai_addr, p->ai_addrlen) < 0) {
      perror("bind");
      close(listenerfd);
      continue;
    }

    if (listen(listenerfd, 10) < 0) {
      perror("listen");
      close(listenerfd);
      continue;
    }

    freeaddrinfo(servinfo);
    return listenerfd;
  }

  freeaddrinfo(servinfo);
  return -1;
}
