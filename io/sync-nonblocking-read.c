#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

void syncRead(int fd) {
    printf("read():\n");
    int buffer_size = 100;
    unsigned char buffer[buffer_size];
          
    time_t start_seconds = time(NULL); 
    int bytes_read = read(fd, buffer, buffer_size);
    time_t end_seconds = time(NULL); 

    // print message from server
    printf("  ");
    for(int i=0; i<bytes_read; i++) {
        printf("%c", buffer[i]);
    }
    printf(" (duration: %ld secs)\n", (end_seconds - start_seconds));
}

int main(int argc, char* argv[]) {
    // create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        perror("error creating socket");
        exit(-1);
    }

    // set port and IP
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("connect():\n");
    // connect to server
    if (connect(socket_desc,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        exit(-1);
    }

    // set socket to be non-blocking
    int on = 1;
    int rc = ioctl(socket_desc, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(socket_desc);
        exit(-1);
    }

    syncRead(socket_desc);

    printf("do something else...\n");
    
    for(int i=0; i<6; i++) {
        sleep(1);
        printf("sleep 1s...\n");
        syncRead(socket_desc);
    }

    close(socket_desc);
    return 0;
}
