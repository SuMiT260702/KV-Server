#ifndef LOGIC_H
#define LOGIC_H


#include <iostream>
#include <stdlib.h>
#include <semaphore>
#include <unistd.h>
#include <pthread.h>

#include "socketClass.h"
#include "memoryCache.h"
#include "communication.h"
#include "database.hxx"

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


extern counting_semaphore<20> client_count;
extern struct memoryCache cache[bucket];
extern pthread_mutex_t cache_lock[bucket];

extern bool tokeznizer(char *buffer,int size, vector<string>&v,int &total_token);

// int custom_recv(int &client_fd, char *buffer,int size, int x);

// int custom_send(int &client_fd, char *buffer,int size, int x);


extern bool read_command(string &key, char *buffer,string &string_buffer, database &db);

extern bool add_command(string &key, string &value, database &db);

extern bool update_command(string &key, string &value,database &db);

extern bool delete_command(string &key,database &db);

extern void *work(void *arg); // function that thread will be assigned


#endif