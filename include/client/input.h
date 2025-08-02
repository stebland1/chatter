#ifndef INPUT_H
#define INPUT_H

#include "utils.h"
#include <stddef.h>

typedef struct {
  char buf[MAX_MSG_LEN];
  int len;
} InputBuffer;

int handle_user_input(int serverfd, InputBuffer *ib);
void terminal_clear_line();
void terminal_write_ib(InputBuffer *ib);
int ib_append(InputBuffer *ib, char c);

#endif
