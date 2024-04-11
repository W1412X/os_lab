#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int pipe_x[2];
int pipe_y[2];

struct HelpX {
    int x, last_result;
};

void f_x(struct HelpX *h) {
    if (h->x != 1) {
        h->last_result = h->last_result * h->x;
        h->x--;
        f_x(h);
    } else {
        printf("thread f_x get result %d\n", h->last_result);
        write(pipe_x[1], &h->last_result, sizeof(int));
    }
    close(pipe_x[1]);
}

void f_y(int *y) {
    if (*y <= 2) {
        int tmp = 1;
        printf("thread f_y get result %d\n", tmp);
        write(pipe_y[1], &tmp, sizeof(int));
    } else {
        int a = 1, b = 1;
        int tmp;
        for (int i = 3; i <= *y; i++) {
            tmp = a + b;
            a = b, b = tmp;
        }
        printf("thread f_y get result %d\n", tmp);
        write(pipe_y[1], &tmp, sizeof(int));
    }
    close(pipe_y[1]);
}

void f_xy() {
    int re_x, re_y;
    read(pipe_x[0], &re_x, sizeof(int));
    read(pipe_y[0], &re_y, sizeof(int));
    int sum = re_x + re_y;
    printf("thread f_xy get result %d\n", sum);
}

int main() {
    struct HelpX x;
    int y;
    printf("Enter x, y: ");
    scanf("%d %d", &x.x, &y);
    x.last_result = 1;

    if (pipe(pipe_x) < 0 || pipe(pipe_y) < 0) {
        perror("create pipe failed");
        exit(EXIT_FAILURE);
    }

    pthread_t thrdx, thrdy, thrdxy;
    pthread_create(&thrdy, NULL, (void *(*)(void *))f_y, &y);
    pthread_create(&thrdx, NULL, (void *(*)(void *))f_x, &x);
    pthread_create(&thrdxy, NULL, (void *(*)(void))f_xy, NULL);

    pthread_join(thrdx, NULL);
    pthread_join(thrdy, NULL);
    pthread_join(thrdxy, NULL);

    return 0;
}
