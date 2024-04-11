#include<unistd.h>
#include<stdio.h>
#include<signal.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
int pipe_help[2];
char** split_string(const char *input_string, int *length) {
    char *token;
    char *input_copy = strdup(input_string);
    char **result = NULL;
    int count = 0;
    token = strtok(input_copy, "|");
    while (token != NULL) {
        count++;
        token = strtok(NULL, "|");
    }
    result = (char**)malloc(count * sizeof(char*));
    if (result == NULL) {
        *length = 0;
        return NULL;  // 内存分配失败
    }
    strcpy(input_copy, input_string);
    token = strtok(input_copy, "|");
    for (int i = 0; i < count; i++) {
        result[i] = strdup(token); 
        token = strtok(NULL, "|");
    }

    *length = count;
    free(input_copy);
    return result;
}
void extract_single_command(char*command,char**tokens){
    char *token; // 动态分配空间
    int i = 0;
    token = strtok(command, " ");
    while (token != NULL) {
        tokens[i] = strdup(token); // 使用 strdup 复制字符串并将指针存入数组
        i++;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL; // 在数组末尾添加一个 NULL 指针作为结束标记
}
void removeAtSymbol(char *str) {//用于后台运行的标志符
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '@') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}
int child_pid;//save the pid of the subprocess
void sigint_handler(int signum) {
    printf("\nCtrl+C pressed. Exiting command\n");
    kill(child_pid, SIGINT);
}
void parse_output_string(const char *input_string, char result[2][100]) {
    char *token;
    char input_copy[100];
    strcpy(input_copy, input_string);
    token = strtok(input_copy, ">");
    strcpy(result[0], token);

    token = strtok(NULL, ">");
    if (token != NULL) {
        // 去除右侧字符串的开头空格
        while (*token == ' ') {
            token++;
        }
        int end = strlen(token) - 1;
        // 去除右侧字符串的末尾空格
        while (end >= 0 && token[end] == ' ') {
            token[end] = '\0';
            end--;
        }
        strcpy(result[1], token);
    } else {
        strcpy(result[1], "");
    }
}
void parse_input_string(const char *input_string, char result[2][100]) {
    char *token;
    char input_copy[100];
    strcpy(input_copy, input_string);
    token = strtok(input_copy, "<");
    if (token != NULL) {
        // 去除右侧字符串的开头空格
        while (*token == ' ') {
            token++;
        }
        int end = strlen(token) - 1;
        // 去除右侧字符串的末尾空格
        while (end >= 0 && token[end] == ' ') {
            token[end] = '\0';
            end--;
        }
        strcpy(result[0], token);
    } else {
        strcpy(result[0], "");
    }
    token = strtok(NULL, "<");
    strcpy(result[1], token);

}
int judge_command_length(char*command){
    if(strlen(command)>=100){
            printf("\033[1;31mError -> input command length over the limit\n");
            printf("\033[0m");
            return 0;
    }
    return 1;
}
int if_in_sub(char* command){
    if(strchr(command,'@')!=NULL){
        return 1;
    }
    return 0;
}
void execute_out_redirection(char *command,int background){//
    char result[2][100];
    parse_output_string(command,result);
    int log_fd;
    int stdout_backup;
    log_fd=open(result[1],O_APPEND | O_CREAT | O_WRONLY,0644);
    if(log_fd<0){
        printf("\033[1;33mError -> Failed to log to the file\n");
        return;
    }
    stdout=dup(1);
    if(dup2(log_fd,1)<0){
        printf("\033[1;31mError -> can not redirecting to log file\n");
        return;
    }
    char **tokens=(char **)malloc(20 * sizeof(char*));
    extract_single_command(result[0],tokens);    
    if(execvp(tokens[0],tokens)==-1){
        printf("\033[1;31mError -> Cannot find command\n");
    }
    dup2(stdout_backup,1);
    close(log_fd);
    close(stdout_backup);
    free(tokens);
}
void execute_input_redirection(char *command,int background){//
    char result[2][100];
    parse_input_string(command,result);
    int input_fd;
    int stdin_backup;
    input_fd=open(result[0],O_RDONLY);
    if(input_fd<0){
        printf("\033[1;33mError -> Failed to open the file\n");
        return;
    }
    stdout=dup(1);
    if(dup2(input_fd,0)<0){
        printf("\033[1;31mError -> can not read the file\n");
        return;
    }
    //
    char **tokens=(char **)malloc(20 * sizeof(char*));
    extract_single_command(result[1],tokens);    
    if(execvp(tokens[0],tokens)==-1){
        printf("\033[1;31mError -> Cannot find command\n");
    }
    dup2(stdin_backup,1);
    close(input_fd);
    close(stdin_backup);
    free(tokens);
}
void execute_pipe_command(char *command,int background){
    char **command_set;
    int command_num;
    command_set=split_string(command,&command_num);
    if(pipe(pipe_help)<0){
        printf("\033[1;31mError -> Cannot create pipe\n");
        return;
    }
    for(int i=0;i<command_num;i++){
        if(i!=0){//读取管道的输入
            dup2(pipe_help[0],STDIN_FILENO);
        }
        dup2(pipe_help[1],STDOUT_FILENO);
        char **tokens=(char **)malloc(20 * sizeof(char*));
        extract_single_command(command_set[i],tokens);
        if(execvp(tokens[0],tokens)==-1){
            printf("\033[1;31mError -> Cannot find command\n");
        }
    }
}
void execute_normal_command(char *command,int background){//
    int log_fd;
    int stdout_backup;
    char result[2][100];
    if(background==1){
        log_fd=open("log.txt",O_APPEND | O_CREAT | O_WRONLY,0644);
        if(log_fd<0){
            printf("\033[1;31mError -> Failed to log to the file\n");
            return;
        }
        stdout=dup(1);
        if(dup2(log_fd,1)<0){
            printf("\033[1;31mError -> can not redirecting to log file\n");
            return;
        }
    }
    char **tokens=(char **)malloc(20 * sizeof(char*));
    extract_single_command(command,tokens);
    if(execvp(tokens[0],tokens)==-1){
        printf("\033[1;31mError -> Cannot find command\n");
    }
    if(background==1){
        dup2(stdout_backup,1);
        close(log_fd);
        close(stdout_backup);
    }
    free(tokens);
}
void test(){
    printf("\033[1;31mthis is a test\n");
    printf("\033[01m");
}
int main(){
    int background;
    char*command;
    rl_attempted_completion_function = NULL;
    using_history();
    while(1){
        //
        int current_index=0;
        int command_length=0;
        background=0;
        printf("\033[1;34m>>");
        printf("\033[0m");
        command=readline(" ");
        add_history(command);
        //判断用户输入的命令长度的合法
        if(judge_command_length(command)==0){
            continue;
        }
        //解析命令
        //judge if background
        background=if_in_sub(command);
        removeAtSymbol(command);
        int a=fork();
        if(a<0){
            printf("\033[1;31mError -> failed to create process\n");
            printf("\033[0m");
            continue;
        }else if(a==0){//
        printf("ssss");
            signal(SIGINT,SIG_DFL);
            int tmp_back=background;
            char tmp_command[100];
            strcpy(tmp_command,command);
            execute_input_redirection(command,background);
            exit(EXIT_SUCCESS);
            
        }else{//父进程
        
            child_pid=a;
            signal(SIGINT,sigint_handler);
            if(background==1){
                //输出输出到对应的log.txt文件中
            }else{//在父进程中等待执行
                wait(NULL);
            }
        }
        free(command);
    }
}
