#include "client/client.h"
#include "client/raw.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
    return EXIT_FAILURE;
  }

  int serverfd;
  if ((serverfd = client_connect(argv[1], argv[2])) < 0) {
    fprintf(stderr, "Failed to connect to server\n");
    return EXIT_FAILURE;
  }

  enable_raw_mode();
  int status = client_run(serverfd);

  client_shutdown(serverfd);
  return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
