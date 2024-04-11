#include <stdio.h>
#include <signal.h>

int main() {
    int pid1 = 8888,pid2=1;
    int result1 = kill(pid1, SIGTERM);
    printf("Test1");
    if (result1 == 0) {
        printf("Send SIGTERM to process %d\n", pid1);
    } else {
        perror("Send failed");
    }
    int result2 = kill(pid2, SIGTERM);
    printf("Test2");
    if (result1 == 0) {
        printf("Send SIGTERM to process %d\n", pid2);
    } else {
        perror("Send failed");
    }
    return 0;
}
