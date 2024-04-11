#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  // 共享内存区的大小

int main() {
    int shmid;  // 共享内存的标识符
    key_t key = 100;  // 共享内存的键值
    int*shm;  // 指向共享内存的指针

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
	while(1){
		sleep(1);
        printf("Wait Sum:%d  \n",shm[0]);
        printf("Cutting customer: ");
        for(int i=1;i<4;i++){
            if(shm[i]!=0){
                printf("%d  ",shm[i]);
            }
        }
        printf("\n");
        printf("Waiter on Sit: ");
        for(int i=4;i<8;i++){
            if(shm[i]!=0){
                printf("%d  ",shm[i]);
            }
        }
        printf("\n");
        printf("Waiter in Room:  ");
        for(int i=8;i<=20;i++){
            if(shm[i]!=0){
                printf("%d  ",shm[i]);
            }
        }
        printf("\n\n");
        fflush(stdout);
    }

    // 分离共享内存
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
