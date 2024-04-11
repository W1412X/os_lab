#include<unistd.h>
int main(){
    char* argv[]={"ls","-al","/home/wangxv/Files/hw/oshw/lab/lab1",NULL};
    execvp("ls",argv);
}