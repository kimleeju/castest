#include <iostream>
#include <pthread.h>
#include <atomic>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

class MultiArg{
    public:
    MultiArg(int* ar1, int* ar2, int cnt){
        arg1 = ar1;
        arg2 = ar2;
        count = cnt;
    }
    MultiArg(){}
    int* arg1;
    int* arg2;
    int count;
    int num;
};

std::atomic<int*>*key; 
int* bf = 0;
struct timeval t_start;
struct timeval t_end;



void* t_function(void* ma) 
{
    MultiArg* mma = (MultiArg*)ma;
    //std::atomic<int*>key; 
    while(mma->count > 0){ 
        while(1){
            mma->arg1 = (&key[mma->num])->load(memory_order_acquire);
            if((&key[mma->num])->compare_exchange_strong(mma->arg1,mma->arg2)){
                break;
            }   
        }   
        mma->count--;
    }   
}

int main(int argc, char* argv[]){
    if(argc < 3){ 
        cout<<"Usage: ./a.out thread_num operation_num"<<endl;
    }   
    int t_num = atoi(argv[1]);
    int op = atoi(argv[2]);
    int* operation = &op;
    pthread_t p_thread[t_num];
    MultiArg *MA = new MultiArg[t_num];
    key = new std::atomic<int*>[t_num];

    gettimeofday(&t_start,NULL);
    
    for(int i = 0 ; i < t_num ; i++){
        MA[i].arg2 = operation;
        MA[i].count = op/t_num;
        MA[i].num = i;
	MultiArg* Multiarg = &MA[i];
        pthread_create(&p_thread[i], NULL, t_function, (void* )Multiarg);
    }   
    
    for(int i = 0 ; i < t_num ; i++){
        pthread_join(p_thread[i], (void **)NULL);
    }   
    
    gettimeofday(&t_end,NULL);
    float lat = (t_end.tv_sec-t_start.tv_sec + (t_end.tv_usec - t_start.tv_usec) / 1000000.0);
    printf("IOPS = %6.2f\n",op/lat);

}   

