#include "poll.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

Poll *poll_create() {
  Poll *poll = malloc(sizeof(*poll));
  if (!poll) {
    return NULL;
  }

  poll->pollfds = malloc(sizeof(struct pollfd) * 10);
  if (!poll->pollfds) {
    free(poll);
    return NULL;
  }

  poll->count = 0;
  poll->capacity = 10;
  return poll;
}

void poll_destroy(Poll *poll) {
  free(poll->pollfds);
  free(poll);
}

int poll_insert_fd(Poll *poll, int fd) {
  if (poll->count >= poll->capacity) {
    size_t new_capacity = poll->capacity * 2;
    struct pollfd *new_pollfds = realloc(poll->pollfds, new_capacity);
    if (!new_pollfds) {
      fprintf(stderr, "Failed to insert fd %d into poll ds\n", fd);
      perror("realloc");
      return -1;
    }
    poll->pollfds = new_pollfds;
    poll->capacity = new_capacity;
  }

  poll->pollfds[poll->count].fd = fd;
  poll->pollfds[poll->count].events = POLLIN;
  poll->pollfds[poll->count++].revents = 0;
  return 0;
}

void poll_delete_fd(Poll *poll, int index) {
  poll->pollfds[index] = poll->pollfds[poll->count - 1];
  poll->count--;
}
