#include <stdio.h>
#include <unistd.h>

int main() {

    int f = fork();
        
    if (f == 0) {
        execl("./a", (char*)NULL);
    } else {
        printf("Parent Process: %d\n", (int) getpid());
        execl("./a", (char*)NULL);
        printf("NOthing printing");
    }
    return 0;
}