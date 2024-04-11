#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
void task1(int *);
void task2(int *);
int pipe1[2],pipe2[2];
pthread_t thrd1,thrd2;
int main(){
	int ret,num1,num2;
	//using pipe() create two unamed pipe,if failed,exit
	if(pipe(pipe1)<0){
		perror("pipe1 not create\n");
		exit(EXIT_FAILURE);
	}
	if(pipe(pipe2)<0){
		perror("pipe2 not create\n");
		exit(EXIT_FAILURE);
	}
	//use pthread_create create two thread,if failed then exit
	num1=1;
	ret=pthread_create(&thrd1,NULL,(void *)task1,(void*)&num1);
	if(ret){
		perror("pthread_create task1");
		exit(EXIT_FAILURE);
	}
	num2=2;
	ret=pthread_create(&thrd2,NULL,(void *)task2,(void*)&num2);
	if(ret){
		perror("pthread_create task2");
		exit(EXIT_FAILURE);
	}
	pthread_join(thrd2,NULL);
	pthread_join(thrd1,NULL);
	exit(EXIT_SUCCESS);
}
void task1(int * num){
	int x=1;
	do{
		write(pipe1[1],&x,sizeof(int));
		read(pipe2[0],&x,sizeof(int));
		printf("thread %d read:%d\n",*num,x++);
	}while(x<=9);
	close(pipe1[1]);
	close(pipe2[0]);
}
void task2(int * num){
	int x;
	do{
		read(pipe1[0],&x,sizeof(int));
		printf("thread2 read %d\n",x++);
		write(pipe2[1],&x,sizeof(int));
	}while(x<=9);
	close(pipe1[0]);
	close(pipe2[1]);
}
