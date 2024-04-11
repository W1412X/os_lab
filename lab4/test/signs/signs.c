#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

int main() {
    int sem_id;
    struct sembuf sem_op;

    // 创建或获取信号量集的标识符
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // 初始化信号量的值为1
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    // 执行P操作，等待信号量
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    // 执行V操作，释放信号量
    sem_op.sem_op = 1;
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    // 删除信号量集
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
