

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Read a user id and password from standard input,
   - create a new process to run the validate program
   - send 'validate' the user id and password on a pipe,
   - print a message
        "Password verified" if the user id and password matched,
        "Invalid password", or
        "No such user"
     depending on the return value of 'validate'.
*/


#define VERIFIED "Password verified\n"
#define BAD_USER "No such user\n"
#define BAD_PASSWORD "Invalid password\n"
#define OTHER "Error validating password\n"


int main(void) {
    char userid[10];
    char password[10];

    /* Read a user id and password from stdin */
    printf("User id:\n");
    scanf("%s", userid);
    printf("Password:\n");
    scanf("%s", password);

    int pipefd[2];
    int ret_fork;
    int ret_child;
    int status;
    if(pipe(pipefd) == -1){
        perror("Pipe exited with error");
    }else{
        ret_fork = fork();
    }

    if(ret_fork > 0){//parent
        close(pipefd[0]); //close read we won't be reading from pipe
        if (write(pipefd[1], userid, 10) == -1){
            perror("Error while writing userid");
        }
        if(write(pipefd[1], password, 10) == -1){
            perror("Error while writing password");
        }
        /*finished writing close pipe so reading can begin */
        close(pipefd[1]);
        /*wait for child to finish */
        if (wait(&status) != -1){
            if (WIFEXITED(status)) {
                ret_child = WEXITSTATUS(status);
                /*to print to stdout*/
                if(ret_child == 0){
                    printf("%s", VERIFIED);
                } if(ret_child == 1){
                    printf("%s", OTHER);
                } if(ret_child == 2){
                    printf("%s", BAD_PASSWORD );
                } if(ret_child == 3){
                    printf("%s", BAD_USER);
                }
            } else {
                printf("Child exited abnormally\n");
            }
        }
    }

    if(ret_fork == 0){//child
        close(pipefd[1]); //close write child will not write to pipe

        /*reset stdin so it reads from pipe */
        if((dup2(pipefd[0], fileno(stdin))) == -1){
            perror("dup2");
            exit(-1);
        }

        /*since we reset stdin to read from pipe we don't need pipefd[0]*/
        close(pipefd[0]);
        execl("validate", "validate", (char *) 0);
    }
    return 0;
}
