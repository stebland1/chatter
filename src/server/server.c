#include "server/server.h"
#include "server/connection.h"
#include "server/hashtable.h"
#include "server/messaging.h"
#include "server/socket.h"
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 64
#define CLIENT_IDS_INIT_CAPACITY 1

int server_init(ServerContext *ctx, const char *hostname, const char *port) {
  ctx->listenerfd = get_listener_socket(hostname, port);
  if (ctx->listenerfd < 0) {
    return -1;
  }

  HashTable *cc = ht_create(HT_KEY_INT);
  if (!cc) {
    fprintf(stderr, "Failed to create hash table\n");
    return -1;
  }
  ctx->connected_clients = cc;

  ctx->connected_client_ids.ids = malloc(sizeof(ctx->connected_client_ids.ids));
  if (!ctx->connected_client_ids.ids) {
    fprintf(stderr, "Failed to allocate for connected client ids\n");
    ht_destroy(ctx->connected_clients);
    return -1;
  }
  ctx->connected_client_ids.count = 0;
  ctx->connected_client_ids.capacity = CLIENT_IDS_INIT_CAPACITY;

  struct kevent kev;
  EV_SET(&kev, ctx->listenerfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  int kq = kqueue();
  struct timespec ts;
  ts.tv_sec = ts.tv_nsec = 0;

  if (kevent(kq, &kev, 1, NULL, 0, &ts) < 0) {
    perror("kevent");
    ht_destroy(ctx->connected_clients);
    return -1;
  }

  ctx->kq = kq;
  return 0;
}

void server_run(ServerContext *ctx) {
  struct kevent eventlist[MAX_EVENTS];

  while (1) {
    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size = sizeof clientaddr;

    size_t nev;
    if ((nev = kevent(ctx->kq, NULL, 0, eventlist, MAX_EVENTS, 0)) < 0) {
      if (errno == EINTR) {
        continue;
      }

      perror("kevent");
      break;
    }

    for (size_t i = 0; i < nev; i++) {
      if (eventlist[i].ident == (uintptr_t)ctx->listenerfd) {
        if (handle_new_connection(ctx, (struct sockaddr *)&clientaddr,
                                  &clientaddr_size) == -1) {
          fprintf(stderr, "Failed to connect\n");
        }
      } else if (handle_client_message(eventlist[i].ident, ctx) < 0) {
        fprintf(stderr, "Failed to handle message from fd %zu\n", i);
      }
    }
  }
}

void server_shutdown(ServerContext *ctx) {
  close_all_fds(ctx);
  free(ctx->connected_client_ids.ids);
  ht_destroy(ctx->connected_clients);
}
