#include "server.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void close_all_fds(ServerContext *ctx) {
  for (int i = 0; i <= ctx->maxfd; i++) {
    if (FD_ISSET(i, &ctx->masterfds)) {
      close(i);
    }
  }
}

int get_num_digits(int num) {
  int count = 0;

  do {
    num /= 10;
    count++;
  } while (num != 0);

  return count;
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

int handle_client_message(int senderfd, ServerContext *ctx) {
  char buf[MAX_MSG_LEN];
  int bytes_received = recv(senderfd, buf, MAX_MSG_LEN, 0);
  if (bytes_received <= 0) {
    if (bytes_received == 0) {
      // TODO: Relay user X left the chat.
      printf("user-%d left the chat.\n", senderfd);
    }

    close(senderfd);
    // TODO: Every time we remove a fd, check to see if it's the
    // largest.
    FD_CLR(senderfd, &ctx->masterfds);
    return 0;
  }

  buf[bytes_received] = '\0';
  char *msg = malloc(strlen(buf) + 1 /* null */ + 7 + get_num_digits(senderfd));
  if (!msg) {
    fprintf(stderr, "Failed to allocate memory for message\n");
    return -1;
  }
  sprintf(msg, "user-%d: %s", senderfd, buf);
  send_message(senderfd, msg, ctx);
  free(msg);
  return 0;
}

void handle_new_connection(ServerContext *ctx, struct sockaddr *clientaddr,
                           socklen_t *clientaddr_size) {
  int connectionfd = accept(ctx->listenerfd, clientaddr, clientaddr_size);
  if (connectionfd < 0) {
    return;
  }

  // TODO: Relay user X entered the chat.
  printf("user-%d entered the chat.\n", connectionfd);

  FD_SET(connectionfd, &ctx->masterfds);
  if (connectionfd > ctx->maxfd) {
    ctx->maxfd = connectionfd;
  }
}

void send_message(int senderfd, char *msg, ServerContext *ctx) {
  // TODO: Apparently send buffers messages
  // So we could end up with bytes still to send.
  for (int i = 0; i <= ctx->maxfd; i++) {
    if (FD_ISSET(i, &ctx->masterfds) && i != senderfd && i != ctx->listenerfd) {
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

  ServerContext ctx = {
      .masterfds = {0},
      .readfds = {0},
      .listenerfd = listenerfd,
      .maxfd = listenerfd,
  };

  FD_ZERO(&ctx.masterfds);
  FD_ZERO(&ctx.readfds);
  FD_SET(listenerfd, &ctx.masterfds);

  while (1) {
    ctx.readfds = ctx.masterfds;
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    if (select(ctx.maxfd + 1, &ctx.readfds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        close_all_fds(&ctx);
        fprintf(stderr, "Error selecting ready file desriptors\n");
        return EXIT_FAILURE;
      }
    }

    for (int i = 0; i <= ctx.maxfd; i++) {
      if (FD_ISSET(i, &ctx.readfds)) {
        if (i == listenerfd) {
          handle_new_connection(&ctx, (struct sockaddr *)&clientaddr,
                                &clientaddr_size);
        } else if (handle_client_message(i, &ctx) < 0) {
          return EXIT_FAILURE;
        }
      }
    }
  }

  close_all_fds(&ctx);
  return EXIT_SUCCESS;
}
