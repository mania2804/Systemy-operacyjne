#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

// ===== Message Queue =====
struct msg_buffer {
    long msg_type;
    int number;
};

int main() {
    // ----------------- PIPE -----------------
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if(pid == 0) { // child for pipe
        close(fd[1]);
        char buf[100];
        read(fd[0], buf, sizeof(buf));
        printf("Pipe child received: %s\n", buf);
        close(fd[0]);
        exit(0);
    } else { // parent for pipe
        close(fd[0]);
        char pipe_msg[100];
        printf("Enter message for pipe: ");
        fgets(pipe_msg, sizeof(pipe_msg), stdin);
        write(fd[1], pipe_msg, strlen(pipe_msg)+1);
        close(fd[1]);
        wait(NULL);
    }

    // ----------------- MESSAGE QUEUE -----------------
    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct msg_buffer msg;
    msg.msg_type = 1;
    srand(time(NULL));
    msg.number = rand() % 100;

    msgsnd(msgid, &msg, sizeof(msg.number), 0);
    printf("Message Queue sent number: %d\n", msg.number);

    struct msg_buffer rcv;
    msgrcv(msgid, &rcv, sizeof(rcv.number), 1, 0);
    printf("Message Queue received number: %d\n", rcv.number);

    msgctl(msgid, IPC_RMID, NULL);

    // ----------------- SHARED MEMORY -----------------
    key_t shm_key = ftok("shmfile", 65);
    int shmid = shmget(shm_key, 1024, 0666 | IPC_CREAT);
    char *shm_ptr = (char*) shmat(shmid, NULL, 0);

    pid = fork();
    if(pid == 0) { // child for shared memory
        sleep(1); // wait for parent to write
        printf("Shared memory child reads: %s\n", shm_ptr);
        shmdt(shm_ptr);
        exit(0);
    } else { // parent
        printf("Enter message for shared memory: ");
        fgets(shm_ptr, 1024, stdin);
        wait(NULL);
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
