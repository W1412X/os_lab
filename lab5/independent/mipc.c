#include"mipc.h"
void init(){//初始化上边的东西
    shmaddr=set_shm(shm_cum,18,shm_flg);
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
