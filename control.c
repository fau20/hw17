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

int main(int argc, char* argv[]) {
  if(argc < 2) {
    printf("Error. Please provide create or remove command line argument.\n");
    return 0;
  }
  else if(strcmp(argv[1], "create") == 0) {
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    close(fd);

    int *data;
    int shmd;
    shmd = shmget(KEY, sizeof(int), IPC_CREAT | 0640);
    data = shmat(shmd, 0, 0);
    *data = 0;
    shmdt(data);

    int semd;
    semd = semget(KEY2, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (semd == -1) {
      printf("error %d: %s\n", errno, strerror(errno));
    }
    else {
      union semun us;
      us.val = 1;
      semctl(semd, 0, SETVAL, us);
    }
  }
  else if(strcmp(argv[1], "remove") == 0) {
    int fd = open("data.txt", O_RDONLY, 0777);
    if(errno) {
      printf("Error: %s\n", strerror(errno));
      return 0;
    }
    struct stat st;
    stat("data.txt", &st);
    if(errno) {
      printf("Error: %s\n", strerror(errno));
      return 0;
    }

    char file[st.st_size + 1];
    read(fd, file, st.st_size + 1);
    file[st.st_size] = 0;
    if(errno) {
      printf("Error: %s\n", strerror(errno));
      return 0;
    }
    printf("%s\n", file);
    close(fd);

    int shmd;
    shmd = shmget(KEY, 0, 0);
    shmctl(shmd, IPC_RMID, 0);

    int semd;
    semd = semget(KEY2, 0, 0);
    semctl(semd, IPC_RMID, 0);
  }
  else {
    printf("Error. Please provide create or remove command line argument.\n");
    return 0;
  }
}
