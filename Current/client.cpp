#include <iostream>
#include <string>
#include "socketClass.h"
using namespace std;

#define BUFFSIZE 5000
struct socketClass host;

int custom_recv(int &client_fd, char *buffer,int size, int x)
{
    memset(buffer,'\0',size);
    int check = recv(client_fd,buffer,size,x);
    if(check <= 0)
    {
        cout<<"Error while receving\n";
        exit(1);
    }
    buffer[check]='\0';
    return check;
}

int custom_send(int &client_fd, char *buffer,int size, int x)
{
    int check = send(client_fd,buffer,size,x);
    if(check <= 0)
    {
        cout<<"Error while sending\n";
        exit(1);
    }
    return check;
}

int send_buffer(char *buffer,int &client_fd)
{
    int check = send(client_fd,buffer,strlen(buffer),0);
    if(check <= 0)
    {
        cout<<"Error while sending\n";
        exit(1);
    }
    return check;
}

void work(char *hostname, int port)
{
    char buffer[BUFFSIZE];
    int server_fd = host.connect(hostname,port);
    cout<<"Connected\n";
    custom_recv(server_fd,buffer,BUFFSIZE,0);
    cout<<buffer<<"\n";
    // --- start

    int recv_count = 0;
    while(true)
    {
        scanf("%[^\n]",buffer);
        getchar();

        custom_send(server_fd,buffer,strlen(buffer),0);
        custom_recv(server_fd,buffer,BUFFSIZE,0);

        cout<<"SERVER : "<<buffer<<"\n";
    }
}


int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cout<<"Not correct number of argument\n";
        exit(0);
    }
    work(argv[1],atoi(argv[2]));
    return 0;
}