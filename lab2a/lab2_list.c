//
//  lab2_list.c
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
#include <string.h>
#include <signal.h>
#include "SortedList.c"

SortedList_t* shared_list;
SortedListElement_t** elements;

int iterations_num = 0;
int sync_num = 0;
int yield_flag_i = 0;
int yield_flag_d = 0;
int yield_flag_l = 0;
int opt_yield;

void signal_handler(){
    fprintf(stderr, "Segmentation fault.\n");
    exit(2);
}

void* thread_function(void* argument) {
    SortedListElement_t* t_elements = (SortedListElement_t*)argument;
    
    signal(SIGSEGV, signal_handler);
    
    pthread_mutex_t mutex;
    int spin_lock = 0;
    
    /* Inserting */
    for(int i=0; i<iterations_num; i++){
        if (sync_num == 1)
            pthread_mutex_lock(&mutex);
        else if (sync_num == 2)
            while(__sync_lock_test_and_set(&spin_lock, 1) == 1);
        
        SortedList_insert(shared_list, &t_elements[i]);
        
        if(sync_num == 1)
            pthread_mutex_unlock(&mutex);
        else if(sync_num == 2)
            __sync_lock_release(&spin_lock);
    }
    
    /* Getting length */
    if (sync_num == 1)
        pthread_mutex_lock(&mutex);
    else if (sync_num == 2)
        while(__sync_lock_test_and_set(&spin_lock, 1) == 1);
    
    SortedList_length(shared_list);
    
    if(sync_num == 1)
        pthread_mutex_unlock(&mutex);
    else if(sync_num == 2)
        __sync_lock_release(&spin_lock);
    
    /* Looking up and deleting */
    for(int i=0; i<iterations_num; i++){
        if (sync_num == 1)
            pthread_mutex_lock(&mutex);
        else if (sync_num == 2)
            while(__sync_lock_test_and_set(&spin_lock, 1) == 1);
        
        SortedListElement_t* temp = SortedList_lookup(shared_list, t_elements[i].key);
        if (SortedList_delete(temp) == 1) {
            fprintf(stderr, "Corrupted list!\n");
            exit(0);
        }
        
        if(sync_num == 1)
            pthread_mutex_unlock(&mutex);
        else if(sync_num == 2)
            __sync_lock_release(&spin_lock);
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
        {"yield", required_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };
    
    while (1) {
        opt = getopt_long(argc, (char**)argv, "y:t:i:s:", long_options, NULL);
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
                    sync_num = 1;
                else if (optarg[0] == 's')
                    sync_num = 2;
                else {
                    fprintf(stderr, "Invalide --sync= option.\n");
                    exit(1);
                }
                break;
            case 'y' :
                for (unsigned int i=0; i<strlen(optarg); i++) {
                    if (optarg[i] == 'i') {
                        opt_yield |= INSERT_YIELD;
                        yield_flag_i = 1;
                    }
                    else if (optarg[i] == 'd') {
                        opt_yield |= DELETE_YIELD;
                        yield_flag_d = 1;
                    }
                    else if (optarg[i] == 'l') {
                        opt_yield |= LOOKUP_YIELD;
                        yield_flag_l = 1;
                    }
                    else {
                        fprintf(stderr, "Unknown yield option.\n");
                        exit(1);
                    }
                }
                break;
        }
    }
    
    shared_list = (SortedList_t*)malloc(sizeof(SortedList_t));
    elements = (SortedListElement_t**)malloc(sizeof(SortedListElement_t*)*thread_num);
    
    /* Initializing all the elements */
    srand((int)time(NULL));
    for (int i=0; i<thread_num; i++) {
        elements[i] = (SortedListElement_t*)malloc(sizeof(SortedListElement_t)*iterations_num);
        
        for (int j=0; j<iterations_num; j++) {
            char* key = (char*)malloc(sizeof(char)*128);

            for (int k=0; k<128; k++) {
                key[k] = rand() % 128;
            }
            elements[i][j].key = key;
            elements[i][j].next = NULL;
            elements[i][j].prev = NULL;
        }
    }
    
    struct timespec start, end;
    char* test_type = (char*)malloc(sizeof(char)*18);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[thread_num];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 100000000);
    int i;
    int err;
    /* Creating threads */
    for (i = 0; i < thread_num; i++) {
        err = pthread_create(&threads[i], &attr, thread_function, elements[i]);
        if (err) {
            fprintf(stderr, "Error creating threads.\n");
            exit(1);
        }
    }
    //wait for all threads
    for (i = 0; i < thread_num; i++) {
        err = pthread_join(threads[i], NULL);
        if (err) {
            fprintf(stderr, "Error waiting all threads to complete.\n");
            exit(1);
        }
    }
    pthread_attr_destroy(&attr);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    if(SortedList_length(shared_list) != 0){
        fprintf(stderr, "The shared list is nonempty.\n");
        exit(0);
    }
    
    strcat(test_type, "list-");
    if (yield_flag_i == 1)
        strcat(test_type, "i");
    if (yield_flag_d == 1)
        strcat(test_type, "d");
    if (yield_flag_l == 1)
        strcat(test_type, "l");
    if (!yield_flag_i && !yield_flag_d && !yield_flag_l)
        strcat(test_type, "none");
    strcat(test_type, "-");
    if (sync_num == 0)
        strcat(test_type, "none");
    else if (sync_num == 1)
        strcat(test_type, "m");
    else
        strcat(test_type, "s");
    
    
    long operation_num = thread_num * iterations_num * 3;
    long runtime = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    long per_operation_time = runtime/operation_num;
    
    printf("%s,%d,%d,%d,%ld,%ld,%ld\n", test_type, thread_num, iterations_num, 1,
           operation_num, runtime, per_operation_time);
    
    exit(0);
}
