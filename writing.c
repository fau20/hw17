#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define KEY 10000
#define KEY2 10001

int main() {
  int semd;
  semd = semget(KEY2, 0, 0);
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;

  semop(semd, &sb, 1);

  int shmd;
  int *data;
  shmd = shmget(KEY, 0, 0);
  data = shmat(shmd, 0, 0);

  int fd = open("data.txt", O_RDONLY, 0777);
  lseek(fd, -(*data), SEEK_END);

  char line[*data + 1];
  read(fd, line, *data);
  line[*data] = 0;
  printf("previous line: %s\n", line);
  close(fd);

  open("data.txt", O_WRONLY | O_APPEND, 0777);
  printf("Please enter next line: ");

  char buffer[1000];
  fgets(buffer, 1000, stdin);

  *data = strlen(buffer);
  write(fd, buffer, *data);
  close(fd);
  shmdt(data);
  sb.sem_op = 1;
  semop(semd, &sb, 1);

  return 0;
}
