#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main () {
    
    pid_t child = fork();

    if (child == 0) {
        execl("./bin/ipc", "./bin/ipc", "10", "a", (char *)NULL);
        perror("execl");
    } else {
        execl("./bin/ipc", "./bin/ipc", "12", "b", (char *)NULL);
        perror("execl");
    }

    return 0;
}