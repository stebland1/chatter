#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

struct termios oldtermios;

void disable_raw_mode() { tcsetattr(fileno(stdin), TCSAFLUSH, &oldtermios); }

void enable_raw_mode() {
  struct termios newtermios;
  tcgetattr(fileno(stdin), &oldtermios);
  newtermios = oldtermios;
  newtermios.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(fileno(stdin), TCSAFLUSH, &newtermios);
  atexit(disable_raw_mode);
}
