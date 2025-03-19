#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BOARD_SIZE 81

int board [9][9];

// Returns 1 if grid is valid, 0 if it is invalid.
int isValidValueInSubGrid(int row, int column) {
    int cellValue = board[row][column] ;
    int countOcurrences = 0;
    const int subgridRowOrigin = (row / 3 ) * 3  ;
    const int subgridColOrigin = (column / 3 ) * 3 ;
    for (int i = 0; i < 3; i ++ ){
        for(int j = 0; j < 3; j++) {
            if ( cellValue == board[subgridRowOrigin + i][subgridColOrigin + j]) {
                countOcurrences ++;
            }
        }
    }
    return (countOcurrences == 1) ? 1 : 0;
}

void* checkColumns(void* vargp) {
    for (int col = 0 ; col < 9; col ++ ) {
        int digits[9] = { 1, 1, 1 , 1 ,1 ,1 ,1, 1, 1 };
        pid_t tid = syscall(SYS_gettid);
        printf("Revisando columna %d, thread: %d\n", col, (int) tid);
        for(int row = 0; row < 9; row++) {
            int index = board[row][col] - 1;
            digits[index] = 1 - digits[index];
        }
        for (int a = 0; a < 9; a++ ) {
            if (digits[a] == 1) {
                return (void *)0;
            }
        }
    }

    return (void *)1;
}

void* checkRows(void* vargp) {
    for(int row = 0; row < 9; row++) {
        int digits[9] = { 1, 1, 1 , 1 ,1 ,1 ,1, 1, 1 };
        pid_t tid = syscall(SYS_gettid);
        printf("Revisando fila %d, thread: %d\n", row, (int) tid);
        for (int col = 0 ; col < 9; col ++ ) {
            int index = board[row][col] - 1;
            digits[index] = 1 - digits[index];
        }
        for (int a = 0; a < 9; a++ ) {
            if (digits[a] == 1) {
                return (void *)0;
            }
        }
    }

    return (void *)1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <sudoku_file>\n", argv[0]);
        return 1;
    }

    int addr = open(argv[1], O_RDONLY);
    if (addr == -1) {
        perror("open");
        return 1;
    }

    void *file = mmap(NULL, BOARD_SIZE, PROT_READ, MAP_SHARED, addr, 0);
    if (file == MAP_FAILED) {
        perror("mmap");
        close(addr);
        return 1;
    }

    char *data = (char *)file;
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i / 9][i % 9] = data[i] - '0';  // Convert char to int
    }

    munmap(file, BOARD_SIZE);
    close(addr);
    
    // Print the board to verify
    printf("Sudoku Board:\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }

    // VALIDATE 3X3 Subgrids
    for (int i = 0; i < BOARD_SIZE; i++) {
        int row = i / 9;
        int column = i % 9;
        int isCellValid = isValidValueInSubGrid(row, column);
        if (isCellValid == 0) {
            fprintf(stderr, "Sudoku board is invalid on position: (%d, %d)\n", column + 1, row + 1);
            return 1;
        }
    }
    
    pid_t parent_pid = getppid();
    
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("Fork failed");
        return 1;
    }
    if (child_pid == 0) {
        char pid_str[10];
        snprintf(pid_str, sizeof(pid_str), "%d", parent_pid);
        execlp("ps", "ps", "-p", pid_str, "-lLf", (char *)NULL);
    } else {
        wait(NULL);
        printf("Revisando columnas");
        pthread_t thread_id;
        int exitStatus = 0;
        pthread_create(&thread_id, NULL, checkColumns, NULL);
        int result = pthread_join(thread_id, (void *)&exitStatus);

        printf("exit status: %d\n", exitStatus);

        printf("Revisando filas");
        pthread_create(&thread_id, NULL, checkRows, NULL);
        pthread_join(thread_id, (void *)&exitStatus);
        printf("exit status: %d\n", exitStatus);

    }

    pid_t child_pid2 = fork();
    if (child_pid2 < 0) {
        perror("Fork failed");
        return 1;
    }

    if (child_pid2 == 0) {
        char pid_str[10];
        snprintf(pid_str, sizeof(pid_str), "%d", parent_pid);
        execlp("ps", "ps", "-p", pid_str, "-lLf", (char *)NULL);
    }
    wait(NULL);
    return 0;
}
