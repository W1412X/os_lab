# 山东大学操作系统实验 实验4 进程的同步
## 实验思路
- 创建一个共享内存shm_id用于存储三种材料的数量，为一个int型的数组
- 创建一个共享内存shm_ind用于记录当前供应材料的序号，实现轮流供应
- 创建两个信号量sem_id,sem_wake_id
  - sem_id -> 用于确保资源同一时间仅被一个进程访问
  - sem_wake -> 用于在消费者进程中发现对应的材料不足唤醒供应者进程
## 实验代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFSZ 256
# define RETE 1
int sem_wake_id;
typedef union semuns {
int val;
} Sem_uns;
int down(int sem_id){//P操作，尝试获取对资源的访问权限
    struct sembuf buf;
    buf.sem_op=-1;
    buf.sem_num=0;
    buf.sem_flg=SEM_UNDO;//标记为
    if(semop(sem_id,&buf,1)<0){
        perror("down error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
int up(int sem_id){//V操作 尝试获取释放资源权限
    struct sembuf buf;
    buf.sem_op=1;
    buf.sem_num=0;
    buf.sem_flg=SEM_UNDO;
    if(semop(sem_id,&buf,1)<0){
        perror("up error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
void supply(int shmid,int shm_ind,int sem_id){//传入这个共享内存段的标识
    //连接共享内存段
    int *shared_array=(int*)shmat(shmid,NULL,0);
    int *index_now=(int*)shmat(shm_ind,NULL,0);
    if(shared_array==(int*)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    down(sem_wake_id);
    down(sem_id);
    printf("%d supply %d\n",getpid(),*index_now%3);
    shared_array[*index_now%3]+=1;
    *index_now=(*index_now)+1;
    printf("%d\n",*index_now);
    for(int i=0;i<3;i++){
        printf("%d ",shared_array[i]);
    }
    printf("\n");
    up(sem_id);
    sleep(1);
}
void smoke(int shmid,int ind,int sem_id){//传入... ...
    //连接共享内存段
    int *shared_array=(int*)shmat(shmid,NULL,0);
    if(shared_array==(int*)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    down(sem_id);
    printf("%d smoke has matrial %d nums %d\n",getpid(),ind,shared_array[ind]);
    int judge=1;
    for(int i=0;i<3;i++){
        if(shared_array[i]>0){
            continue;
        }else{
            judge=0;
        }
    }
    if(judge==0){//材料不足
        printf("no enough metrial %d %d %d\n",shared_array[0],shared_array[1],shared_array[2]);
        up(sem_wake_id);
    }else{//材料足够
        for(int i=0;i<3;i++){
            shared_array[i]=shared_array[i]-1;
        }
        printf("%d smoke\n",getpid());
    }
    up(sem_id);
    sleep(1);
}
int main(){
    //创建共享内存段
    int shmid;
    int *shared_array,*index_now;
    int shm_ind;
    shmid=shmget(IPC_PRIVATE,3*sizeof(int),IPC_CREAT|0666);
    shm_ind=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0666);
    if(shmid==-1||shm_ind==-1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    // 创建信号量
    int sem_id;
    Sem_uns sem_arg1,sem_arg2;
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    sem_wake_id= semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id == -1||sem_wake_id==-1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    sem_arg1.val = 1;
    sem_arg2.val=1;
    if (semctl(sem_id, 0, SETVAL, sem_arg1) == -1 || semctl(sem_wake_id, 0, SETVAL, sem_arg2) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    down(sem_wake_id);
    pid_t smoke_array[3];
    for(int i=0;i<3;i++){
        smoke_array[i]=fork();
        if(smoke_array[i]==0){
            while(1){
                smoke(shmid,i,sem_id);
            }
        }else if(smoke_array[i]>0){

        }else{
            printf("create failed");
            exit(EXIT_FAILURE);
        }
    }
    pid_t supply_array[2];
    for(int i=0;i<2;i++){
        supply_array[i]=fork();
        if(supply_array[i]==0){
            while(1){
                supply(shmid,shm_ind,sem_id);
            }
        }else if(supply_array[i]>0){

        }else{
            printf("create failed");
            exit(EXIT_FAILURE);
        }
    }
    for(int i=0;i<5;i++){
        wait(NULL);
    }
}
```
## 实验代码解释

### `down` 函数

```c
int down(int sem_id)
```

这个函数执行 P 操作，尝试获取对共享资源的访问权限。它通过调用 `semop` 函数来操作信号量，将信号量的值减一，表示占用资源。如果无法获取资源，则程序会输出错误信息并退出。

### `up` 函数

```c
int up(int sem_id)
```

这个函数执行 V 操作，尝试释放对共享资源的访问权限。它通过调用 `semop` 函数来操作信号量，将信号量的值加一，表示释放资源。

### `supply` 函数

```c
void supply(int shmid, int shm_ind, int sem_id)
```

这个函数表示生产者的行为。它连接到共享内存段，并使用 `down` 函数获取对共享资源的访问权限。然后，它向共享内存中的指定位置增加一个资源，并更新索引。最后，使用 `up` 函数释放对共享资源的访问权限，并进行短暂的睡眠。

### `smoke` 函数

```c
void smoke(int shmid, int ind, int sem_id)
```

这个函数表示消费者的行为。它连接到共享内存段，并使用 `down` 函数获取对共享资源的访问权限。然后，它检查共享内存中的资源是否足够进行消费。如果足够，它会消耗资源并更新共享内存。如果不足，则通过 `up` 函数释放对共享资源的访问权限，并唤醒生产者。最后，进行短暂的睡眠。

### `main` 函数

```c
int main()
```

这个函数是程序的入口点。它首先创建了共享内存段和信号量，并初始化它们。然后创建了多个生产者和消费者的子进程。主进程使用 `wait` 函数等待所有子进程执行完毕，并释放相关资源。

这些函数一起实现了生产者-消费者问题的解决方案，确保了生产者和消费者之间的正确协作。