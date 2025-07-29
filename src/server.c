#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MSG_LEN 512
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void close_all_fds(int maxfds, fd_set *fds) {
  for (int i = 0; i <= maxfds; i++) {
    if (FD_ISSET(i, fds)) {
      close(i);
    }
  }
}

int main(void) {
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, "8081", &hints, &servinfo) != 0) {
    return EXIT_FAILURE;
  }

  int socketfd =
      socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (socketfd == -1) {
    freeaddrinfo(servinfo);
    perror("socket");
    return EXIT_FAILURE;
  }

  if (bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
    freeaddrinfo(servinfo);
    perror("bind");
    return EXIT_FAILURE;
  }

  if (listen(socketfd, 10) < 0) {
    freeaddrinfo(servinfo);
    perror("listen");
    return EXIT_FAILURE;
  }

  fd_set masterfds, readfds;
  int maxfd = socketfd;
  FD_ZERO(&masterfds);
  FD_ZERO(&readfds);
  FD_SET(socketfd, &masterfds);
  while (1) {
    readfds = masterfds;
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        close_all_fds(maxfd, &masterfds);
        freeaddrinfo(servinfo);
        return EXIT_FAILURE;
      }
    }

    for (int i = 0; i <= maxfd; i++) {
      if (FD_ISSET(i, &readfds)) {
        if (i == socketfd) {
          int connectionfd = accept(socketfd, (struct sockaddr *)&clientaddr,
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

            FD_CLR(i, &masterfds);
            close(i);
            continue;
          }

          buf[MIN((int)strcspn(buf, "\n"), bytes_received)] = '\0';
          printf("user-%d: %s\n", i, buf);
        }
      }
    }
  }

  freeaddrinfo(servinfo);
  close_all_fds(maxfd, &masterfds);
  return EXIT_SUCCESS;
}
