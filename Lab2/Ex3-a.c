#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>


int main() {
    
    pid_t child = fork();

    if (child == 0) {
        for (int i = 0; i < 5000000; i++){
            printf("Hello from child");
        }
    } else {
        while (1){}
    }
    
    return 0;
}