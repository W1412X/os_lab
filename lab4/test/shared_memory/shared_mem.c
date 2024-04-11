#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define SHM_SIZE 10 // 共享内存大小
#define MAX_RAND 100 // 随机数的最大值
// 定义共享内存结构体
typedef struct {
    int data[SHM_SIZE];
    int index;
} shared_data;
// 写入随机数字的进程函数
void writer_process(int shm_id) {
    // 获取共享内存指针
    shared_data *shm_ptr = (shared_data *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    // 写入随机数字
    srand(time(NULL));
    for (int i = 0; i < SHM_SIZE; i++) {
        shm_ptr->data[i] = rand() % MAX_RAND;
        printf("Writing to shared memory: %d\n", shm_ptr->data[i]);
        sleep(1);
    }
    // 设置结束标志
    shm_ptr->index = SHM_SIZE;
    // 分离共享内存
    shmdt(shm_ptr);
}
// 读取随机数字的进程函数
void reader_process(int shm_id) {
    // 等待5秒
    sleep(5);
    // 获取共享内存指针
    shared_data *shm_ptr = (shared_data *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    for (int i = 0; i < SHM_SIZE; i++) {
        printf("Reading from shared memory: %d\n", shm_ptr->data[i]);
        sleep(1);
    }
    // 分离
    shmdt(shm_ptr);
}
int main() {
    int shm_id;
    pid_t pid;
    // 创建共享内存
    shm_id = shmget(IPC_PRIVATE, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    // 创建写入进程
    pid = fork();
    if (pid == 0) {
        writer_process(shm_id);
        exit(0);
    } else if (pid == -1) {
        perror("fork");
        exit(1);
    }
    // 创建读取进程
    pid = fork();
    if (pid == 0) {
        reader_process(shm_id);
        exit(0);
    } else if (pid == -1) {
        perror("fork");
        exit(1);
    }
    wait(NULL);
    wait(NULL);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
