#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "socketClass.h"

#define BUFFSIZE 5000

using namespace std;
vector<char *> keys;
vector<char *> values;

long start = true;
long stop = false;
long key_count = 0;
long value_count = 15;
double total_count = 0;
char hostname[100] = "localhost";
long port = 0;




pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

long custom_recv(long &client_fd, char *buffer,long size, long x)
{
    memset(buffer,'\0',size);
    long check = recv(client_fd,buffer,size,x);
    if(check <= 0 || check>=BUFFSIZE)
    {
        cout<<"Error while receving\n";
        cout<<"Buffer : "<<buffer<<" -- check : "<<check<<" --\n";
        exit(1);
    }
    buffer[check]='\0';
    return check;
}

long custom_send(long &client_fd, char *buffer,long size, long x)
{
    long check = send(client_fd,buffer,size,x);
    if(check <= 0)
    {
        cout<<"Error while sending\n";
        exit(1);
    }
    return check;
}


void *test(void *arg)
{
    double add_count = 0,read_count = 0,update_count = 0,delete_count = 0;
    long key_index = 0;
    long value_index = 0;
    long command_type = 0;
    char buffer[BUFFSIZE];
    struct socketClass host;
    long server_fd = host.connect(hostname,port);
    while(start)
    {;}
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    while(!stop)
    {
        // continue;
        // command_type = rand()%4;
        command_type = 0;
        if(command_type == 0) // add
        {
            add_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s] [%s]",keys[key_index],values[value_index]);
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);

        }
        else if(command_type == 1) // update
        {
            update_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s] [%s]",keys[key_index],values[value_index]);
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);

        }
        else if(command_type == 2) // read
        {
            read_count++;
            key_index = rand()%key_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s]",keys[key_index]);
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
        }
        else{
            delete_count++;
            key_index = rand()%key_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s]",keys[key_index]);
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
        }

    }
    snprintf(buffer,BUFFSIZE,"[DISCONNECT]");
    custom_send(server_fd,buffer,strlen(buffer),0);
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    close(server_fd);
    cout<<"SERVER : "<<buffer<<" --\n";

    pthread_mutex_lock(&count_lock);
    cout<<"add_count : "<<add_count<<" \n";
    total_count+=add_count + read_count + update_count + delete_count;
    pthread_mutex_unlock(&count_lock);


    cout<<"Task finish\n";
    return NULL;
}

void prepare()
{
    long i = 0;
    char *p;
    while(i<key_count)
    {
        long k = rand()%100 + 1;
        p = (char *)malloc(k+1);
        for(long j = 0;j<k;j++)
        {
            p[j] = (char)(rand()%(26) + 97);
        }
        p[k] = '\0';
        keys.push_back(p);
        i++;
    }
    i = 0;
    while(i<value_count)
    {
        long k = rand()%50 + 1;
        p = (char *)malloc(k+1);
        for(long j = 0;j<k;j++)
        {
            p[j] = (char)(rand()%(26) + 97);
        }
        p[k] = '\0';
        values.push_back(p);
        i++;
    }


}

int main(int argc,char *argv[])
{
    long thread_count = 0,time_period = 0, memo = 0,test_type = 0;
    // cout<<"Enter host name (IP)\n";
    // cin>>hostname;
    // cout<<"Port Number\n";
    // cin>>port;
    port = 5001;
    // cout<<"Enter Number of Keys : \n";
    // cin>>key_count;
    key_count = 10000000;
    // cout<<"Enter Number of values : \n";
    // cin>>value_count;
    value_count = 20;
    cout<<"Number of client thread : \n";
    cin>>thread_count;
    cout<<"Time period (seconds) :";
    cin>>time_period;
    // time_period = 30;
    // cout<<"Memory for server : ";
    // cin>>memo;
    // cout<<"Type of test \n";
    // cout<<"1. DISC HEAVY - ADD|UPDATE|DELETE \n";
    // cout<<"2. DISC HEAVY - UNIQUE READ\n";
    // cout<<"3. MEMORY HEAVY - GET POPULAR (SMALL SETS READS)\n";
    // cout<<"4. GET|PUT - MIX\n";
    // cin>>test_type;

    cout<<"Preparing...\n";
    prepare();
    cout<<"Keys and values generated";
    vector<pthread_t> threads(thread_count);
    for(long i=0;i<thread_count;i++)
    {
        pthread_create(&threads[i],NULL,test,NULL);
    }

    cout<<"Threads created\n";
    sleep(5);
    cout<<"Starting...\n";
    start = false;
    sleep(time_period);
    stop = true;

    cout<<" thread joining\n";

    // sleep(5);
    for(long i=0;i<threads.size();i++)
    {
        pthread_join(threads[i],NULL);
    }

    cout<<" thread join complete \n";
    double avg_though = total_count/time_period;
    cout<<"Average throughput : "<<avg_though<<" \n";


    return 0;
}