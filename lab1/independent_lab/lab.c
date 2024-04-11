#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
void sigcat(){
    printf("PID %d continue\n",getpid());
}
int main(){
    while(1){
        printf("NEW CIRCLE\n");
        int status;
        pid_t child_pid1,child_pid2;
        signal(SIGINT,(void(*)(int))sigcat);
        child_pid1=fork();
        if(child_pid1<0){
            printf("Creat First process failed\n");
        }else if(child_pid1==0){
            printf("First Process Created PID %d\n",getpid());
            printf("First Process Paused PID %d\n",getpid());
            pause();
            printf("First Process Continue PID %d\n",getpid());
            execlp("/bin/ls","ls",NULL);
        }else{
            child_pid2=fork();
            if(child_pid2<0){
                printf("Creat Second process failed\n");
            }else if(child_pid2==0){
                printf("Second Process Created PID %d\n",getpid());
                status=execlp("/bin/ps","-n",NULL);
            }else{
                waitpid(child_pid2,&status,0);
                kill(child_pid1,SIGINT);
            }
            wait(NULL);
        }
        sleep(3);
    }
}