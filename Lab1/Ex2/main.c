#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

int main(int argc, char* argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    const char* source_path = argv[1];
    const char* dest_path = argv[2];
    
    // Open source file for reading
    int source_fd = open(source_path, O_RDONLY);
    if (source_fd < 0){
        perror("Error opening source file");
        return 1;
    }
    
    // Open destinatoin file (create if doesn't exist, truncate if it does)
    // The 0644 stands for the file permissions.
    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error opening destination file");
        close(source_fd);
        return 1;
    }
    
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    
    // Transfering data via buffer.
    // read() from file in chunks of BUFFER_SIZE and place then in buffer variable.
    while((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        // Data from the buffer is written in dest file, An returns the 
        // No of bytes written.
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written < 0 ){
            perror("Error writing to destination file");
            close(source_fd);
            close(dest_fd);
            return 1;
        }
    }

    if (bytes_read < 0){
        perror("Error reading from source file");
    }
    
    
    // Close resources
    close(source_fd);
    close(dest_fd);
    
    printf("File %s copied succesfully to %s\n", source_path, dest_path);
    return 0;
}