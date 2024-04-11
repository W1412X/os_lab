#include<stdio.h>
#include<unistd.h>
int main(){
    int pid=fork();
    if(pid==-1){
        printf("fork failed\n");
    }else if(pid==0){
        printf("child process\n");
    }else{
        printf("parent process\n");
    }
    return 0;
}
