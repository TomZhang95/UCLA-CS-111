//
//  lab1b-server.c
//  lab1b
//
//  Created by Tom Zhang on 10/15/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <getopt.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <mcrypt.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <netinet/in.h>


int main(int argc, const char * argv[]) {
    int opt;
    int port = -1;
    int encrypt = 0;
    int key_size = 16; //16 chars are 128 bits
    
    char* key = malloc(17*sizeof(char));
    char* encrypt_file;
    
    struct option longopts[] = {
        {"port", required_argument, NULL, 'p'},
        {"encrypt", required_argument, NULL, 'e'},
        {0,0,0,0}
    };
    
    /* Processing options */
    while(1) {
        opt = getopt_long (argc, (char**)argv, "p", longopts, NULL);
        if (opt == -1)
            break;
        if (opt == '?') {
            fprintf(stderr, "Valid argument: --port\n");
            exit(1);
        }
        if (opt == 'p')
            port = atoi(optarg);
        else if (opt == 'e') {
            encrypt = 1;
            encrypt_file = optarg;
            FILE* file;
            file = fopen(encrypt_file, "r");
            if (file == NULL) {
                ssize_t read_in = read(fileno(file), key, key_size);
                if (read_in < 0) {
                    fprintf(stderr, "Error reading key file.\n");
                    exit(1);
                }
                close(fileno(file));
            }
        }
    }
    
    if (port == -1) {
        fprintf(stderr, "Mandatory option --port= must provide!\n");
        exit(1);
    }
    
    MCRYPT encrypt_td;
    MCRYPT decrypt_td;
    char *encrypt_IV;
    char *decrypt_IV;
    if (encrypt) {
        /* Sample code from mcrypt Linux Man page */
        encrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        decrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        
        if (encrypt_td==MCRYPT_FAILED) {
            return 1;
        }
        if (decrypt_td==MCRYPT_FAILED) {
            return 1;
        }
        
        encrypt_IV = malloc(mcrypt_enc_get_iv_size(encrypt_td));
        decrypt_IV = malloc(mcrypt_enc_get_iv_size(decrypt_td));
        
        /* Put random data in IV. */
        int i;
        for (i=0; i< mcrypt_enc_get_iv_size(encrypt_td); i++) {
            encrypt_IV[i]=rand();
        }
        i=mcrypt_generic_init(encrypt_td, key, key_size, encrypt_IV);
        if (i<0) {
            mcrypt_perror(i);
            return 1;
        }
        
        int j;
        for (j=0; i< mcrypt_enc_get_iv_size(decrypt_td); i++) {
            decrypt_IV[i]=rand();
        }
        j=mcrypt_generic_init(decrypt_td, key, key_size, decrypt_IV);
        if (j<0) {
            mcrypt_perror(j);
            return 1;
        }
    }

    
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
        
        /* Creating listen socket with IPv4 Internet protocal */
        int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_socket < 0) {
            fprintf(stderr, "Socket creation failed, exit with code 1.\n");
            exit(1);
        }
        
        /* Getting server information */
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        /* Binding */
        if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            fprintf(stderr, "Failed binding server to port %d.\n", port);
            kill(child_pid, SIGINT);
            exit(1);
        }
        
        /* listening request, 5 default queueing limit */
        listen(listen_socket, 2);

        /* Accept request */
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int accept_socket = accept(listen_socket, (struct sockaddr*) &client_addr, &client_addr_len);
        if (accept_socket < 0){
            fprintf(stderr, "Error when accepting %d.\n", port);
            kill(child_pid, SIGINT);
            exit(1);
        }
        else
            close(listen_socket);
        
        /* Polling */
        struct pollfd poll_fd[2];
        
        /* From client */
        poll_fd[0].fd = accept_socket;
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
            
            /* Check from client */
            if (poll_fd[0].revents & (POLLHUP + POLLERR)) {
                fprintf(stderr, "Exit from client error");
                close(accept_socket);
                kill(child_pid, SIGINT);
                exit(1);
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
                    if (WIFSIGNALED(status)) {
                        fprintf(stdout, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                    }
                    else {
                        fprintf(stdout, "SHELL EXIT SIGNAL=N/A STATUS=%d\n", WEXITSTATUS(status));
                    }
                    if (encrypt) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    exit(0);
                }
            }
            
            /* Available to read from client */
            if (poll_fd[0].revents & POLLIN) {
                char read_buffer[256];
                int read_in = read(accept_socket, read_buffer, 1);
                if (read_in < 0) {
                    if (encrypt) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    fprintf(stderr, "Exit with client reading error.");
                    exit(1);
                }
                
                if (encrypt) {
                    mdecrypt_generic(encrypt_td, read_buffer, 1);
                }
                if (read_buffer[0] == 4) {
                    close(to_child_fd[1]);
                    close(accept_socket);
                    if (encrypt) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    exit(0);
                }
                else if (read_buffer[0] == 3) {
                    kill(child_pid, SIGINT);
                    close(accept_socket);
                }
                else {
                    write(to_child_fd[1], read_buffer, 1);
                }
            }
            
            /* Available to read from shell */
            if (poll_fd[1].revents & POLLIN) {
                char read_buffer[256];
                int read_in = read(from_child_fd[0], read_buffer, 256);
                if (read_in < 0) {
                    if (encrypt) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    fprintf(stderr, "Exit with client reading error.");
                    exit(1);
                }
                for (int i=0; i<read_in; i++) {
                    if (read_buffer[0] == 4) {
                        close(accept_socket);
                        if (encrypt) {
                            mcrypt_generic_deinit(encrypt_td);
                            mcrypt_module_close(encrypt_td);
                            mcrypt_generic_deinit(decrypt_td);
                            mcrypt_module_close(decrypt_td);
                        }
                        exit(0);
                    }
                    if (encrypt)
                        mcrypt_generic(encrypt_td, &read_buffer[i], 1);
                    write(accept_socket, &read_buffer[i], 1);
                }
            }

        }

    }
    if (encrypt) {
        mcrypt_generic_deinit(encrypt_td);
        mcrypt_module_close(encrypt_td);
        mcrypt_generic_deinit(decrypt_td);
        mcrypt_module_close(decrypt_td);
    }
    exit(0);
}
