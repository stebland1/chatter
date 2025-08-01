#include "client/input.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int handle_receive_message(int serverfd, InputBuffer *ib) {
  char msgbuf[MAX_MSG_LEN];
  int bytes_received = recv(serverfd, msgbuf, MAX_MSG_LEN, 0);
  if (bytes_received <= 0) {
    if (bytes_received < 0) {
      fprintf(stderr, "Error receiving message\n");
    }

    printf("Server disconnected\n");
    return -1;
  }

  msgbuf[bytes_received] = '\0';
  terminal_clear_line();
  write(STDOUT_FILENO, msgbuf, bytes_received);
  if (ib->len > 0) {
    write(STDOUT_FILENO, ib->buf, ib->len);
  }
  return 0;
}
