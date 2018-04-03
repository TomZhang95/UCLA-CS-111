//
//  lab1b-client.c
//  lab1b
//
//  Created by Tom Zhang on 10/11/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <getopt.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h>
#include <mcrypt.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    int opt;
    int port = -1;
    int log_flag = 0;
    int encrypt = 0;
    int key_size = 16; //16 chars are 128 bits
    
    char* key = malloc(17*sizeof(char));
    char* log_file;
    char* encrypt_file;
    
    struct option longopts[] = {
        {"port", required_argument, NULL, 'p'},
        {"log", required_argument, NULL, 'l'},
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
        else if (opt == 'l') {
            log_flag = 1;
            log_file = optarg;
        }
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
    
    FILE* log_fd;
    if (log_flag) {
        log_fd = fopen(log_file, "w");
        if (log_fd == NULL) {
            fprintf(stderr, "Error when open log file: %s\n", log_file);
            exit(1);
        }
    }
    
    /* Creating TCP socket with IPv4 Internet protocal */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "Socket creation failed, exit with code 1.\n");
        exit(1);
    }
    
    /* Getting server information */
    struct hostent* server;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server = gethostbyname("localhost");
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
    
    /* Connect to server */
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Connection failed to establish.\n");
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
    
    struct pollfd poll_fd[2];
    
    /* From keyboard */
    poll_fd[0].fd = STDIN_FILENO;
    poll_fd[0].events = POLLIN;
    
    /* From shell */
    poll_fd[1].fd = socket_fd;
    poll_fd[1].events = POLLIN;;
    

    while(1) {
        if (poll(poll_fd, 2, 0) == -1) {
            printf("Error on polling: %s\n", strerror(errno));
            exit(1);
        }
        
        /* Check input from keyboard */
        if (poll_fd[0].revents & (POLLHUP + POLLERR)) {
            write(socket_fd, "\x04", 1);
            if (encrypt) {
                mcrypt_generic_deinit(encrypt_td);
                mcrypt_module_close(encrypt_td);
                mcrypt_generic_deinit(decrypt_td);
                mcrypt_module_close(decrypt_td);
            }
            fprintf(stderr, "Exit with keyboard error.");
            tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
            exit(1);
        }
        
        /* Check input from server */
        if (poll_fd[1].revents & (POLLHUP + POLLERR)) {
            if (encrypt) {
                mcrypt_generic_deinit(encrypt_td);
                mcrypt_module_close(encrypt_td);
                mcrypt_generic_deinit(decrypt_td);
                mcrypt_module_close(decrypt_td);
            }
            fprintf(stderr, "Exit because of server closed.");
            tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
            exit(0);
        }
        
        /* Process data recieved from keyboard, sending 
         to STDOUT and server */
        if (poll_fd[0].revents & POLLIN) {
            char read_buffer[256];
            int read_in = read(STDIN_FILENO, read_buffer, 1);
            
            if (read_in < 0) {
                char sign_EOT[1] = "\x04";
                if (encrypt)
                    mcrypt_generic(encrypt_td, sign_EOT, 1);
                write(socket_fd, sign_EOT, 1);
                if (encrypt) {
                    mcrypt_generic_deinit(encrypt_td);
                    mcrypt_module_close(encrypt_td);
                    mcrypt_generic_deinit(decrypt_td);
                    mcrypt_module_close(decrypt_td);
                }
                fprintf(stderr, "Exit with keyboard reading error.");
                tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                exit(1);

            }
            
            //for (int i=0; i<read_in; i++) {
                char newline[1] = "\n";
                
                if (read_buffer[0] == '\r' || read_buffer[0] =='\n') {
                    write(STDOUT_FILENO, "\r\n", 2);
                    if (encrypt)
                        mcrypt_generic(encrypt_td, newline, 1);
                    write(socket_fd, newline, 1);
                }
                else if (read_buffer[0] == 4) {
                    char sign_EOT[1] = "\x04";
                    if (encrypt) {
                        mcrypt_generic(encrypt_td, sign_EOT, 1);
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    write(socket_fd, sign_EOT, 1);
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                    exit(0);
                }
                else if (read_buffer[0] == 3) {
                    char sign_ETX[1] = "\x03";
                    if (encrypt) {
                        mcrypt_generic(encrypt_td, sign_ETX, 1);
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    write(socket_fd, sign_ETX, 1);
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                    exit(0);
                }
                else {
                    write(STDOUT_FILENO, &read_buffer[0], 1);
                    if (encrypt)
                        mcrypt_generic(encrypt_td, &read_buffer[0], 1);
                    write(socket_fd, &read_buffer[0], 1);
                }
            if (log_flag) {
                char log_message[] = "SENT 1 byte: ";
                strcat(log_message, read_buffer);
                strcat(log_message, "\n");
                write(fileno(log_fd), log_message, strlen(log_message));
            }
            //}
            
        }
        
        /* Process received data from server, print to STDOUT */
        if (poll_fd[1].revents & POLLIN) {
            char read_buffer[256];
            int read_in = read(socket_fd, read_buffer, 1);
            
            if (read_in < 0) {
                if (encrypt) {
                    mcrypt_generic_deinit(encrypt_td);
                    mcrypt_module_close(encrypt_td);
                    mcrypt_generic_deinit(decrypt_td);
                    mcrypt_module_close(decrypt_td);
                }
                fprintf(stdout, "Exit by server closed.");
                tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
                exit(0);
            }
            
            //for (int i=0; i<read_in; i++) {
            if (log_flag) {
                char log_message[] = "RECEIVED 1 byte: ";
                strcat(log_message, read_buffer);
                strcat(log_message, "\n");
                write(fileno(log_fd), log_message, strlen(log_message));
            }
                if (encrypt)
                    mdecrypt_generic(encrypt_td, read_buffer, 1);
                if (read_buffer[0] == '\r' || read_buffer[0] =='\n') {
                    write(STDOUT_FILENO, "\r\n", 2);
                }
                else {
                    write(STDOUT_FILENO, read_buffer, 1);
                }
            //}
            
        }

    }
    
    if (encrypt) {
        mcrypt_generic_deinit(encrypt_td);
        mcrypt_module_close(encrypt_td);
        mcrypt_generic_deinit(decrypt_td);
        mcrypt_module_close(decrypt_td);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tattr);
    exit(0);
}
