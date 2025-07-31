#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

/*
 * A wrapper around send. To ensure all bytes are sent.
 */
int sendall(int fd, const void *buf, size_t len) {
  size_t total_sent = 0;
  const char *p = buf;

  while (total_sent < len) {
    ssize_t n = send(fd, p + total_sent, len - total_sent, 0);
    if (n == -1) {
      perror("send");
      return -1;
    }
    total_sent += n;
  }

  return 0;
}
