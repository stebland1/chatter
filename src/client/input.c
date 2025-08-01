#include "utils.h"
#include <stdio.h>
#include <unistd.h>

void terminal_clear_line() { write(STDOUT_FILENO, "\r\x1b[K", 4); }

void rewrite_input_buffer(char *inputbuf, int *inputbuf_len) {
  terminal_clear_line();
  write(STDIN_FILENO, inputbuf, *inputbuf_len);
}

int handle_user_input(int serverfd, char *inputbuf, int *inputbuf_len) {
  char c;
  if (read(STDIN_FILENO, &c, 1) == -1) {
    perror("read");
    return -1;
  }

  switch (c) {
  case 127:
    (*inputbuf_len)--;
    rewrite_input_buffer(inputbuf, inputbuf_len);
    break;
  case '\n':
    // we've finished the line and we're sending it!
    inputbuf[(*inputbuf_len)++] = c;
    if (sendall(serverfd, inputbuf, *inputbuf_len) == -1) {
      return -1;
    }
    terminal_clear_line();
    write(STDIN_FILENO, "you: ", 5);
    write(STDIN_FILENO, inputbuf, *inputbuf_len);
    *inputbuf_len = 0;
    break;
  default:
    inputbuf[(*inputbuf_len)++] = c;
    rewrite_input_buffer(inputbuf, inputbuf_len);
    break;
  }

  inputbuf[*inputbuf_len] = '\0';
  return 0;
}
