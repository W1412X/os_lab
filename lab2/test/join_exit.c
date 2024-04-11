#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
void *func1(void*pvoid){
	pthread_exit("func1 exit!");
}
void* func2(void *pvoid){
	static int i=0;
	while(1){
		printf("thread function 2, i: %d\n",i++);
		sleep(1);
		if(i==2){
			pthread_exit((void*)&i);
		}
	}
}
int main(){
	int* reVal;
	char *str;
	pthread_t tid1,tid2;
	pthread_create(&tid1,NULL,func1,NULL);
	pthread_create(&tid2,NULL,func2,NULL);
	pthread_join(tid1,(void**)&str);
	printf("thread 1 run a str: %s\n",str);
	pthread_join(tid2,(void **)&reVal);
	printf("thread2 return a value : %d",*reVal);
	return 0;

}
