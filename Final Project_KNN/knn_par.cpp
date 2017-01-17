#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"

using namespace std;

typedef pair<int,double>  PAIR;
const int colLen = 2;
const int rowLen = 20000000;
//const int rowLen = 20000000;
pthread_mutex_t mutex;
ifstream fin;
ofstream fout;

double dataSet[20000000][2];
//double dataSet[20000000][2];
char labels[20000000];
double *testData;
long  num_threads;
int k;
int *k_index_dis;
map<int,double> map_index_dis;
double index_dis[20000000];
map<char,int> map_label_freq;
double *label_freq;
struct CmpByValue
{
        bool operator() (const PAIR& lhs,const PAIR& rhs)
        {
                return lhs.second < rhs.second;
        }
};


void input_dataSet(){
    for(int i=0; i<rowLen; i++){
        //cout << i << endl;
        for(int j=0; j<colLen; j++){
            fin >> dataSet[i][j];
            //cout << dataSet[i][j] << endl;
        }
        fin >> labels[i];
    }

}
/*
void* input_dataSet(void* rank){
    struct drand48_data drand_buf;
    long my_rank = (long)rank;
    srand48_r(time(NULL), &drand_buf);
    //cout << my_rank << endl;
    for(int i=(int)my_rank; i<rowLen; i+=num_threads){
        //cout << i << endl;
        for(int j=0; j<colLen; j++){
            drand48_r(&drand_buf, &dataSet[i][j]);
            cout << dataSet[i][j] << endl;
        }
        labels[i]=floor(my_rank/2);
    }

}
*/
void init(){
    long init_thread;
    pthread_t* thread_handles;
    input_dataSet();
    /*
    thread_handles = new pthread_t[num_threads];
    pthread_mutex_init(&mutex, NULL);
    for(init_thread=0; init_thread<num_threads; init_thread++)
        pthread_create(&thread_handles[init_thread], NULL, input_dataSet, (void*) init_thread);
    //cout << "The main thread." << endl;
    for(init_thread=0; init_thread<num_threads; init_thread++)
        pthread_join(thread_handles[init_thread], NULL);
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    */
    //cout << dataSet[16827620][0] << endl;
    //cout << labels[0] << endl;
    //cout << labels[15000000] << endl;
    //cout << labels[2] << endl;
    //cout << "please input the test data :" << endl;
    srand((int)time(NULL));
    for(int p=0; p<colLen; p++)
        testData[p]=(rand()%2000)/1000;
}

/*
 * calculate the distance between test data and dataSet[i]
 */
double get_distance(double *d1, double *d2)
{
        double sum = 0;
        for(int i=0;i<colLen;i++)
        {
                sum += (d1[i]-d2[i])*(d1[i]-d2[i]);
        }

        //  cout<<"the sum is = "<<sum<<endl;
        return sqrt(sum);
}

/*
 * calculate all the distance between test data and each training data
 */
void* get_all_distance(void* arg)
{

        pthread_mutex_t mutex;
        double distance;
        long dis_my_rank = (long)arg;
        //cout << dis_my_rank << endl;

        for(int i=(int)dis_my_rank;i<rowLen;i+=num_threads)
        {
                distance = get_distance(dataSet[i],testData);
                //nnn<key,value> => <i,distance>
                index_dis[i]=distance;
                //map_index_dis[i] = distance;
        }
        //traverse the map to print the index and distance
/*
        map<int,double>::const_iterator it = map_index_dis.begin();

        while(it!=map_index_dis.end())
        {
                //cout<<"index = "<<it->first<<" distance = "<<it->second<<endl;
                it++;
        }

}
*/
}
/*
 * check which label the test data belongs to to classify the test data
 */
void get_max_freq_label()
{
        label_freq = new double[k];

        //transform the map_index_dis to vec_index_dis
/*
        vector<PAIR> vec_index_dis( map_index_dis.begin(),map_index_dis.end() );
*/
        //sort the vec_index_dis by distance from low to high to get the nearest data
        //sort(vec_index_dis.begin(),vec_index_dis.end(),CmpByValue());
        //sort(index_dis, index_dis+k);
        //quick-sort
        int counter=0;
        double smallest=99.0;
        k_index_dis = new int[k];
        for(int i=0;i<rowLen;i++){
            if(index_dis[i]<=smallest){
                k_index_dis[counter]=i;
                smallest = index_dis[i];
                counter++;
            }
            if(counter==k){
                counter=0;
            }
        }
        for(int i=0;i<k;i++)
        {
                //cout << "i " << k_index_dis[i] << endl;
                //cout<<"the index = "<<vec_index_dis[i].first<<" the distance = "<<vec_index_dis[i].second<<" the label = "<<labels[vec_index_dis[i].first]<<" the coordinate ( "<<dataSet[ vec_index_dis[i].first ][0]<<","<<dataSet[ vec_index_dis[i].first ][1]<<" )"<<endl;
                //calculate the count of each label
                int key;
                key = k_index_dis[i];    
                label_freq[i]=labels[key];
        }

        //map<char,int>::const_iterator map_it = map_label_freq.begin();
        char label;
        int label_0 = 0;
        int label_2 = 0;
        int j;
        //find the most frequent label
        while(j<k)
        {
                if(label_freq[j]==48)
                    label_0++;
                if(label_freq[j]==50)
                    label_2++;
                j++;
        }
        //cout << "label_0 is " << label_0 << endl;
        //cout << "label_2 is " << label_2 << endl;
        //if(label_0>label_2)
            //cout<<"The test data belongs to the 0 label"<<endl;
        //else
            //cout<<"The test data belongs to the 2 label"<<endl;

}

int main()
{
        int h;
        unsigned seed;
        double start, finish;
        //labels = new char[rowLen];
        testData = new double[colLen];        
        long main_thread;
        pthread_t* main_thread_handles;
        num_threads = 2;//nproces();
        main_thread_handles = new pthread_t[num_threads];
    fin.open("train.txt");
    if(!fin){
        cout << "can not open the file train.txt" << endl;
        exit(1);
    }
        for(h=0;h<1000;h++){
            GET_TIME(start);
            seed = (unsigned)time(NULL);
            srand(seed);
        //threads = (pthread_t*) malloc(num_threads*sizeof(pthread_t));
        //cout<<"please input the k value : "<<endl;
        //cin>>k;
            k = rand()%20000000+1;
            init();
            for(main_thread = 0; main_thread < num_threads; main_thread++)
            {        
                    pthread_create(&main_thread_handles[main_thread], NULL, get_all_distance, (void*)main_thread);
            }
            for(main_thread = 0; main_thread < num_threads; main_thread++)
            pthread_join(main_thread_handles[main_thread], NULL);

        free(main_thread_handles);
        get_max_freq_label();
        GET_TIME(finish);
        cout << finish-start << endl;
        }
        return 0;
}

