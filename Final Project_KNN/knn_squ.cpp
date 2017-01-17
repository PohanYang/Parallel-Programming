#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include "timer.h"

using namespace std;

int main(int argc, char* argv[]);
typedef pair<int,double>  PAIR;
const int colLen = 2;
const int rowLen = 20000000;
ifstream fin;
ofstream fout;

void init(); 
double get_distance(double *d1,double *d2);
void get_all_distance();
void get_max_freq_label();

double dataSet[rowLen][colLen];
char labels[rowLen];
double testData[colLen];
int k;
map<int,double> map_index_dis;
map<char,int> map_label_freq;
struct CmpByValue
{
    bool operator() (const PAIR& lhs,const PAIR& rhs)
    {
        return lhs.second < rhs.second;
    }
};


void init()
{

        /* input the dataSet */
        for(int i=0;i<rowLen;i++)
        {
                for(int j=0;j<colLen;j++)
                {
                        fin>>dataSet[i][j];
                }
                fin>>labels[i];
        }
        //cout<<"please input the test data :"<<endl;
        /* inuput the test data */
        srand((int)time(NULL));
        for(int i=0;i<colLen;i++)
                testData[i] = (rand()%2000)/1000;

}

/*
 * calculate the distance between test data and dataSet[i]
 */
double get_distance(double *d1,double *d2)
{
        double sum = 0;
        for(int i=0;i<colLen;i++)
        {
                sum += pow( (d1[i]-d2[i]) , 2 );
        }

        //  cout<<"the sum is = "<<sum<<endl;
        return sqrt(sum);
}

/*
 * calculate all the distance between test data and each training data
 */
void get_all_distance()
{
        double distance;
        int i;
        for(i=0;i<rowLen;i++)
        {
                distance = get_distance(dataSet[i],testData);
                //<key,value> => <i,distance>
                map_index_dis[i] = distance;
        }

        //traverse the map to print the index and distance
        map<int,double>::const_iterator it = map_index_dis.begin();
        while(it!=map_index_dis.end())
        {
                //cout<<"index = "<<it->first<<" distance = "<<it->second<<endl;
                it++;
        }
}

/*
 * check which label the test data belongs to to classify the test data
 */
void get_max_freq_label()
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

        map<char,int>::const_iterator map_it = map_label_freq.begin();
        //for(map_it=map_label_freq.begin();map_it!=map_label_freq.end();map_it++)
            //cout << map_it->first << " " << map_it->second << endl;
        char label;
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
        //cout<<"The test data belongs to the "<<label<<" label"<<endl;
}

int main(int argc, char* argv[])
{
        FILE *pFileBefore;
        char cBuffer[8];
        pFileBefore = fopen("squ_answer.txt","wb");
        if(pFileBefore==NULL){
            cout << "Failure" << endl;
            return 1;
        }
        int h;
        unsigned seed;
        double start, finish, elapsed;
        fin.open("train.txt");

        if(!fin)
        {
                cout<<"can not open the file data.txt"<<endl;
                exit(1);
        }
        for(h=0; h<1000; h++){
            GET_TIME(start);
            seed = (unsigned)time(NULL);
            srand(seed);
            //cout<<"please input the k value : "<<endl;
            //cin >> k;
            k = rand()%20000000+1;
            init();
            get_all_distance();
            get_max_freq_label();
            GET_TIME(finish);
            elapsed = finish-start;
            sprintf(cBuffer, "%f", elapsed);
            fwrite(cBuffer, 1, sizeof(double)+1, pFileBefore);
            cout << elapsed << endl;
        }
        fclose(pFileBefore);
        return 0;
}

