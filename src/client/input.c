#include "client/input.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>

void terminal_clear_line() { write(STDOUT_FILENO, "\r\x1b[K", 4); }

void rewrite_input_buffer(InputBuffer *ib) {
  terminal_clear_line();
  write(STDIN_FILENO, ib->buf, ib->len);
}

int handle_user_input(int serverfd, InputBuffer *ib) {
  char c;
  if (read(STDIN_FILENO, &c, 1) == -1) {
    perror("read");
    return -1;
  }

  switch (c) {
  case 127:
    ib->len--;
    rewrite_input_buffer(ib);
    break;
  case '\n':
    // TODO: appending to the input buffer should be protected by MAX_MSG_LEN
    ib->buf[ib->len++] = c;
    if (sendall(serverfd, ib->buf, ib->len) == -1) {
      return -1;
    }
    terminal_clear_line();
    write(STDIN_FILENO, "you: ", 5);
    write(STDIN_FILENO, ib->buf, ib->len);
    ib->len = 0;
    break;
  default:
    ib->buf[ib->len++] = c;
    rewrite_input_buffer(ib);
    break;
  }

  ib->buf[ib->len] = '\0';
  return 0;
}
