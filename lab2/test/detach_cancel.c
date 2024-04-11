#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
void* func(void *pvoid){
	int i=0;
	while(1){
		printf("thread function,i:%d \n",i++);
		sleep(1);
	}
}
int main(){
	pthread_t tid;
	pthread_create(&tid,NULL,func,NULL);
	pthread_detach(tid);//cancel the thread
	
	//cancel the thread two minutes later
	sleep(2);
	pthread_cancel(tid);
	printf("have canceled the thread\n");
	return 0;

}
