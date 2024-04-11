#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE  80

// 定义消息结构体
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;

    // 生成唯一的键值
    key = 201;

    // 获取消息队列
    msgid = msgget(key, 0666 | IPC_CREAT);
    // 删除消息队列
    msgctl(msgid, IPC_RMID, NULL);
    key=200;
        // 获取消息队列
    msgid = msgget(key, 0666 | IPC_CREAT);
    // 删除消息队列
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
