#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
 
pthread_mutex_t mutex;
long long number_in_circle;
 
long long threads_tosses;
long long ramain_tosses;
int SEED;
 
void* tossDarts(void*);
 
int main(int argc, char **argv) {
    pthread_t* threads;
    
    long long number_of_tosses = atoll(argv[1]);
    long long ramain_tosses;
    double pi_estimate;
    long thread, num_threads;
     
    // Initial
    num_threads = get_nprocs();
    
    SEED = time(NULL);
    threads_tosses = number_of_tosses / num_threads;
    ramain_tosses = number_of_tosses % num_threads;
    
    threads = malloc(sizeof(pthread_t)*num_threads);
     
    // Toss
    pthread_mutex_init(&mutex, NULL);
    for(thread = 0; thread < num_threads; thread++)
        pthread_create(&threads[thread], NULL, tossDarts, (void *)thread);
    pthread_mutex_destroy(&mutex);
         
    // Join
    for(thread = 0; thread < num_threads; thread++)
        pthread_join(threads[thread], NULL);
        
    free(threads);
     
    // Estimate
    pi_estimate = 4*number_in_circle /((double)number_of_tosses);
    printf("%lf\n", pi_estimate);
     
    return 0;
}
 
void* tossDarts(void* arg) {
    struct drand48_data drand_buf;
    long long tid = (long long) arg;
    long long result = 0;
    long long toss;
    double x, y;
    int seed = SEED + tid;
    long long tosses;
     
    srand48_r(seed, &drand_buf);
    
    tosses = threads_tosses;
    if(tid == 0)
        tosses += ramain_tosses;
     
    for(toss = 0; toss < tosses; toss++) {
        drand48_r(&drand_buf, &x);
        drand48_r(&drand_buf, &y);  
        if(x*x + y*y <= 1)   result++;
    }
     
    // Join Data
    pthread_mutex_lock(&mutex);
    number_in_circle += result;
    pthread_mutex_unlock(&mutex);
    
    return;
}
