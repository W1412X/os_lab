#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
void* thread_to_do_func1(void* tid){
    while(1){
	printf("This is a new thread 1\n");
	}
};
void* thread_to_do_func2(void* tid){
    while(1){
		printf("This is a new thread 2\n");
	}
	}
void* thread_to_do_func3(void* tid){
	while(1){
		printf("This is a new thread 3\n");
	}
	}
int main(){
    pthread_t tid[4];
    int re=pthread_create(&tid[0],NULL,thread_to_do_func1,NULL);
    re=pthread_create(&tid[1],NULL,thread_to_do_func2,NULL);
    re=pthread_create(&tid[2],NULL,thread_to_do_func3,NULL);
	while(1){
		sleep(1);
	}
}
