#ifndef INPUT_H
#define INPUT_H

#include "utils.h"
#include <stddef.h>

int handle_user_input(int serverfd, char *inputbuf, int *inputbuf_len);
void terminal_clear_line();
void rewrite_input_buffer(char *inputbuf, int *inputbuf_len);

#endif
