## 独立实验
### 项目包含四个文件：mipc.h、worker.c、enter.c和check_mem.c
> 通过CMake编译
- CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.0)
project(Solve)
add_executable(worker worker.c)
add_executable(enter enter.c)
add_executable(check_mem check_mem.c)
```
- 编译运行步骤
   - 在源文件目录下操作
     - touch CMakeLists.txt
     - 复制cmake代码到CMakeLists.txt中
     - mkdir build
     - cmake ..
     - cd build
     - make
  - 得到的可执行文件在build目录下
### mipc.h（头文件）

- 变量

    - key_t shm_cum: 共享内存键值，用于存放顾客编号的共享内存。
    - int shm_cum_id: 共享内存标识符。
    - int *shmaddr: 指向共享内存的指针。
    - int shm_flg: 创建共享内存时的标志位，值为IPC_CREAT | 0666，表示创建新共享内存并赋予所有用户读写权限。
    - key_t sem_op_cum_shm_key: 控制共享数组操作的信号量键值。
    - int sem_op_cum_shm_id: 控制共享数组操作的信号量标识符。
    - union semuns: 用于设置信号量值的联合体，包含一个整型成员val。
    - key_t msg_key1: 第一个消息队列的键值，用于理发师->顾客的消息传递。
    - int msgqid1: 第一个消息队列的标识符。
    - key_t msg_key2: 第二个消息队列的键值，用于顾客->理发师的消息传递。
    - int msgqid2: 第二个消息队列的标识符。
    - key_t account_sem_key: 账本信号量键值。
    -  int account_sem_id: 账本信号量标识符。
    -  int ind: 用于生成顾客编号的全局计数器。

- 函数

    -  init(): 初始化所需的共享内存、信号量、消息队列等资源。主要工作包括：
        -  使用set_shm创建并初始化顾客编号的共享内存，第一个元素表示当前等待的顾客数量。
        -  使用set_sem创建控制共享数组操作的信号量。
        -  使用set_sem创建账本信号量。
        -  使用set_msq创建两个消息队列。

    -  set_sem(key_t sem_key, int sem_val, int sem_flg): 创建或获取指定键值的信号量，并设置其初始值。

    -  get_ipc_id(char *proc_file, key_t key): 通过读取指定系统文件，获取与给定键值对应的IPC资源（如信号量、共享内存、消息队列）的ID。

    -  int *set_shm(key_t shm_key, int shm_num, int shm_flg): 创建或获取指定键值的共享内存，并将其附加到进程地址空间。若为新创建，初始化所有元素为0。

    -  int set_msq(key_t msq_key, int msq_flg): 创建或获取指定键值的消息队列。

    -  down(int sem_id): 对指定信号量执行递减操作（P操作）。

    -  up(int sem_id): 对指定信号量执行递增操作（V操作）。
- 代码
```c
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

