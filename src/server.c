#include "server.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void close_all_fds(int maxfds, fd_set *fds) {
  for (int i = 0; i <= maxfds; i++) {
    if (FD_ISSET(i, fds)) {
      close(i);
    }
  }
}

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
      close(listenerfd);
      continue;
    }

    if (listen(listenerfd, 10) < 0) {
      close(listenerfd);
      continue;
    }

    freeaddrinfo(servinfo);
    return listenerfd;
  }

  freeaddrinfo(servinfo);
  return -1;
}

void send_message(int listenerfd, char *msg, int from, int maxfd, fd_set *fds) {
  // TODO: Apparently send buffers messages
  // So we could end up with bytes still to send.
  for (int i = 0; i <= maxfd; i++) {
    if (FD_ISSET(i, fds) && i != from && i != listenerfd) {
      int bytes_sent = send(i, msg, strlen(msg), 0);
      if (bytes_sent == -1) {
        perror("send");
      }
    }
  }
}

int main(int argc, char **argv) {
  const char *port, *hostname = NULL;
  switch (argc) {
  case 2:
    port = argv[1];
    break;
  case 3:
    hostname = argv[1];
    port = argv[2];
    break;
  default:
    fprintf(stderr, "Usage: %s [hostname] port\n", argv[0]);
    return EXIT_FAILURE;
  }

  int listenerfd = get_listener_socket(hostname, port);
  if (listenerfd < 0) {
    fprintf(stderr, "Error initializing listener: %d\n", listenerfd);
    return EXIT_FAILURE;
  }

  fd_set masterfds, readfds;
  int maxfd = listenerfd;
  FD_ZERO(&masterfds);
  FD_ZERO(&readfds);
  FD_SET(listenerfd, &masterfds);

  while (1) {
    readfds = masterfds;
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        close_all_fds(maxfd, &masterfds);
        fprintf(stderr, "Error selecting ready file desriptors\n");
        return EXIT_FAILURE;
      }
    }

    for (int i = 0; i <= maxfd; i++) {
      if (FD_ISSET(i, &readfds)) {
        if (i == listenerfd) {
          int connectionfd = accept(listenerfd, (struct sockaddr *)&clientaddr,
                                    &clientaddr_size);
          if (connectionfd < 0) {
            continue;
          }

          // TODO: Relay user X entered the chat.
          printf("user-%d entered the chat.\n", connectionfd);

          FD_SET(connectionfd, &masterfds);
          if (connectionfd > maxfd) {
            maxfd = connectionfd;
          }
        } else {
          char buf[MAX_MSG_LEN];
          int bytes_received = recv(i, buf, MAX_MSG_LEN, 0);
          if (bytes_received <= 0) {
            if (bytes_received == 0) {
              // TODO: Relay user X left the chat.
              printf("user-%d left the chat.\n", i);
            }

            close(i);
            FD_CLR(i, &masterfds);
            continue;
          }

          buf[bytes_received] = '\0';
          send_message(listenerfd, buf, i, maxfd, &masterfds);
        }
      }
    }
  }

  close_all_fds(maxfd, &masterfds);
  return EXIT_SUCCESS;
}
