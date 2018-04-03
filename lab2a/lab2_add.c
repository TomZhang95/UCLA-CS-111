//
//  lab2_add.c
//  lab2a
//
//  Created by Tom Zhang on 10/26/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>

long long counter;
int iterations_num;
int yield_flag;
int calling_num;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}

void add_m (long long *pointer, long long value) {
    pthread_mutex_t lock;
    if (pthread_mutex_lock(&lock)) {
        fprintf(stderr, "Error in pthread_mutex_lock()\n");
        exit(2);
    }
    long long sum = *pointer + value;
    if (yield_flag)
        sched_yield();
    *pointer = sum;
    if (pthread_mutex_unlock(&lock)){
        fprintf(stderr, "Error in pthread_mutex_unlock()\n");
        exit(2);
    }
}

void add_s(long long *pointer, long long value){
    int lock;
    while(__sync_lock_test_and_set(&lock, 1));
    long long sum = *pointer + value;
    if (yield_flag)
        sched_yield();
    *pointer = sum;
    __sync_lock_release(&lock);
}

void add_c(long long *pointer, long long value) {
    long long new, old;
    if (yield_flag)
        sched_yield();
    do {
        old = *pointer;
        new = old + value;
        if (yield_flag)
            sched_yield();
    } while(__sync_val_compare_and_swap(pointer, old, new) != old);
}

void add_wrapper(long long *pointer, long long value) {
    switch (calling_num) {
        case 0: add(pointer, value);
            break;
        case 1: add_m(pointer, value);
            break;
        case 2: add_s(pointer, value);
            break;
        case 3: add_c(pointer, value);
            break;
        default: fprintf(stderr, "Invalid --sync= option.\n");
    }
}

void* thread_function () {
    for (int i=0; i<iterations_num; i++) {
        add_wrapper(&counter, 1);
        add_wrapper(&counter, -1);
    }
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    int opt = 0;
    int thread_num = 0;
    
    const struct option long_options[] =
    {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", no_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };
    
    iterations_num = 0;
    calling_num = 0;
    yield_flag = 0;
    
    
    while (1) {
        opt = getopt_long(argc, (char**)argv, "yt:i:s:", long_options, NULL);
        if (opt == -1)
            break;
        if (opt == '?') {
            fprintf(stderr, "Valid usage: ./lab2_add --iterations= --threads= --sync= --yield\n");
            exit(1);
        }
        
        switch(opt) {
            case 't' :
                thread_num = atoi(optarg);
                if (thread_num < 1){
                    fprintf(stderr, "Thread number too small.\n");
                    exit(1);
                }
                break;
            case 'i' :
                iterations_num = atoi(optarg);
                if (iterations_num < 1){
                    fprintf(stderr, "Iterations number too small.\n");
                    exit(1);
                }
                break;
            case 's' :
                if (optarg[0] == 'm')
                    calling_num = 1;
                else if (optarg[0] == 's')
                    calling_num = 2;
                else if (optarg[0] == 'c')
                    calling_num = 3;
                break;
            case 'y' :
                yield_flag = 1;
                break;
        }
    }
    struct timespec start,end;
    counter = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[thread_num];
    int i;
    int err;
    /* Creating threads */
    for (i = 0; i < thread_num; i++) {
        err = pthread_create(&threads[i], NULL, thread_function, NULL);
        if (err) {
            fprintf(stderr, "Error creating threads.\n");
            exit(2);
        }
    }
    //wait for all threads
    for (i = 0; i < thread_num; i++) {
        err = pthread_join(threads[i], NULL);
        if (err) {
            fprintf(stderr, "Error waiting all threads to complete.\n");
            exit(2);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long runtime = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    int operations = thread_num * iterations_num * 2;
    long ns_per_op = runtime / operations;
    char yield[2][10] = {"", "-yield"};
    char name[4][10] = {"-none", "-m", "-s", "-c"};
    printf("add%s%s,%d,%d,%d,%ld,%ld,%lld\n",
           yield[yield_flag], name[calling_num], thread_num,
           iterations_num, operations, runtime, ns_per_op, counter);
    
    exit(0);
}
