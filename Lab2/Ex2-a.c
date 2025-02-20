#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    for (int i = 0; i < 1000000; i++) {
        printf("%d\n", i);
    }
    for (int i = 0; i < 1000000; i++) {
        printf("%d\n", i);
    }
    for (int i = 0; i < 1000000; i++) {
        printf("%d\n", i);
    }

    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Execution time: %f seconds\n", cpu_time_used);

    return 0;
}