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
    printf("Supply %d st , materail: ",(1+*index_now)/2);
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
    printf("%d smoke has material %d nums %d\n",getpid(),ind,shared_array[ind]);
    int judge=1;
    for(int i=0;i<3;i++){
        if(shared_array[i]>0){
            continue;
        }else{
            judge=0;
        }
    }
    if(judge==0){//材料不足
        printf("no enough material %d %d %d\n",shared_array[0],shared_array[1],shared_array[2]);
        up(sem_wake_id);
        up(sem_id);
        down(sem_id);
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
    shared_array=(int*)shmat(shmid,NULL,0);
    shared_array[0]=1;shared_array[1]=1;shared_array[2]=1;
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