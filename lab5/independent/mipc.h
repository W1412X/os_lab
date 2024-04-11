#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#define BUFSZ 256
#define MAXVAL 100
#define STRSIZ 8
#define MAX_MSG_SIZE 1000
#define WORKREQUEST 1
#define CUTABLE 2
#define WORKFINISHED 3
//定义一些全局变量
//存放顾客号的共享内存数组
//第一个元素为当前等待的顾客的数量
key_t shm_cum=100;
int shm_cum_id;
int *shmaddr;
int shm_flg=IPC_CREAT | 0666;
//控制共享数组操作的信号量
int sem_op_cum_shm_id;
key_t sem_op_cum_shm_key=300;
typedef union semuns {
int val;
} Sem_uns;
//两个消息队列，理发师->顾客，顾客->理发师
struct Msgbuf {
    long mtype;
    int msg;
};
key_t msg_key1=200;
int msgqid1;
key_t msg_key2=201;
int msgqid2;
//账本的信号量
int account_sem_id;
key_t account_sem_key=304;
int ind=0;

int set_sem(key_t sem_key, int sem_val, int sem_flg);
int get_ipc_id(char *proc_file,key_t key);
int *set_shm(key_t shm_key, int shm_num, int shm_flg);
int set_msq(key_t msq_key, int msq_flg);
int down(int sem_id);
int up(int sem_id);
void init();
void init(){//初始化上边的东西
    shmaddr=set_shm(shm_cum,21,shm_flg);
    sem_op_cum_shm_id=set_sem(sem_op_cum_shm_key,1,IPC_CREAT | 0666);
    account_sem_id=set_sem(account_sem_key,1,IPC_CREAT|0666);
    msgqid1=set_msq(msg_key1,IPC_CREAT|0666);
    msgqid2=set_msq(msg_key2,IPC_CREAT|0666);
}



int set_sem(key_t sem_key, int sem_val, int sem_flg)
{
    int sem_id;
    Sem_uns sem_arg;
    // 测试由 sem_key 标识的信号量数组是否已经建立
    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0)
    {
        // semget 新建一个信号量 , 其标号返回到 sem_id
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0)
        {
            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }
        // 设置信号量的初值 
        sem_arg.val = sem_val;
        if (semctl(sem_id, 0, SETVAL, sem_arg) < 0)
        {
            perror("semaphore set error");
            exit(EXIT_FAILURE);
        }
    }
    return sem_id;
}
int get_ipc_id(char *proc_file,key_t key){//获取对应的IPC的id号
    FILE*pf;
    int i,j;
    char line[BUFSZ],colum[BUFSZ];
    pf=fopen(proc_file,"r");
    if(pf==NULL){
        fgets(line,BUFSZ,pf);
    }
    while(!feof(pf)){
        i=j=0;
        fgets(line,BUFSZ,pf);
        while(line[i]==' ')i++;
        while(line[i]!=' ')colum[i++]=line[i++];
        colum[j]='\0';
        if(atoi(colum)!=key)continue;
        j=0;
        while(line[i]==' ')i++;
        while(line[i]!=' ')colum[j++]=line[i++];
        colum[j]='\0';
        i=atoi(colum);
        fclose(pf);
        return i;
    }
    fclose(pf);
    return -1;
}
int *set_shm(key_t shm_key, int shm_num, int shm_flg)
{
    int i, shm_id;
    int *shm_buf;
    // 测试由 shm_key 标识的共享内存区是否已经建立
    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0)
    {
        // shmget 新建 一个长度为 shm_num 字节的共享内存 , 其标号返回 shm_id
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0)
        {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
        // shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if ((shm_buf = (int *)shmat(shm_id, 0, 0)) < (int *)0)
        {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
        for(int i=0;i<shm_num;i++){
            shm_buf[i]=0;
        }
    }
    // shm_key 标识的共享内存区已经建立 , 将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (int *)shmat(shm_id, 0, 0)) < (int *)0)
    {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}
int down(int sem_id) {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) < 0) {
        perror("down error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
int up(int sem_id) {
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) < 0) {
        perror("up error");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
int set_msq(key_t msq_key, int msq_flg)
{
    int msq_id;
    // 测试由 msq_key 标识的消息队列是否已经建立
    if ((msq_id = get_ipc_id("/proc/sysvipc/msg", msq_key)) < 0)
    {
        // msgget 新建一个消息队列 , 其标号返回到 msq_id
        if ((msq_id = msgget(msq_key, msq_flg)) < 0)
        {
            perror("messageQueue set error");
            exit(EXIT_FAILURE);
        }
    }
    return msq_id;
}
