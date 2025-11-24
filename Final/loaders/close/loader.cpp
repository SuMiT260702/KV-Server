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
#include <sstream> 
#include <fstream>
#include <sys/stat.h>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "socketClass.h"

#define BUFFSIZE 5000
#define MAX_THREAD 50

using namespace std;
vector<char *> keys;
vector<char *> values;

long start = true;
long stop = false;
long key_count = 0;
long value_count = 15;
double total_count = 0;
char hostname[100];
long port = 0;
long probability = 0;
long test_type = 0;
long total_latency = 0;


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



pid_t start_mpstat_single_core(int interval, int core, const string &logfile) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        // CHILD: redirect stdout and stderr to logfile
        int fd = open(logfile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open logfile");
            _exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            _exit(1);
        }
        if (dup2(fd, STDERR_FILENO) == -1) {
            perror("dup2 stderr");
            _exit(1);
        }
        close(fd);

        char core_str[16];
        char interval_str[16];
        snprintf(core_str, sizeof(core_str), "%d", core);
        snprintf(interval_str, sizeof(interval_str), "%d", interval);

        execlp("mpstat", "mpstat", "-P", core_str, interval_str, (char*)NULL);

        perror("execlp mpstat");
        _exit(1);
    }

    return pid;
}

int stop_mpstat(pid_t mpstat_pid) {
    if (mpstat_pid <= 0) return -1;
    if (kill(mpstat_pid, SIGTERM) != 0) {
        perror("kill mpstat");
    }

    int status = 0;
    if (waitpid(mpstat_pid, &status, 0) == -1) {
        perror("waitpid");
        return -1;
    }
    return 0;
}



bool file_Exists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void *test1() 
{
    /**
     * @brief 1. DISC HEAVY - ADD|UPDATE|DELETE 
     */
    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    double add_count = 0,update_count = 0,delete_count = 0;
    double latency = 0;
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
        command_type = rand()%3;
        if(command_type == 0) // add
        {
            add_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s] [%s]",keys[key_index],values[value_index]);

            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();

        }
        else if(command_type == 1) // update
        {
            update_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[UPDATE] [%s] [%s]",keys[key_index],values[value_index]);
            
            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();

        }
        else{
            delete_count++;
            key_index = rand()%key_count;
            snprintf(buffer,BUFFSIZE,"[DELETE] [%s]",keys[key_index]);

            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();

        }

        latency += std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();

    }

    snprintf(buffer,BUFFSIZE,"[DISCONNECT]");
    custom_send(server_fd,buffer,strlen(buffer),0);
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    close(server_fd);

    pthread_mutex_lock(&count_lock);
    total_count+=add_count + update_count + delete_count;
    total_latency += latency;
    pthread_mutex_unlock(&count_lock);


    return NULL;
}



void *test_2_3() 
{
    /**
     * @brief 
     * Work for type 2 and 3 test
     * 2. DISC HEAVY - UNIQUE READ GET ALL
     * 3. MEMORY HEAVY - GET POPULAR (SMALL SETS READS)
     */

    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    double read_count = 0;
    double latency = 0;
    long key_index = 0;
    char buffer[BUFFSIZE];
    struct socketClass host;

    long server_fd = host.connect(hostname,port);
    while(start)
    {;}
    custom_recv(server_fd,buffer,BUFFSIZE,0);

    while(!stop)
    {
        read_count++;
        key_index = rand()%key_count;
        snprintf(buffer,BUFFSIZE,"[READ] [%s]",keys[key_index]);

        start_time = std::chrono::high_resolution_clock::now();
        custom_send(server_fd,buffer,strlen(buffer),0);
        custom_recv(server_fd,buffer,BUFFSIZE,0);
        end_time = std::chrono::high_resolution_clock::now();

        latency += std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
        
    }

    snprintf(buffer,BUFFSIZE,"[DISCONNECT]");
    custom_send(server_fd,buffer,strlen(buffer),0);
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    close(server_fd);

    pthread_mutex_lock(&count_lock);
    total_count+=read_count ;
    total_latency += latency;
    pthread_mutex_unlock(&count_lock);

    return NULL;
}



void *test4()
{
    /**
     * @brief 
     * for test type 4 , this isnt using probability and LRU feature now
     * 4. GET|PUT - MIX
     */

    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    double latency = 0;


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
        command_type = rand()%4;
        if(command_type == 0) // add
        {
            add_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[ADD] [%s] [%s]",keys[key_index],values[value_index]);

            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();

        }
        else if(command_type == 1) // update
        {
            update_count++;
            key_index = rand()%key_count;
            value_index = rand()%value_count;
            snprintf(buffer,BUFFSIZE,"[UPDATE] [%s] [%s]",keys[key_index],values[value_index]);
            
            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();

        }
        else if(command_type == 2) // read
        {
            read_count++;
            key_index = rand()%key_count;
            snprintf(buffer,BUFFSIZE,"[READ] [%s]",keys[key_index]);

            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();
        }
        else{
            delete_count++;
            key_index = rand()%key_count;
            snprintf(buffer,BUFFSIZE,"[DELETE] [%s]",keys[key_index]);

            start_time = std::chrono::high_resolution_clock::now();
            custom_send(server_fd,buffer,strlen(buffer),0);
            custom_recv(server_fd,buffer,BUFFSIZE,0);
            end_time = std::chrono::high_resolution_clock::now();
        }

        latency += std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();

    }

    snprintf(buffer,BUFFSIZE,"[DISCONNECT]");
    custom_send(server_fd,buffer,strlen(buffer),0);
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    close(server_fd);

    pthread_mutex_lock(&count_lock);
    total_count+=add_count + read_count + update_count + delete_count;
    total_latency += latency;
    pthread_mutex_unlock(&count_lock);


    return NULL;
}

