#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <cmath>

using namespace std;

int k;
long num_threads;
void* input_dataSet(void* rank);
void init();
const int colLen = 2;
const int rowLen = 12;
ifstream fin;
ofstream fout;

double dataSet[12][2];
char labels[12];
double testData[2];

int main(int argc, char* argv[]){
    long dis_thread;
    pthread_t* dis_thread_handles;
    //number of threads
    num_threads = 4;//get_nprocs();
    //init
    cout << "Please input the k value :" << endl;
    cin >> k;
    init();
    //cout << dataSet[0][0] << endl;
    //cout << dataSet[0][1] << endl;
    //cout << labels[2] << endl;
    return 0;
}

void init(){
    long thread;
    pthread_t* thread_handles;
    fin.open("data.txt");
    if(!fin){
        cout << "Can't open the file." << endl;
        exit(1);
    }
    //input the dataSet
    thread_handles = new pthread_t[num_threads];
    for(thread=0; thread<num_threads; thread++)
        pthread_create(&thread_handles[thread], NULL, input_dataSet, (void*) thread);
    //printf("The main thread\n");

    for(thread=0; thread<num_threads; thread++)
        pthread_join(thread_handles[thread], NULL);
    free(thread_handles);
    //cout << "Please input the test data :" << endl;
    //for(int i=0; i<colLen; i++)
    //    cin >> testData[i];  
}

void* input_dataSet(void* rank){
    long my_rank = (long) rank;
    //printf("From thread %1d of %d\n", my_rank, num_threads);
    for(int i=(int)my_rank; i<rowLen; i+=num_threads){
        for(int j=0; j<colLen; j++){
            //cout << fin << endl;
            fin >> dataSet[i][j];
        }
        fin >> labels[i];
    }

    return NULL;
}
