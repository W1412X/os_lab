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