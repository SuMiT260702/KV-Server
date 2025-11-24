#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <iostream>
#include <sys/socket.h>

/**
 * @brief a wrapper function for recv and send query, this will help in testing just have to modify this 
 * 
 */

inline int custom_recv(int &client_fd, char *buffer,int size, int x)
{
    // std::cout<<"logic : custom_recv\n";
    memset(buffer,'\0',size);
    int check = recv(client_fd,buffer,size,x);
    if(check <= 0)
    {
        std::cout<<"Error while receving\n";
        exit(1);
    }
    buffer[check]='\0';
    return check;
}


inline int custom_send(int &client_fd, char *buffer,int size, int x)
{
    // std::cout<<"logic : custom_send\n";
    int check = send(client_fd,buffer,size,x);
    if(check <= 0)
    {
        std::cout<<"Error while sending\n";
        exit(1);
    }
    return check;
}


#endif