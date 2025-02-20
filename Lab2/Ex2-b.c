#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_NUM 1000000  // Reduced for readability; increase as needed

int main() {
    struct timeval start, end;
    double elapsed_time;
    pid_t p1, p2, p3;

    gettimeofday(&start, NULL); // Start timing

    p1 = fork();
    if (p1 == 0) {
        p2 = fork();
        if (p2 == 0) {
            p3 = fork();
            if (p3 == 0) {
                // Process p3
                for (int i = 0; i < MAX_NUM; i++) {
                    printf("p3: %d\n", i);
                }
                return 0;
            }
            wait(NULL); // Wait for p3
            for (int i = 0; i < MAX_NUM; i++) {
                printf("p2: %d\n", i);
            }
            return 0;
        }
        wait(NULL); // Wait for p2
        for (int i = 0; i < MAX_NUM; i++) {
            printf("p1: %d\n", i);
        }
        return 0;
    }

    wait(NULL); // Wait for p1 (which ensures p2 and p3 also finish)
    
    gettimeofday(&end, NULL); // End timing

    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Execution time: %f seconds\n", elapsed_time);

    return 0;
}