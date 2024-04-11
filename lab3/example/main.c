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
int if_first_pipe=0;
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
}
void test(){
    printf("\033[1;31mthis is a test\n");
    printf("\033[01m");
}
int pipe_prev[2];
void execute_pipe_recursion(char *commands[], int n,int*pipe_prev) {
    int pipefd[2];
    pid_t pid1, pid2;
    char **tokens = (char **)malloc(20 * sizeof(char *));
            extract_single_command(commands[0], tokens);
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Child process 1: ls
        close(pipefd[0]); // Close reading end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the writing end of the pipe
        close(pipefd[1]); // Close the writing end of the pipe

        char *ls_args[] = {"ls", NULL};
        execvp(tokens[0], tokens);
    } else {
            // Child process 2: wc -w
            wait(NULL);
            close(pipefd[1]); // Close writing end of the pipe
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the reading end of the pipe
            close(pipefd[0]); // Close the reading end of the pipe

            char *wc_args[] = {"wc", "-w", NULL};
            extract_single_command(commands[1], tokens);
            execvp(tokens[0], tokens);
    }
}
void execute_pipe_command(char *command, int background) {
    char **command_set;
    int command_num;
    if (pipe(pipe_prev) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    command_set=split_string(command,&command_num);
    execute_pipe_recursion(command_set,command_num,pipe_prev);
}
bool if_with_char(char *s,char c){
    char *tmp=s;
    while(*tmp!='\0'){
        if(*tmp==c){
            return 1;
        }
        tmp++;
    }
    return 0;
}
int main(){
    int background;
    char*command;
    rl_attempted_completion_function = NULL;
    char cwd[1024]; // 声明一个足够大的字符数组来存储当前工作目录的路径
    using_history();
    while(1){
        //
        if_first_pipe=0;
        int current_index=0;
        int command_length=0;
        background=0;
        getcwd(cwd,sizeof(cwd));
        printf("\033[1;34m%s",cwd);
        printf("\033[0m");
        command=readline("$ ");
        add_history(command);
        //判断用户输入的命令长度
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
            signal(SIGINT,SIG_DFL);
            int tmp_back=background;
            char tmp_command[100];
            strcpy(tmp_command,command);
            if(if_with_char(tmp_command,'|')){
                execute_pipe_command(tmp_command,tmp_back);
            }else if(if_with_char(tmp_command,'<')){
                execute_input_redirection(tmp_command,tmp_back);
            }else if(if_with_char(tmp_command,'>')){
                execute_out_redirection(tmp_command,tmp_back);
            }else{
                execute_normal_command(tmp_command,tmp_back);
            }
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
