## 示例实验
### ** 2024 4.10**
### 如果你能正常运行那这一段没啥用
### **这个示例实验反正在我电脑上源代码是运行不起来的，具体我修改了一些代码，运行的输出和指导书上的一样**
- 修改ipc.c
  - 把那个对应的set_shm函数中的对共享内存初始化的部分删了，因为在control.c中做了初始化
- 修改了control.c的逻辑
  - 因为要读者优先，所以设置count初值为100，如果有读请求就+1，读请求完成就减1。
  - 如果count在运行的时候=100了，说明这个时候没有读请求，可以尝试从消息队列中获取写请求
- 其他的文件
  - 其他文件应该也修改了一些，忘了在哪改的了，对比一下就能看出来应该
> **Tips1->因为给的代码没有清除消息队列的操作，所以每次调试都需要清除以下消息队列防止下次运行的时候还是用这个消息队列，这里直接写了一个clear.c的文件，编译运行就可以清除消息队列**  
> **Tips2->还有就是他给的Makefile文件我这边一直报错缩进有问题，改来该去还是不行，就按照Makefile的逻辑直接写了一个编译的shell脚本make.sh,根据源码编译的时候直接使用make.sh即可**

### 代码部分
#### 代码的话这里贴出来配置如果出问题，最好直接去github直接克隆下来二进制文件
### make.sh
```shell
#!/bin/bash
gcc -g -c control.c ipc.c
gcc control.o ipc.o -o control
gcc -g -c reader.c ipc.c
gcc reader.o ipc.o -o reader
gcc -g -c writer.c ipc.c
gcc writer.o ipc.o -o writer
rm *.o
```  
- 运行
  - 第一次运行需要添加运行权限
  ```shell
  chmod +x make.sh
  ```
  - 然后编译生成二进制文件就运行
  ```shell
  ./make.sh
  ```
