#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char *input;

    // 初始化readline
    rl_attempted_completion_function = NULL;
    using_history();

    while (1) {
        // 读取用户输入
        input = readline("\033[1;34m>> ");

        // 添加输入到历史记录
        add_history(input);

        // 打印用户输入
        printf("You entered: %s\n", input);

        // 释放输入内存
        free(input);
    }

    return 0;
}
y