#ifndef POLL_H
#define POLL_H

#include <stddef.h>
#include <sys/poll.h>

typedef struct {
  struct pollfd *pollfds;
  size_t count;
  size_t capacity;
} Poll;

Poll *poll_create();
void poll_destroy(Poll *poll);
int poll_insert_fd(Poll *poll, int fd);
void poll_delete_fd(Poll *poll, int index);

#endif
