//
//  lab2_list.c
//  lab2b
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
#include "SortedList.h"

int lock_num = 1;
int opt_yield;

struct sublist{
    SortedList_t* m_list;
    pthread_mutex_t mutex_lock;
    volatile int spin_lock;
};

struct thread_data {
    struct sublist** sublist;
    SortedListElement_t** elements;
    int list_num;
    long long wait_time;
    char sync_type;
    int iter_num;
};

int hash(const char *key) {
    int val = 5381;
    val = ((val << 5) + val) + key[0];
    return val;
}

void signal_handler(){
    fprintf(stderr, "Segmentation fault.\n");
    exit(2);
}

void* thread_function(void* argument) {
    int sublist_num = 0;
    struct timespec wait_start;
    struct timespec wait_end;
    struct thread_data* list_elements = (struct thread_data*)argument;
    long long *wait_time = &list_elements->wait_time;
    int iterations_num = list_elements->iter_num;
    int sync_type = list_elements->sync_type;
    
    signal(SIGSEGV, signal_handler);
    
    //pthread_mutex_t mutex = list_elements->m_sublist[sublist_num]->mutex_lock;
    //int spin_lock = list_elements->m_sublist[sublist_num]->spin_lock;
    
    /* Inserting */
    for(int i=0; i<iterations_num; i++){
        SortedListElement_t* temp = list_elements->elements[i];
        const char *key = temp->key;
        sublist_num = hash(key) % list_elements->list_num;
        
        clock_gettime(CLOCK_MONOTONIC, &wait_start);
        if (sync_type == 'm')
            pthread_mutex_lock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if (sync_type == 's')
            while(__sync_lock_test_and_set(&list_elements->sublist[sublist_num]->spin_lock, 1) == 1);
        lock_num++;
        clock_gettime(CLOCK_MONOTONIC, &wait_end);
        *wait_time = *wait_time + (wait_end.tv_sec - wait_start.tv_sec) * 1000000000 + (wait_end.tv_nsec - wait_start.tv_nsec);
        
        SortedList_insert(list_elements->sublist[sublist_num]->m_list, temp);
        
        if(sync_type == 'm')
            pthread_mutex_unlock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if(sync_type == 's')
            __sync_lock_release(&list_elements->sublist[sublist_num]->spin_lock);
    }
    
    /* Getting length */
    for (int i=0; i<list_elements->list_num; i++) {
        //mutex = list_elements->m_sublist[sublist_num]->mutex_lock;
        //spin_lock = list_elements->m_sublist[sublist_num]->spin_lock;
        
        clock_gettime(CLOCK_MONOTONIC, &wait_start);
        if (sync_type == 'm')
            pthread_mutex_lock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if (sync_type == 's')
            while(__sync_lock_test_and_set(&list_elements->sublist[sublist_num]->spin_lock, 1) == 1);
        lock_num++;
        clock_gettime(CLOCK_MONOTONIC, &wait_end);
        *wait_time = *wait_time + (wait_end.tv_sec - wait_start.tv_sec) * 1000000000 + (wait_end.tv_nsec - wait_start.tv_nsec);
        
        SortedList_length(list_elements->sublist[i]->m_list);
        
        if(sync_type == 'm')
            pthread_mutex_unlock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if(sync_type == 's')
            __sync_lock_release(&list_elements->sublist[sublist_num]->spin_lock);
    }
    
    /* Looking up and deleting */
    for(int i=0; i<iterations_num; i++){
        const char *key = list_elements->elements[i]->key;
        sublist_num = hash(key) % list_elements->list_num;
        //mutex = list_elements->m_sublist[sublist_num]->mutex_lock;
        //spin_lock = list_elements->m_sublist[sublist_num]->spin_lock;
        
        clock_gettime(CLOCK_MONOTONIC, &wait_start);
        if (sync_type == 'm')
            pthread_mutex_lock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if (sync_type == 's')
            while(__sync_lock_test_and_set(&list_elements->sublist[sublist_num]->spin_lock, 1) == 1);
        lock_num++;
        clock_gettime(CLOCK_MONOTONIC, &wait_end);
        *wait_time = *wait_time + (wait_end.tv_sec - wait_start.tv_sec) * 1000000000 + (wait_end.tv_nsec - wait_start.tv_nsec);
        
        SortedListElement_t* temp = SortedList_lookup(list_elements->sublist[sublist_num]->m_list, list_elements->elements[i]->key);
        int error = SortedList_delete(temp) == 1;
        if (error == 1) {
            fprintf(stderr, "Corrupted list!\n");
            exit(0);
        }
        
        if(sync_type == 'm')
            pthread_mutex_unlock(&list_elements->sublist[sublist_num]->mutex_lock);
        else if(sync_type == 's')
            __sync_lock_release(&list_elements->sublist[sublist_num]->spin_lock);
    }
    
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    int opt = 0;
    int thread_num = 0;
    int iterations_num = 0;
    char sync_type = ' ';
    int list_num = 1;
    int yield_flag_i = 0;
    int yield_flag_d = 0;
    int yield_flag_l = 0;
    
    
    
    const struct option long_options[] =
    {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", required_argument, NULL, 'y'},
        {"lists", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };
    
    while (1) {
        opt = getopt_long(argc, (char**)argv, "y:t:i:s:l:", long_options, NULL);
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
                    sync_type = 'm';
                else if (optarg[0] == 's')
                    sync_type = 's';
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
            case 'l':
                list_num = atoi(optarg);
                if (list_num < 1){
                    fprintf(stderr, "Sublist number too small.");
                    exit(1);
                }
                break;
        }
    }
    
    struct thread_data** threads_args = (struct thread_data**)malloc(sizeof(struct thread_data*)*thread_num);
    for (int i=0; i<thread_num; i++)
        threads_args[i] = (struct thread_data*)malloc(sizeof(struct thread_data));
    
    /* Initializing all the elements */
    srand((int)time(NULL));
    int i, j, k;
    struct sublist** list_id = (struct sublist**)malloc(list_num*sizeof(struct sublist*));
    for (i = 0; i < list_num; i++){
        list_id[i] = (struct sublist*)malloc(sizeof(struct sublist));
        list_id[i]->m_list = (SortedList_t*)malloc(sizeof(SortedList_t*));
        list_id[i]->mutex_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
        list_id[i]->spin_lock = 0;
    }
    
    srand((int)time(NULL));
    for(i = 0; i < thread_num; i++){
        SortedListElement_t** elements = (SortedListElement_t**)malloc(iterations_num*sizeof(SortedListElement_t*));
        for (j = 0; j < iterations_num; j++){
            elements[j] = (SortedListElement_t*)malloc(sizeof(SortedListElement_t));
            char* key = (char*)malloc(64*sizeof(char));
            for(k = 0; k < rand() % 64 + 1; k++)
                key[k] = (char)(rand() % 64 + 30);
            elements[j]->next = NULL;
            elements[j]->prev = NULL;
            elements[j]->key = key;
        }
        threads_args[i]->sublist = list_id;
        threads_args[i]->elements = elements;
        threads_args[i]->sync_type = sync_type;
        threads_args[i]->iter_num = iterations_num;
        threads_args[i]->list_num = list_num;
        threads_args[i]->wait_time = 0;
    }
    
    struct timespec start, end;
    char* test_type = (char*)malloc(sizeof(char)*18);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[thread_num];
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    pthread_attr_setstacksize(&attr, 100000000);

    int err;
    /* Creating threads */
    for (i = 0; i < thread_num; i++) {
        err = pthread_create(&threads[i], NULL, thread_function, (void*)threads_args[i]);
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
    //pthread_attr_destroy(&attr);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    int list_length = 0;
    for(int i = 0; i < list_num; i++){
        list_length = list_length + SortedList_length(list_id[i]->m_list);
    }
    if(list_length != 0){
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
    if (sync_type == 'm')
        strcat(test_type, "m");
    else if (sync_type == 's')
        strcat(test_type, "s");
    else
        strcat(test_type, "none");
    
    
    long operation_num = thread_num * iterations_num * 3;
    long runtime = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    long per_operation_time = runtime/operation_num;
    long long locks_wait_time = 0;
    for (int i=0; i<thread_num; i++)
        locks_wait_time += threads_args[i]->wait_time;
    
    long long avg_wait_time = locks_wait_time / lock_num;
    
    printf("%s,%d,%d,%d,%ld,%ld,%ld,%lld\n", test_type, thread_num, iterations_num, list_num,
           operation_num, runtime, per_operation_time, avg_wait_time);
    
    exit(0);
}
