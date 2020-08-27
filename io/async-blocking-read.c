#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
    int rc, max_sd, on = 1;
    fd_set master_set;
    struct timeval timeout;
    timeout.tv_sec  = 3 * 60;
    timeout.tv_usec = 0;

    // create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        perror("error creating socket");
        exit(-1);
    }

    // init fd set
    FD_ZERO(&master_set);
    max_sd = socket_desc;
    FD_SET(socket_desc, &master_set);

    // set port and IP
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to server
    printf("connect():\n");
    if (connect(socket_desc,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        exit(-1);
    }

    printf("select():\n");
    time_t start_seconds = time(NULL); 
    rc = select(max_sd + 1, &master_set, NULL, NULL, &timeout);
    time_t end_seconds = time(NULL);
    printf("  (duration: %ld secs)\n", (end_seconds - start_seconds));

    if (rc < 0)
    {
        perror("  select() failed");
    }

    if (rc == 0)
    {
        printf("  select() timed out.\n");
    }

    // check if this socket_desc is ready
    if (FD_ISSET(socket_desc, &master_set))
    {
        syncRead(socket_desc);
    }

    printf("do something else...\n");

    close(socket_desc);
    return 0;
}
