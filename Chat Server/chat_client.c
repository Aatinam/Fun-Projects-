#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define BUF_SIZE 128

int main(void) {

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        exit(1);
    }

    // Reads input from the user, sends it to the server, and then accepts the
    // echo that returns. Exit when stdin is closed.
    printf("%s\n", "Please enter username:" );
    char get_user_name[100];
    int check_read = read(STDIN_FILENO, get_user_name, 100);
    get_user_name[check_read - 1] = '\0'; //because I'm paranoid
                                          //-1 so that the new line char goes away
    int num_written= write(sock_fd, get_user_name, check_read);
    if (num_written != check_read) {
        perror("username not written");
        close(sock_fd);
        exit(1);
    }

    //Settin max_fd
    int max_fd;
    if(STDIN_FILENO >= sock_fd){
      max_fd = STDIN_FILENO;
    } else{
      max_fd = sock_fd;
    }

    fd_set all_fds, listen_fds, write_fds, copy_writefds;
    FD_ZERO(&all_fds);
    FD_ZERO(&write_fds);
    FD_SET(sock_fd, &all_fds);
    FD_SET(STDIN_FILENO, &all_fds);
    FD_SET(sock_fd, &write_fds);

    char buf[BUF_SIZE + 1];
    while (1) {
      listen_fds = all_fds;
      copy_writefds = write_fds;
      int nready = select(max_fd + 1, &listen_fds, &copy_writefds, NULL, NULL);
      if (nready == -1) {
          perror("server: select");
          exit(1);
      }

      if(FD_ISSET(STDIN_FILENO, &listen_fds)){

          int num_read = read(STDIN_FILENO, buf, BUF_SIZE);
          if (num_read == 0) {
              break;
            }
            if (nready == -1) {
                perror("client: select");
                exit(1);
            }
            buf[num_read] = '\0';         // Just because I'm paranoid
            if(FD_ISSET(sock_fd, &write_fds)){
            int num_written = write(sock_fd, buf, num_read);
            if (num_written != num_read) {
              perror("client: write");
              close(sock_fd);
              close(STDIN_FILENO);
              exit(1);
            }
          }
        }

        if(FD_ISSET(sock_fd, &listen_fds)){
        int num_read = read(sock_fd, buf, BUF_SIZE);
        if(num_read == 0){
          break;
        }
        buf[num_read] = '\0';
        printf("Received from server: %s", buf);

    }

}
    close(STDIN_FILENO);
    close(sock_fd);
    return 0;
}
