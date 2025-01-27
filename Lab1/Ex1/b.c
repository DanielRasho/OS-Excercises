#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello World!\n");
        printf("Parent Process: %d\n", (int) getpid());
    
    int f = fork();
        
    if (f == 0) {
        execl("a.out", (char*)NULL);
    } else {
        printf("Child Process: %d\n", (int) getpid());
        execl("a.out", (char*)NULL);
    }
    return 0;
}