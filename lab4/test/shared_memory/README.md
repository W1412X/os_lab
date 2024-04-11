## 共享内存
### shmat()
> 用于将共享内存段附加到当前进程的地址控件
- usage
  ```c
  #include<sys/types.h>
  #include<sys/shm.h>
  void *shmat(int shmid,const void*shmaddr,int shmflg);
  ```
  - shmid -> 共享内存段的标识符
  - shmaddr -> 指定共享内存段附加到进程地址控件的地址，一般设置为NULL
  - shmflg -> 附加标志，一般设0
- exmaple 
  ```c
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/shm.h>
  int main() {
    key_t key = ftok("shmfile",65); // 生成共享内存的key
    int shmid = shmget(key,1024,0666|IPC_CREAT); // 创建共享内存
    char *shmaddr = (char*) shmat(shmid,(void*)0,0); // 将共享内存附加到当前进程
    printf("共享内存内容：%s\n",shmaddr);
    shmdt(shmaddr); // 分离共享内存
    return 0;
  }
  ```
### 示例代码
#### 代码实现功能
- 创建一个进程每隔1s生成随机数存入共享内存中
- 在第一个进程创建5s后创建一个进程不断读取共享内存中的随机数并输出