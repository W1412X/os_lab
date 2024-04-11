#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
int main(){
    int pid=fork();
    if(pid==-1){
        printf("fork failed\n");
    }else if(pid==0){
        printf("child process\n");
        printf("CHILD PID:%d  PARENT PID:%d\n",getpid(),getppid());
    }else{
        printf("parent process\n");
    }
    return 0;
}
