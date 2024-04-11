#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MSG_SIZE 128
// 定义消息结构体
struct message {
    long type;
    char text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;

    // 生成key
    if ((key = ftok(".", 'a')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // 创建消息队列
    if ((msgid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 发送消息
    msg.type = 1; // 设置消息类型为1
    strcpy(msg.text, "Hello, this is a message from process 1!");
    if (msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("Message sent: %s\n", msg.text);

    // 接收消息
    if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 1, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("Message received: %s\n", msg.text);

    // 删除消息队列
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID");
        exit(EXIT_FAILURE);
    }

    return 0;
}
