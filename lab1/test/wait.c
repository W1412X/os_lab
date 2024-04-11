#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
int main(){
    int pid=fork();
    if(pid==-1){
        printf("fork failed\n");
    }else if(pid==0){
        printf("this is the child process\n");
    }else{
        wait(NULL);
        printf("this is the parent process\n");
    }
    return 0;
}