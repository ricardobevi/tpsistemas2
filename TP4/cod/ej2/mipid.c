#include <unistd.h>
#include <stdio.h>
int main() {
    printf("PID: %d PPID: %d\n", getpid(), getppid());
    sleep(10);
    return 0;
}