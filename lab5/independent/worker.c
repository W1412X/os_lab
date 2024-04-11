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