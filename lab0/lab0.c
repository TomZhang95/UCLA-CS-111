//
//  main.c
//  lab0
//
//  Created by Tom Zhang on 10/1/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void signal_handler_seg(int signum) {
    fprintf(stderr, "Catch signal %d\n", signum);
    exit(4);
}

int main(int argc, const char * argv[]) {
    int ifd;
    int ofd;
    int segfault_triger = 0;
    
    //struct for getopt_long(3)
    const struct option long_options[] = {
        {"input", required_argument, NULL, 1},
        {"output", required_argument, NULL, 2},
        {"segfault", no_argument, NULL, 3},
        {"catch", no_argument, NULL, 4},
        {0, 0, 0, 0}
    };
    
    while(1) {
        //use getopt_long to check both short and long options
        int option;
        option = getopt_long(argc, (char**)argv, "i::o::sc", long_options, NULL);
        if (option == -1) //if error occure or no more option when getting option, break
            break;
        
        switch(option) {
            case 1: //case of --input=
                if (!optarg) {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Argument missing.");
                    exit(2);
                }
                ifd = open(optarg, O_RDONLY);
                if (ifd >= 0) {
                    close(0);
                    dup(ifd);
                    close(ifd);
                }
                else {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error when read from file %s\n", optarg);
                    exit(2);
                }
                break;
            case 2: //case of --output=
                if (!optarg) {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Argument missing.");
                    exit(2);
                }
                ofd = creat(optarg, 0666);
                if (ofd >= 0) {
                    close(1);
                    dup(ofd);
                    close(ofd);
                }
                else {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error when create file %s\n", optarg);
                    exit(3);
                }
                break;
            case 3: //case of --segfault
                segfault_triger = 1;
                break;
            case 4:
                signal(SIGSEGV, signal_handler_seg);
                break;
            default:
                fprintf(stderr, "Unknown argument.\nCorrect usage: --input=filename, --output=filename, --segfault, --catch\n");
                exit(1);
                break;
        }
    }
    
    if (segfault_triger == 1) {
        int* seg = NULL;
        *seg = 1;
    }
    
    char* buffer[1024];
    int count = 0;
    int w = 0;
    while (1) {
        count = read(0, &buffer, 1024);
        if (count < 0) {
            fprintf(stderr, "Error when reading the file");
            exit(2);
        }
        if (count == 0)
            break;
        w = write(1, &buffer, count);
        if (w < 0) {
            fprintf(stderr, "Error when openning the file");
            exit(3);
        }
    }
    exit(0);
}
