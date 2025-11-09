#include <iostream>
#include <stdlib.h>
// #include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore>


#include "logic.h"
#include "socketClass.h"
#include "memoryCache.h"

#define BUFFSIZE 5000
#define MAXTOKENS 5
#define TOKENSIZE 1000
#define read_command_length 2
#define add_command_length 3
#define update_command_length 3
#define delete_command_length 2
#define disscont_command_length 1
#define bucket 26

using namespace std;


counting_semaphore<20> client_count(20);

// sem_t client_count; // apple has depricated some of this function


// counting_semaphore<5> sdf(5); for cpp 20 +

struct memoryCache cache[bucket];
pthread_mutex_t cache_lock[bucket];


int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        cout<<"check arguents\n";
        return 0;
    }
    int __client_allowed = atoi(argv[3]);
    struct socketClass server;
    int cachememo = atoi(argv[2])/26; //cache memory
    int port = atoi(argv[1]);

    for(int i=0;i<bucket;i++)
    {
        cache[i].initialize(cachememo);
    }

    for(int i=0;i<bucket;i++)
    {
        pthread_mutex_init(&cache_lock[i],NULL);
    }


    server.start(port);
    // ::sem_init(&client_count,0,__client_allowed);
    cout<<"is it working?\n";
    int val = 0;
    while(true)
    {
        // sem_getvalue(&client_count, &val);
        // printf("Semaphore value = %d\n", val);
        // sem_wait(&client_count);


        client_count.acquire();
        int *client_fd = (int *)malloc(sizeof(int));
        *client_fd = server.accept();
        pthread_t new_client;
        pthread_create(&new_client,NULL,work,(void *)client_fd);

    }
    // sem_destroy(&client_count);
    return 0;
}