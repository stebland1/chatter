#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MSG_LEN 512
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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

    select(maxfd + 1, &readfds, NULL, NULL, NULL);

    for (int i = 0; i <= maxfd; i++) {
      if (FD_ISSET(i, &readfds)) {
        if (i == socketfd) {
          int connectionfd = accept(socketfd, (struct sockaddr *)&clientaddr,
                                    &clientaddr_size);
          if (connectionfd < 0) {
            continue;
          }

          printf("user-%d entered the chat.\n", connectionfd);

          FD_SET(connectionfd, &masterfds);
          if (connectionfd > maxfd) {
            maxfd = connectionfd;
          }
        } else {
          char buf[MAX_MSG_LEN];
          int bytes_received = recv(i, buf, MAX_MSG_LEN, 0);
          if (bytes_received < 0) {
            /*
             * we should do something fancy here, like saying failed to
             * receive msg back to the sender.
             */
            continue;
          }
          if (bytes_received == 0) {
            printf("Client disconnected.\n");
            FD_CLR(i, &masterfds);
            close(i);
            break;
          }

          buf[MIN((int)strcspn(buf, "\n"), bytes_received)] = '\0';
          printf("user-%d: %s\n", i, buf);
        }
      }
    }
  }

  freeaddrinfo(servinfo);
  close(socketfd);
  return EXIT_SUCCESS;
}
