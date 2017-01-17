#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <pthread.h>

using namespace std;

pthread_mutex_t mutex;
typedef char tLabel;
typedef double tData;
typedef pair<int,double>  PAIR;
const int colLen = 2;
const int rowLen = 20000000;
ifstream fin;
ofstream fout;

class KNN
{
        public:
                tData dataSet[rowLen][colLen];
                tLabel labels[rowLen];
                tData testData[colLen];
                int k;
                double distance;
                void get_distance(void* arg);               
                map<int,double> map_index_dis;
                map<tLabel,int> map_label_freq;
        public:

                KNN(int k);

                void get_all_distance();

                void get_max_freq_label();

                struct CmpByValue
                {
                        bool operator() (const PAIR& lhs,const PAIR& rhs)
                        {
                                return lhs.second < rhs.second;
                        }
                };

};

//open file
KNN::KNN(int k)
{
        this->k = k;

        fin.open("train.txt");

        if(!fin)
        {
                cout<<"can not open the file data.txt"<<endl;
                exit(1);
        }

        /* input the dataSet */
        for(int i=0;i<rowLen;i++)
        {
                for(int j=0;j<colLen;j++)
                {
                        fin>>dataSet[i][j];
                }
                fin>>labels[i];
        }

        cout<<"please input the test data :"<<endl;
        /* inuput the test data */
        for(int i=0;i<colLen;i++)
                cin>>testData[i];

}

/*
 * calculate the distance between test data and dataSet[i]
 */
void KNN:: get_distance(void* arg)
{
        tData *d1 = (tData*)arg;
        tData *d2 = testData;
        double sum = 0;
        for(int i=0;i<colLen;i++)
        {
                sum += pow( (d1[i]-d2[i]) , 2 );
        }

        //  cout<<"the sum is = "<<sum<<endl;
        distance = sqrt(sum);
}

/*
 * calculate all the distance between test data and each training data
 */
void KNN:: get_all_distance()
{
        pthread_t* threads;
        long thread, num_threads;
        //initial
        num_threads = 4;//nprocs();

        threads = new pthread_t[num_threads];
        
        long i;
        //create
        for(i=0;i<rowLen;i++)
        {
            pthread_mutex_init(&mutex, NULL);
            pthread_create(&threads[i%4], NULL, get_distance, (void*)dataSet[i]);
            //distance = get_distance(dataSet[i],testData);
            //<key,value> => <i,distance>
            map_index_dis[i] = distance;
            pthread_mutex_destroy(&mutex);
        }

        //traverse the map to print the index and distance
        map<int,double>::const_iterator it = map_index_dis.begin();
        while(it!=map_index_dis.end())
        {
                //cout<<"index = "<<it->first<<" distance = "<<it->second<<endl;
                it++;
        }
        //join
        for(thread=0; thread<num_threads; thread++)
            pthread_join(threads[thread], NULL);

        free(threads);
}

/*
 * check which label the test data belongs to to classify the test data
 */
void KNN:: get_max_freq_label()
{
        //transform the map_index_dis to vec_index_dis
        vector<PAIR> vec_index_dis( map_index_dis.begin(),map_index_dis.end() );
        //sort the vec_index_dis by distance from low to high to get the nearest data
        sort(vec_index_dis.begin(),vec_index_dis.end(),CmpByValue());

        for(int i=0;i<k;i++)
        {
                //cout<<"the index = "<<vec_index_dis[i].first<<" the distance = "<<vec_index_dis[i].second<<" the label = "<<labels[vec_index_dis[i].first]<<" the coordinate ( "<<dataSet[ vec_index_dis[i].first ][0]<<","<<dataSet[ vec_index_dis[i].first ][1]<<" )"<<endl;
                //calculate the count of each label
                map_label_freq[ labels[ vec_index_dis[i].first ]  ]++;
        }

        map<tLabel,int>::const_iterator map_it = map_label_freq.begin();
        tLabel label;
        int max_freq = 0;
        //find the most frequent label
        while( map_it != map_label_freq.end() )
        {
                if( map_it->second > max_freq )
                {
                        max_freq = map_it->second;
                        label = map_it->first;
                }
                map_it++;
        }
        cout<<"The test data belongs to the "<<label<<" label"<<endl;
}

int main(int argc, char** argv)
{
        int k ;
        cout<<"please input the k value : "<<endl;
        cin>>k;

        KNN knn(k);
        knn.get_all_distance();
        knn.get_max_freq_label();

        return 0;
}

