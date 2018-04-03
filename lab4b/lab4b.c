//
//  main.c
//  lab4b
//
//  Created by Tom Zhang on 11/11/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include "mraa.h"
#include "mraa/aio.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <poll.h>

struct timespec begin, end;
int total_time = 0;
mraa_aio_context temp_sensor;
mraa_gpio_context button;
int log_on = 0;
int log_fd;
int count_start = 1;
int period = 1;
char scale = 'F';
int stop = 0;

void click_button() {
    time_t t = time(NULL);
    struct tm time_t = *localtime(&t);
    int hr = time_t.tm_hour;
    int min = time_t.tm_min;
    int sec = time_t.tm_sec;
    if (log_on){
        dprintf(log_fd, "%02d:%02d:%02d SHUTDOWN\n", hr, min, sec);
    }
    dprintf(STDOUT_FILENO, "%02d:%02d:%02d SHUTDOWN\n", hr, min, sec); // PRINT THE REPORT TO STDOUT
    
    mraa_aio_close(temp_sensor);
    mraa_gpio_close(button);
    exit(0);
    return;
}

void check_button() {
    int button_state;
    button_state = mraa_gpio_read(button);
    //button is on
    if (button_state == 1)
        click_button();
    else if (button_state < 0){
        fprintf(stderr, "Button Error.\n");
        mraa_aio_close(temp_sensor);
        mraa_gpio_close(button);
        exit(1);
    }
}

void get_temp() {
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long runtime = (end.tv_sec - begin.tv_sec) + total_time;
    
    if ((count_start || runtime > 0) && (runtime % period) == 0) {
        //Convert to degree C, demo code from  Grove
        const int B = 4275;               // B value of the thermistor
        const int R0 = 100000;            // R0 = 100k
        int a = mraa_aio_read(temp_sensor);
        double R = 1023.0/a-1.0;
        R = R0*R;
        double temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
        
        int hr;
        int min;
        int sec;
        
        time_t t = time(NULL);
        struct tm time_t = *localtime(&t);
        hr = time_t.tm_hour;
        min = time_t.tm_min;
        sec = time_t.tm_sec;
        if (scale == 'F')
            temperature = temperature * 1.8 + 32;
        if (log_on){
            dprintf(log_fd, "%02d:%02d:%02d %04.1f\n", hr, min, sec, temperature);
        }
        dprintf(STDOUT_FILENO, "%02d:%02d:%02d %04.1f\n", hr, min, sec, temperature);
        
        //start counting again
        clock_gettime(CLOCK_MONOTONIC, &begin);
        total_time = 0;
        count_start = 0;
    }
    return;
}

int main(int argc, const char * argv[]) {
    const struct option long_options[] = {
        {"period", required_argument, NULL, 'p'},
        {"scale", required_argument, NULL, 's'},
        {"log", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while(1) {
        opt = getopt_long(argc, (char* const*)argv, "p:s:l:", long_options, NULL);
        if (opt == -1)
            break;
        if (opt == '?'){
            fprintf(stderr, "Valid flag usage:\n./lab4b --period=\n--scale=\n--log=FILENAME\n");
            exit(1);
        }
        switch(opt) {
            case 'p' :
                period = atoi(optarg);
                if (period <= 0){
                    fprintf(stderr, "Enter a larger period number.\n");
                    exit(1);
                }
                break;
            case 's' :
                if (optarg[0] == 'C')
                    scale = 'C';
                else if (optarg[0] == 'F')
                    scale = 'F';
                else{
                    fprintf(stderr, "Valid option: C／F for scale.\n");
                    exit(1);
                }
                break;
            case 'l':
                log_on = 1;
                log_fd = creat((char*)optarg, 00666);
                if (log_fd <= 0){
                    fprintf(stderr, "Error when open log file: %s\n", (char*)optarg);
                    exit(1);
                }
                break;
            case '?': fprintf(stderr, "Valid flag usage:\n./lab4b --period=\n--scale=\n--log=FILENAME\n");
                exit(1);
                break;
        }
    }
    
    //initialize
    temp_sensor = mraa_aio_init(1);
    button = mraa_gpio_init(62);
    mraa_gpio_dir(button, MRAA_GPIO_IN);  // not 0
    
    struct pollfd poll_fd[1];
    poll_fd[0].fd = 0;
    poll_fd[0].events = POLLIN;
    
    char read_buffer[64];
    
    while(1){
        int rt = poll(poll_fd, 1, 0);
        if (rt == -1) {  // error
            fprintf(stderr, "poll error detected.\n");
            mraa_aio_close(temp_sensor);
            mraa_gpio_close(button);
            exit(1);
        }
        else if(rt >= 0){
            check_button();
            //read temperature
            if(stop == 0){
                get_temp();
            }
            if (rt > 0) {
                //check input from stdin
                //if hanging
                if(poll_fd[0].revents & POLLHUP) {
                    break;
                }
                //if error occur
                if(poll_fd[0].revents & POLLERR) {
                    fprintf(stderr, "Stdin reading error.\n");
                    mraa_aio_close(temp_sensor);
                    mraa_gpio_close(button);
                    exit(1);
                }
                //if readble
                if(poll_fd[0].revents & POLLIN) {
                    bzero(read_buffer, 64); // empty input buffer
                    fgets(read_buffer, 64, stdin);
                    if (strcmp(read_buffer, "OFF\n") == 0){
                        if (log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                        click_button();
                    }
                    //Start command
                    else if (strcmp(read_buffer, "START\n") == 0){
                        if (log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                        stop = 0;
                        clock_gettime(CLOCK_MONOTONIC, &begin);
                    }
                    //Stop command
                    else if (strcmp(read_buffer, "STOP\n") == 0) {
                        if(log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                        stop = 1;
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        total_time = end.tv_sec - begin.tv_sec;
                    }
                    //Period command
                    else if (strncmp(read_buffer, "PERIOD=",7) == 0){
                        period = atoi(read_buffer + 7);
                        if (period <= 0){
                            fprintf(stderr, "Period has to be positive integer, exit.\n");
                            mraa_aio_close(temp_sensor);
                            mraa_gpio_close(button);
                            exit(1);
                        }
                        if (log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                    }
                    //Scale command
                    else if (strcmp(read_buffer, "SCALE=F\n") == 0){
                        if (log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                        scale = 'F';
                    }
                    else if (strcmp(read_buffer, "SCALE=C\n") == 0){
                        if (log_on){
                            dprintf(log_fd, "%s", read_buffer);
                        }
                        scale = 'C';
                    }
                }
            }
        }
    }
    mraa_aio_close(temp_sensor);
    mraa_gpio_close(button);
    exit(0);

}
