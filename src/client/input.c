#include "client/input.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>

void terminal_clear_line() { write(STDOUT_FILENO, "\r\x1b[K", 4); }

void terminal_write_ib(InputBuffer *ib) {
  terminal_clear_line();
  write(STDIN_FILENO, ib->buf, ib->len);
}

int ib_append(InputBuffer *ib, char c) {
  if (ib->len >= MAX_MSG_LEN - 1 /* newline */ - 1 /* null */) {
    return -1;
  }

  ib->buf[ib->len++] = c;
  return 0;
}

int handle_user_input(int serverfd, InputBuffer *ib) {
  char c;
  if (read(STDIN_FILENO, &c, 1) == -1) {
    perror("read");
    return -1;
  }

  switch (c) {
  case 127:
    if (ib->len > 0) {
      ib->len--;
      terminal_write_ib(ib);
    }
    break;
  case '\n':
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
    if (ib_append(ib, c) == 0) {
      terminal_write_ib(ib);
    }
    break;
  }

  ib->buf[ib->len] = '\0';
  return 0;
}