```

### worker.c（理发师进程）

- 函数

    - work(): 理发师进程的主要工作循环。包括：
        -  等待接收到顾客的理发请求（类型为WORKREQUEST）。
        -  获取顾客编号，并打印开始为顾客理发的信息。
        -  向顾客发送可开始理发的通知（类型为CUTABLE）。
        -  模拟理发过程（睡眠3秒）。
        -  打印完成理发的信息。
        -  通过调用down和up模拟收取顾客费用的过程（涉及账本信号量）。
        -  发送理发及付款完成的通知（类型为WORKFINISHED）。

    - main(): 主函数，初始化资源后进入无限循环，不断执行work()。
- 代码
```c
#include"mipc.h"
void work(){//理发师进程中要做的
    struct Msgbuf message;
    int customer_id=-1;
    printf("Worker %d Sleeping\n",getpid());
    //等待
    msgrcv(msgqid2,&message,sizeof(message),WORKREQUEST,0);
    //等待到了一个顾客
    customer_id=message.msg;
    printf("Worker %d Start CUT Customer %d\n",getpid(),customer_id);
    //反馈给顾客告诉他可以理发了
    struct Msgbuf message1;
    message1.mtype=CUTABLE;
    message1.msg=customer_id;
    msgsnd(msgqid1,&message1,sizeof(message1),0);
    //理发
    sleep(3);
    printf("Worker %d Finish CUT Customer %d\n",getpid(),customer_id);
    //收钱，需要设置账本信号量
    printf("Worker %d START Get Money From Custmer %d\n",getpid(),customer_id);
    down(account_sem_id);
    sleep(1);
    printf("Worker %d Finish Get Money From Custmer %d\n",getpid(),customer_id);
    up(account_sem_id);
    struct Msgbuf message2;
    message2.mtype=WORKFINISHED;
    message2.msg=customer_id;
    //发送理发以及付款结束，此时可以更新顾客容量的信号量
    msgsnd(msgqid1,&message2,sizeof(message2),0);
}
int main(){
    init();
    while(1){
        work();
    }
    return 0;
}
```

### enter.c（顾客进程）

- 函数

    - customers(): 顾客进程的主要工作流程。包括：
        -  检查共享内存中等待顾客的数量，若无等待顾客则返回。
        -  向理发师发送理发请求（类型为WORKREQUEST）。
        -  等待理发师发送可开始理发的通知（类型为CUTABLE）。
        -  等待理发师发送理发及付款完成的通知（类型为WORKFINISHED）。
        -  更新共享内存中顾客状态，释放已理发顾客的编号。

    -  add_customer(): 循环接收输入，每当接收到一个字符，就创建一个顾客进程（通过fork）。若顾客等待区已满，打印提示信息。否则，更新共享内存中的顾客数量和编号，并创建一个子进程执行customers()。

    -  main(): 主函数，初始化资源后调用add_customer()。
- 代码
```c
#include"mipc.h"
void customers(){
    //首先检查顾客队列是否有顾客
    if(shmaddr[0]<=0){
        return;
    }
    //有顾客
    struct Msgbuf message;
    message.msg=shmaddr[shmaddr[0]];
    message.mtype=WORKREQUEST;
    //发送理发请求，等待回应阻塞
    msgsnd(msgqid2,&message,sizeof(message),0);
    int r=msgrcv(msgqid1,&message,sizeof(message),CUTABLE,0);
    if(r>0){//已经开始理发了,在这等待理发完成的信号
        if(msgrcv(msgqid1,&message,sizeof(message),WORKFINISHED,0)>0){//接到理完头发的信息
            down(sem_op_cum_shm_id);
			printf("Get FINISHMSG %d\n",message.msg);
            for(int i=1;i<shmaddr[0];i++){
                shmaddr[i]=shmaddr[i+1];
            }
            shmaddr[shmaddr[0]]=0;
            shmaddr[0]--;
            up(sem_op_cum_shm_id);
        }
    }
}
void add_customer(){//
    while(getchar()){//
		ind++;
		if(shmaddr[0]>20){
            printf("NO MORE POSITION\n");
        }else{
            down(sem_op_cum_shm_id);
            shmaddr[0]++;
            shmaddr[shmaddr[0]]=ind;
            up(sem_op_cum_shm_id);
            if(fork()==0){
                customers();
                exit(EXIT_SUCCESS);
            }else{

            }
        }
    }
}
int main(){
    init();
    add_customer();
    return 0;
}
```
### check_mem.c（监控进程）

- 函数

    -  main(): 监控进程主函数，主要工作包括：
        -  创建共享内存的标识符shmid，通过shmget获取已存在的共享内存。
        -  将共享内存连接到当前进程的地址空间，通过shmat获得指向共享内存的指针shm。
        -  进入无限循环，每秒打印一次共享内存中顾客的状态信息：
            - 等待理发的顾客总数
            - 正在接受理发的顾客编号
            - 等待座位的顾客编号
            - 等待房间的顾客编号
- 代码
```c
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

```
> 通过以上文件的配合，该项目实现了理发店场景的模拟，其中理发师进程负责接收顾客请求并模拟理发过程，顾客进程负责产生顾客请求、等待服务并支付费用，监控进程负责实时展示共享内存中顾客的状态信息。