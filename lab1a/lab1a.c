//
//  main.c
//  lab1a
//
//  Created by Tom Zhang on 10/7/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/termios.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_SIZE 1
#define BUFFER_SIZE 1024

void shell_option(struct termios original_tattr, char special_case[]) {
    /* Pipe process */
    int to_child_fd[2];
    int from_child_fd[2];
    /* From terminal to shell */
    if (pipe(to_child_fd) == -1) {
        fprintf(stderr, "Pipe constructing error");
        exit(1);
    }
    /* From shell to terminal */
    if (pipe(from_child_fd) == -1) {
        fprintf(stderr, "Pipe constructing error");
        exit(1);
    }
    
    pid_t child_pid, parent_pid;
    parent_pid = getpid();
    child_pid = fork();
    if (child_pid < 0) {
        fprintf(stderr, "Fork failed.");
        exit(1);
    }
    else if (child_pid == 0) {
        /* Child process */
        close(to_child_fd[1]); //close write to shell
        close(from_child_fd[0]); //close read from shell
        dup2(to_child_fd[0],STDIN_FILENO);
        dup2(from_child_fd[1],STDOUT_FILENO);
        dup2(from_child_fd[1],STDERR_FILENO);
        
        char * execvp_argv[2];
        char *file = "/bin/bash";
        execvp_argv[0] = file;
        execvp_argv[1] = NULL;
        if (execvp(file, execvp_argv) == -1) {
            fprintf(stderr, "Execute shell error. \n");
            kill(parent_pid, SIGKILL);
            exit(1);
        }
    }
    else {
        /* Parent process */
        struct pollfd poll_fd[2];
        
        /* From keyboard */
        poll_fd[0].fd = STDIN_FILENO;
        poll_fd[0].events = POLLIN;
        
        /* From shell */
        poll_fd[1].fd = from_child_fd[0];
        poll_fd[1].events = POLLIN;;
        
        close(to_child_fd[0]);
        close(from_child_fd[1]);
        
        while(1) {
            if (poll(poll_fd, 2, 0) == -1) {
                printf("Error on polling: %s\n", strerror(errno));
                kill(child_pid, SIGKILL);
                exit(1);
            }
            
            /* Check input from keyboard */
            if (poll_fd[0].revents & (POLLHUP + POLLERR)) {
                fprintf(stderr, "Exit with keyboard error.");
                tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                break;
            }
            
            /* Check pipe from shell */
            if (poll_fd[1].revents & (POLLHUP + POLLERR)) {
                close(to_child_fd[1]);
                close(from_child_fd[0]);
                
                int status;
                pid_t closed_pid = waitpid(child_pid, &status, WNOHANG);
                if (closed_pid == -1) {
                    fprintf(stderr, "Error occur when waiting child process end.\n");
                    exit(1);
                }
                else if (closed_pid == child_pid) {
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                    if (WIFSIGNALED(status)) {
                        fprintf(stdout, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                    }
                    else {
                        fprintf(stdout, "SHELL EXIT SIGNAL=N/A STATUS=%d\n", WEXITSTATUS(status));
                    }
                    exit(0);
                }
            }
            
            /* Available to read from keyboard */
            if (poll_fd[0].revents & POLLIN) {
                char read_buffer[BUFFER_SIZE];
                read(STDIN_FILENO, read_buffer, READ_SIZE);
                if (read_buffer[0] == '\r' || read_buffer[0] == '\n') {
                    write(STDOUT_FILENO, special_case, 2);
                    write(to_child_fd[1], "\n", 1);
                }
                else if (read_buffer[0] == 4) {
                    close(to_child_fd[1]);
                }
                else if (read_buffer[0] == 3)
                    kill(child_pid, SIGINT);
                else {
                    write(STDOUT_FILENO, read_buffer, 1);
                    write(to_child_fd[1], read_buffer, 1);
                }
                
            }
            
            /* Available to read from shell */
            if (poll_fd[1].revents & POLLIN) {
                char buffer[BUFFER_SIZE];
                ssize_t readin = read(from_child_fd[0], buffer, READ_SIZE);
                for (ssize_t i=0; i<readin; i++) {
                    if (buffer[i] == '\r' || buffer[i] == '\n')
                        write(STDOUT_FILENO, special_case, 2);
                    else
                        write(STDOUT_FILENO, buffer, 1);
                }
            }
            
        }
    }
}



int main(int argc, const char * argv[]) {
    int opt = 0;
    int shell_active;
    char special_case[2] = {'\r', '\n'};
    struct option longopts[] = {
        {"shell", no_argument, &shell_active, 's'},
        {0,0,0,0}
    };
    
    /* get/set new terminal attributes and test if set/get success */
    struct termios original_tattr, new_tattr;
    /* Make sure stdin is a terminal. */
    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
    }
    if (tcgetattr(STDIN_FILENO, &original_tattr) == -1) {
        fprintf(stderr, "Tcgetattr error.");
        exit(1);
    }
    new_tattr = original_tattr;
    new_tattr.c_iflag = ISTRIP;	/* only lower 7 bits	*/
    new_tattr.c_oflag = 0;		/* no processing	*/
    new_tattr.c_lflag = 0;		/* no processing	*/
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tattr) == -1) {
        fprintf(stderr, "Tcsetattr error.");
        exit(1);
    }
    else {
        struct termios tmp;
        tcgetattr(0, &tmp);
        /* Check if current terminal attributes changed correctly */
        if (tmp.c_iflag != ISTRIP || tmp.c_oflag != 0 || tmp.c_lflag != 0) {
            fprintf(stderr, "Terminal attributes setting unsuccessful.");
            exit(1);
        }
    }
    
    /* Set value to shell_active */
    while(1) {
        opt = getopt_long (argc, (char**)argv, "s", longopts, NULL);
        if (opt == -1)
            break;
        else if (opt == '?') {
            fprintf(stderr, "Valid argument: --shell\n");
            exit(1);
        }
    };
    
    if (shell_active == 's')
        shell_option(original_tattr, special_case);
    else {
        while (1) {
            char buffer[BUFFER_SIZE];
            ssize_t readin = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            for (ssize_t i=0; i<readin; i++) {
                if (buffer[i] == '\r' || buffer[i] == '\n')
                    write(STDOUT_FILENO, special_case, 2);
                else if (buffer[i] == 4) {
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                    exit(0);
                }
                else
                    write(STDOUT_FILENO, buffer, 1);
            }
        }
    }
    
    exit(0);
}
