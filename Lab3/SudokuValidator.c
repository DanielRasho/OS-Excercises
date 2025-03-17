#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BOARD_SIZE 81

int main() {
    int addr = open("sudoku.txt", O_RDONLY);
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
    
    int board [9][9];

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
    
    return 0;
}
