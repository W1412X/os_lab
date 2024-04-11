#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  // 共享内存区的大小

int main() {
    int shmid;  // 共享内存的标识符
    key_t key = 101;  // 共享内存的键值
    char *shm;  // 指向共享内存的指针

    // 创建共享内存
    if ((shmid = shmget(key, 0, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 将共享内存连接到当前进程的地址空间
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    printf("%d",shm);
    // 读取共享内存中的数据并打印
    printf("Data in shared memory: %s\n", shm);

    // 分离共享内存
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
