#ifndef SERVER_MESSAGING_H
#define SERVER_MESSAGING_H

int handle_receive_message(int serverfd, char *msgbuf);
int handle_send_message(int serverfd, char *msgbuf);

#endif