### control.c
```c
#include "ipc.h"
int main(int argc, char *argv[])
{
    int i;
    int rate;
    int w_mid;
    int count = MAXVAL;
    Msg_buf msg_arg;
    struct msqid_ds msg_inf;
    // 建立一个共享内存先写入一串 A 字符模拟要读写的内容
    buff_key = 101;
    buff_num = STRSIZ + 1;
    shm_flg = IPC_CREAT | 0644;
    buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
    for (i = 0; i < STRSIZ; i++){
        buff_ptr[i] = 'A';
    }
    buff_ptr[i]='\0';
    // 建立一条请求消息队列
    quest_flg = IPC_CREAT | 0644;
    quest_key = 201;
    quest_id = set_msq(quest_key, quest_flg);
    // 建立一条响应消息队列
    respond_flg = IPC_CREAT | 0644;
    respond_key = 202;
    respond_id = set_msq(respond_key, respond_flg);
    // 控制进程准备接收和响应读写者的消息
    printf("Wait quest \n");
    int if_get_r_msg=0;
    while (1)
    {
         usleep(1000000);
        // 当 count 大于 0 时说明没有新的读写请求，查询是否有任何新请求
        if (count >= 100)
        {
            quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
            int r=msgrcv(quest_id, &msg_arg, sizeof(msg_arg), READERQUEST, quest_flg);
            if ( r>= 0)
            {
                // 有读者请求，允许读者读
                if_get_r_msg=1;
                count+=1;
                msg_arg.mtype = msg_arg.mid;
                msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
                printf("%d quest read\n", msg_arg.mid);
            }
            if (msgrcv(quest_id, &msg_arg, sizeof(msg_arg), READFINISHED, quest_flg) >= 0)
            {
                // 有读者完成
                if_get_r_msg=1;
                count-=1;
                printf("%d reader finished\n", msg_arg.mid);
            }
        }
        if (count==100)
        {
            if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), WRITERQUEST, quest_flg) >= 0)
            {
                // 有写者请求
                msg_arg.mtype = msg_arg.mid;
                printf("mid %d\n",msg_arg.mid);
                msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
                printf("%d quest write\n", msg_arg.mid);
            }
            // 以阻塞方式接收消息 .
            if (msgrcv(quest_id, &msg_arg, sizeof(msg_arg), WRITEFINISHED, quest_flg) >= 0)
            {
                // 有读者完成
                if_get_r_msg=1;
                printf("%d write finished\n", msg_arg.mid);
            }

        }
    }
    return EXIT_SUCCESS;
}
```
### reader.c
```c
#include "ipc.h"
int main(int argc, char *argv[])
{
    int i;
    int rate;
    Msg_buf msg_arg;

    // 可在在命令行第一参数指定一个进程睡眠秒数，以调解进程执行速度
    if (argv[1] != NULL)
        rate = atoi(argv[1]);
    else
        rate = 3;
    // 附加一个要读内容的共享内存
    buff_key = 101;
    buff_num = STRSIZ + 1;
    shm_flg = IPC_CREAT | 0666;
    buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
    // 联系一个请求消息队列
    quest_flg = IPC_CREAT | 0644;
    quest_key = 201;
    quest_id = set_msq(quest_key, quest_flg);
    // 联系一个响应消息队列
    respond_flg = IPC_CREAT | 0644;
    respond_key = 202;
    respond_id = set_msq(respond_key, respond_flg);
    // 循环请求读
    msg_arg.mid = getpid();
    while (1)
    {
        // 发读请求消息
        msg_arg.mtype = READERQUEST;
        msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
        printf("%d reader quest\n", msg_arg.mid);
        // 等待允许读消息
        msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
        printf("%d reading: %s\n", msg_arg.mid, buff_ptr);
        // 发读完成消息
        msg_arg.mtype = READFINISHED;
        msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
        sleep(rate);
    }
    return EXIT_SUCCESS;
}
```
### writer.c
```c
#include "ipc.h"
int main(int argc, char *argv[])
{
    int i, j = 0;
    int rate;
    Msg_buf msg_arg;

    // 可在在命令行第一参数指定一个进程睡眠秒数，以调解进程执行速度
    if (argv[1] != NULL)
        rate = atoi(argv[1]);
    else
        rate = 3;
    // 附加一个要读内容的共享内存 
    buff_key = 101;
    buff_num = STRSIZ + 1;
    shm_flg = IPC_CREAT | 0644;
    buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
    // 联系一个请求消息队列
    quest_flg = IPC_CREAT | 0666;
    quest_key = 201;
    quest_id = set_msq(quest_key, quest_flg);
    // 联系一个响应消息队列
    respond_flg = IPC_CREAT | 0666;
    respond_key = 202;
    respond_id = set_msq(respond_key, respond_flg);
    // 循环请求写
    msg_arg.mid = getpid();
    while (1)
    {
        // 发写请求消息
        msg_arg.mtype = WRITERQUEST;
        msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
        printf("%d writer quest\n", msg_arg.mid);
        // 等待允许写消息
        msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
        // 写入 STRSIZ 个相同的字符
        for (i = 0; i < STRSIZ; i++)
            buff_ptr[i] = 'A' + j;
        j = (j + 1) % STRSIZ; // 按 STRSIZ 循环变换字符
        printf("%d writing: %s\n", msg_arg.mid, buff_ptr);
        // 发写完成消息
        msg_arg.mtype = WRITEFINISHED;
        msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
        sleep(rate);
    }
    return EXIT_SUCCESS;
}
```
### ipc.c
```c
#include "ipc.h"
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
/*
*
set_sem 函数建立一个具有 n 个信号量的信号量
*
如果建立成功，返回 一个信号量数组的标识符 sem_id
*
输入参数：
*
sem_key 信号量数组的键值
*
sem_val 信号量数组中信号量的个数
*
sem_flag 信号量数组的存取权限
*/
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
/*
*
set_shm 函数建立一个具有 n 个字节 的共享内存区
*
如果建立成功，返回 一个指向该内存区首地址的指针 shm_buf
*
输入参数：
*
shm_key 共享内存的键值
*
shm_val 共享内存字节的长度
*
shm_flag 共享内存的存取权限
*/
char *set_shm(key_t shm_key, int shm_num, int shm_flg)
{
    int i, shm_id;
    char *shm_buf;
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
        if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0)
        {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
    }
    // shm_key 标识的共享内存区已经建立 , 将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0)
    {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}
/*
*
set_msq 函数建立一个消息队列
*
如果建立成功，返回 一个消息队列的标识符 msq_id
*
输入参数：
*
msq_key 消息队列的键值
*
msq_flag 消息队列的存取权限
*/
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
### ipc.h
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#define BUFSZ 256
#define MAXVAL 100
#define STRSIZ 8
#define WRITERQUEST 1
#define READERQUEST 2
#define READFINISHED 3
#define WRITEFINISHED 4
typedef union semuns {
int val;
} Sem_uns;
typedef struct msgbuf {
long mtype;
int mid;
} Msg_buf;
key_t buff_key;
int buff_num;
char *buff_ptr;
int shm_flg;
int quest_flg;
key_t quest_key;
int quest_id;
int respond_flg;
key_t respond_key;
int respond_id;
int get_ipc_id(char *proc_file,key_t key);
char *set_shm(key_t shm_key,int shm_num,int shm_flag);
int set_msq(key_t msq_key,int msq_flag);
int set_sem(key_t sem_key,int sem_val,int sem_flag);
int down(int sem_id);
int up(int sem_id);
```
### clear.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSG_SIZE  80
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;
    key = 201;
    msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
    key=202;
    msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

```

### 写者饥饿
> **因为读者饥饿改的代码较多，不在这里写了，可以取github获取所有代码**
#### 修改
- 修改read.c源文件while的sleep(rate)为usleep(100)
#### 运行
- 利用make.sh编译运行
- 一个示例的运行步骤
  - 终端1
    - 运行control
  - 终端2
    - 运行writer
  - 终端3
    - 等待一些前两个的输出后运行reader
  > 会发现运行reader后一直输出reader的信息，没有writer的信息。writer饥饿