void *test(void *arg)
{
    
    if(test_type == 1)
    {
        test1();
    }
    else if(test_type == 2 ){
        test_2_3();
    }
    else if(test_type == 3){
        test_2_3();
    }
    else{
        test4();
    }
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

void destroy()
{
    long i = 0;
    while(i < key_count)
    {
        free(keys[i]);
        keys[i] = NULL;
        i++;
    }

    i = 0;
    while(i<value_count)
    {
        free(values[i]);
        values[i]=NULL;
        i++;
    }
    keys.clear();
    values.clear();
}

int main(int argc,char *argv[])
{
    long time_period = 0, memo = 0;
    cout<<"Enter host name (IP)\n";
    cin>>hostname;
    cout<<"Port Number\n";
    cin>>port;
    port = 5001;
    int num0Ftest = -1;
    cout<<"Enter Number of Test\n";
    cin>>num0Ftest;
    cout<<"Type of test \n";
    cout<<"1. DISC HEAVY - ADD|UPDATE|DELETE \n";
    cout<<"2. DISC HEAVY - UNIQUE READ GET ALL\n";
    cout<<"3. MEMORY HEAVY - GET POPULAR (SMALL SETS READS)\n";
    cout<<"4. GET|PUT - MIX\n";
    cin>>test_type;
    if(test_type == 3)
    {
        key_count = 100;
    }
    else{
        cout<<"Enter Number of Keys : \n";
        cin>>key_count;
    }
    if(test_type == 4)
    {
        cout<<"Enter Percentage of GET (cache hit) in MIX request\n";
        cin>>probability;
    }

    cout<<"Time period (seconds) :";
    cin>>time_period;

    value_count = 20;
    vector<int> thread_count;
    for(int i=2;i<=MAX_THREAD;i+=2)
    {
        thread_count.push_back(i);
    }

    // cout<<"Number of client thread : \n";
    // cin>>thread_count;
    
    for(int k = 0; k<thread_count.size(); k++)
    {
        int x = num0Ftest;
        while(x--)
        {
            total_latency = 0;
            total_count = 0;
            start = true;
            stop = false;

            cout<<"Preparing...\n";
            prepare();
            cout<<"Keys and values generated\n";
            vector<pthread_t> threads(thread_count[k]);
            for(long i=0;i<thread_count[k];i++)
            {
                pthread_create(&threads[i],NULL,test,NULL);
            }


            // utilization

            int monitor_core = 0;             
            int sample_interval = 1;          
            std::stringstream ss2;
            ss2 << "../../CPU/" << test_type<<"cpu_core"<<thread_count[k]<<".log";
            std::string logfile = ss2.str();
            pid_t mpid = start_mpstat_single_core(sample_interval, monitor_core, logfile);
            if (mpid <= 0) {
                cerr << "Failed to start mpstat. Exiting.\n";
                return 1;
            }
    

            cout<<"Threads created\n";
            sleep(1);

            cout<<"Testing...\n";
            start = false;
            sleep(time_period);
            stop = true;

            if (stop_mpstat(mpid) != 0) {
                cerr << "Warning: failed to stop mpstat cleanly\n";
            } else {
                cout << "Stopped mpstat (pid " << mpid << ")\n";
            }


            cout<<" thread joining\n";

            // sleep(5);
            for(long i=0;i<threads.size();i++)
            {
                pthread_join(threads[i],NULL);
            }

            cout<<" thread join complete \n";
            double avg_though = total_count/time_period;
            cout<<"Average throughput : "<<avg_though<<" \n";
            double avg_latency = total_latency/total_count;
            cout<<"Average Latency : "<<avg_latency<<"\n";

            std::stringstream ss;
            ss << "../../test_result/type" << test_type<< ".csv";
            
            
            std::string filename = ss.str();

            bool exists = file_Exists(filename.c_str());

            std::ofstream csv(filename, std::ios::app);

            if (!csv) {
                std::cerr << "Error opening file!\n";
                return 1;
            }
            
            

            if(!exists)
            {
                csv<<"thread_count,throughput,Latency\n"; // headers
            }

            csv<<thread_count[k]<<","<<avg_though<<","<<avg_latency<<"\n";
            destroy();
        }
    }
    return 0;
}