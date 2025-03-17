#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define SHM_NAME "/my_shm"
#define SHM_SIZE 160

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number> <letter>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    char x = argv[2][0];
    
    printf("Hi from %c, n=%d\n", x,n);
    
    // Create shared memory and store its file descriptor.
    int shared_memory_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shared_memory_fd == -1) {
        if (errno == EEXIST) {
            printf("Shared memory already created, reusing it...\n");
            shared_memory_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
        } else {
            perror("shm_open");
            return 1;
        }
    } else {
        if (ftruncate(shared_memory_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            close(shared_memory_fd);
            shm_unlink(SHM_NAME); // Remove shared memory if failed
            return 1;
        }
    }

    // Map the shared memory into the process's address space
    void *shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shared_memory_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        close(shared_memory_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(shared_memory_fd);
        close(pipe_fd[0]);  // Close both ends of the pipe
        close(pipe_fd[1]);
        shm_unlink(SHM_NAME);
        return 1;
    }
    
    // OPERATING ON PARENT
    if (pid > 0) {
        close(pipe_fd[0]);  // Close Read pipe
        for (int i = 0; i < SHM_SIZE; i++) {
            if (i % n == 0) {
                write(pipe_fd[1], &x, 1); // Write just one character (fixed)
            }
        }
        close(pipe_fd[1]);
        wait(NULL);
        
        printf("Current memory data:\n");
        printf("%s\n", (char *)shm_ptr);
        shm_unlink(SHM_NAME);
    } 
    // OPERATING ON CHILD
    else {
        close(pipe_fd[1]);  // Close Write pipe
        
        char buffer[1];
        ssize_t bytesRead;

        for (int i = 0; i < SHM_SIZE; i++) {
        // Check if the block is empty (assuming '\0' indicates an empty block)
        if (((char *)shm_ptr)[i] == '\0') {
            bytesRead = read(pipe_fd[0], buffer, 1);

            if (bytesRead == -1) {
                fprintf(stderr, "%s: Failed to read from pipe!\n", &x);
                close(pipe_fd[0]);
                return 1;
            } else if (bytesRead == 0) {
                break;
            } else {
                // Write the byte to the shared memory
                ((char *)shm_ptr)[i] = buffer[0];
            }
        }
        }
        
        close(pipe_fd[0]);
        return 0;
    }

    munmap(shm_ptr, SHM_SIZE);
    close(shared_memory_fd);
    return 0;
}