#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <omp.h>

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

void* checkColumns() {
    int isValid = 1; 

    omp_set_nested(1);
    omp_set_num_threads(9);
    #pragma omp parallel for schedule(dynamic)
    for (int col = 0; col < 9; col++) {
        int digits[9] = { 1, 1, 1 , 1 ,1 ,1 ,1, 1, 1 };
        pid_t tid = syscall(SYS_gettid);
        printf("Revisando columna %d, thread: %d\n", col, (int) tid);

        for(int row = 0; row < 9; row++) {
            int index = board[row][col] - 1;
            digits[index] = 1 - digits[index];
        }

        for (int a = 0; a < 9; a++) {
            if (digits[a] == 1) {
                isValid = 0; 
                break; 
            }
        }
    }

    return (void *)isValid; 
}

void* checkRows() {
    int isValid = 1; 
    omp_set_nested(1);
    omp_set_num_threads(9);
    #pragma omp parallel for schedule(dynamic)
    for(int row = 0; row < 9; row++) {
        int digits[9] = { 1, 1, 1 , 1 ,1 ,1 ,1, 1, 1 };
        pid_t tid = syscall(SYS_gettid);
        printf("Revisando fila %d, thread: %d\n", row, (int) tid);

        for (int col = 0; col < 9; col++) {
            int index = board[row][col] - 1;
            digits[index] = 1 - digits[index];
        }

        for (int a = 0; a < 9; a++) {
            if (digits[a] == 1) {
                isValid = 0; 
                break; 
            }
        }
    }

    return (void *)(intptr_t)isValid; 
}

void checkProcessInfo(pid_t pid) {
    int stringSize = 10;
    char pid_str[stringSize];
    sprintf(pid_str, "%d", pid);
    execlp("ps", "ps", "-p", pid_str, "-lLf", (char *)NULL);
}

int main(int argc, char *argv[]) {
    omp_set_num_threads(1);
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
        checkProcessInfo(parent_pid);
    }
        
    // Validate columns
    wait(NULL);
    printf("Revisando columnas");
    pthread_t thread_id;
    int exitStatus = 0;
    pthread_create(&thread_id, NULL, checkColumns, NULL);
    int result = pthread_join(thread_id, (void *)&exitStatus);

    if (exitStatus == 0 ) {
        printf("INVALID COLUMNS");
        return 1;
    }

    // Validate rows
    printf("Revisando filas");
    pthread_create(&thread_id, NULL, checkRows, NULL);

    pid_t child_pid2 = fork();
    
    pthread_join(thread_id, (void *)&exitStatus);

    if (exitStatus == 0 ) {
        printf("INVALID ROWS");
        return 1;
    }

    if (child_pid2 < 0) {
        perror("Fork failed");
        return 1;
    }

    if (child_pid2 == 0) {
        checkProcessInfo(parent_pid);
    }

    wait(NULL);
    return 0;
}
