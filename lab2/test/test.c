#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
void task(int*x);
int pipe[2];
pthread_t thrd1;
int main(){
	int re;
	if(pipe(pipe)<0){
		perror("pipe not created \n");
		exit(EXIT_FAILURE);
	}
	re=pthread_create(&thrd,NULL,(void*)task,NULL);
	if(re){
		perror("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	pthread_join(thrd1,NULL);
	return EXIT_SUCCESS;
}
void task(int*x){
	while(x!=1){
		
	}
}
