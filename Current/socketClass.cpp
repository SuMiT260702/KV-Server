#include "socketClass.h"

socketClass::socketClass(/* args */)
{
}

socketClass::~socketClass()
{
}


int socketClass::start(int port)
{
    // cout<<"socketClass:: start \n";
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1)
    {
        cout<<"Socket Failed\n";
        exit(11);
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // any address
    address.sin_port = htons(port);

    int check = ::bind(server_fd, (struct sockaddr*)&address, sizeof(address)); // there is a std::bind function also 
    if(check < 0)
    {
        cout<<"Error in Bind\n";
        exit(12);
    }

    check = ::listen(server_fd,QUE_LEN);

    if(check < 0)
    {
        cout<<"Error in listen\n";
        close(server_fd);
        exit(12);
    }

    cout<<"listing on "<<port<<" ...\n";
    return server_fd;
}

int socketClass::accept()
{
    // cout<<"socketClass:: accept \n";
    socklen_t addrlen = sizeof(address);
    int client_fd = ::accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if(client_fd < 0)
    {
        cout<<"error in accept\n";
        exit(10);
    }
    cout<<"Client Connected \n";
    return client_fd;
}

int socketClass::connect(char *hostname,int port)
{
    // cout<<"socketClass:: connect \n";
    struct hostent *server;
    //-------> address; // server_address
    server = gethostbyname(hostname);
    if (server == NULL) {
        cout<<" hostname problem \n";
        exit(0);
    }
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd < 0)
    {
        cout<<" connect socket fail\n";
        exit(0);
    }

    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr,server->h_length);
    address.sin_port = htons(port);


    if (::connect(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
    {
        cout<<"connect connect problem\n";
        exit(0);
    }

    return server_fd;
}