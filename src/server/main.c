#include "server/hashtable.h"
#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>

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

  HashTable *cc = ht_create(HT_KEY_INT);
  ServerContext ctx = {
      .masterfds = {0},
      .readfds = {0},
      .connected_clients = cc,
  };

  if (server_init(&ctx, hostname, port) < 0) {
    fprintf(stderr, "Failed to initialize server\n");
    return EXIT_FAILURE;
  }

  server_run(&ctx);

  server_shutdown(&ctx);
  return EXIT_SUCCESS;
}
