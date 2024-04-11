#include"/home/wangxv/Files/hw/oshw/lab/lab1/example/pctl.h"
int main(int argc,char*argv[]){
    int i,pid,status;
    char *args[]={"/bin/ls","-a",NULL};
    signal(SIGINT, (void (*)(int))sigcat);
    perror("SIGINT\n");
    pid=fork();
    if(pid<0){
        printf("Create process failed\n");
        exit(EXIT_FAILURE);
    }
    if(pid==0){
        printf("I am a Child process%d\nMy father is %d\n",getpid(),getppid());
        pause();
        printf("%d Child process will running:",getpid());
        if(argv[1]!=NULL){
            for(i=1;argv[i]!=NULL;i++){
                printf("%s ",argv[i]);
            }
            status=execve(argv[1],&argv[1],NULL);
        }else{
            for(i=0;args[i]!=NULL;i++){
                printf("%s ",args[i]);
            }
            status=execve(args[0],args,NULL);
        }
    }else{
        printf("\nI am Parent process %d\n",getpid());
        if(argv[1]!=NULL){
            printf("%d wait for child done\n",getpid());
            waitpid(pid,&status,0);
            printf("Mychild process exit! status=%d\n\n",status);
        }else{
            sleep(5);
            if(kill(pid,SIGINT)>=0){
                printf("%d WakeUp %d child.\n",getpid(),pid);
                printf("%d don't wait for child done\n\n",getpid());
            }
        }
    }
    return EXIT_SUCCESS;
}