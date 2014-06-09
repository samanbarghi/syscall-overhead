#include <string.h>
#include <sys/msg.h>

int main() {
  int msqid = 0;
  struct message {
    long type;
    char text[20];
  } msg;

  msg.type = 1;
  strcpy(msg.text, "This is message 1");
  msgsnd(msqid, (void *) &msg, sizeof(msg.text), IPC_NOWAIT);
  strcpy(msg.text, "This is message 2");
  msgsnd(msqid, (void *) &msg, sizeof(msg.text), IPC_NOWAIT);

  return 0;
}
