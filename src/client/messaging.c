#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int handle_send_message(int serverfd, char *msgbuf) {
  if (fgets(msgbuf, MAX_MSG_LEN, stdin) == NULL) {
    fprintf(stderr, "EOF on stdin\n");
    return -1;
  }

  if (send(serverfd, msgbuf, strlen(msgbuf), 0) == -1) {
    perror("send");
    return -1;
  }

  return 0;
}

int handle_receive_message(int serverfd, char *msgbuf) {
  int bytes_received = recv(serverfd, msgbuf, MAX_MSG_LEN, 0);
  if (bytes_received <= 0) {
    if (bytes_received < 0) {
      fprintf(stderr, "Error receiving message\n");
    }

    printf("Server disconnected\n");
    return -1;
  }

  msgbuf[bytes_received] = '\0';
  printf("%s", msgbuf);

  return 0;
}
