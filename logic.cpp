#include "logic.h"


bool tokeznizer(char *buffer,int size, vector<string>&v,int &total_token)
{
    cout<<"logic : tokeznizer\n";
    int i=0,j=0;
    while(i<size)
    {
        if(buffer[i]=='[')
        {
            i++;
            if(j>=v.size())
            {
                return false;
            }
            v[j].clear();
            while(i<size && buffer[i]!=']')
            {
                v[j].push_back(buffer[i]);
                i++;
            }
            j++;
        }
        else if(buffer[i]!=' ')
        {
            return false;
        }
        i++;
    }
    total_token = j;
    // cout<<"Tokens : ";
    // for(int i = 0;i<j;i++)
    // {
    //     cout<<v[i]<<" | ";
    // }
    // cout<<" \n";
    return true;
}


int custom_recv(int &client_fd, char *buffer,int size, int x)
{
    cout<<"logic : custom_recv\n";
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
    cout<<"logic : custom_send\n";
    int check = send(client_fd,buffer,size,x);
    if(check <= 0)
    {
        cout<<"Error while sending\n";
        exit(1);
    }
    return check;
}


bool read_command(string &key, char *buffer)
{
    cout<<"logic : read_command\n";
    pthread_mutex_lock(&cache_lock[key[0]-'a']);
    cout<<" Bucket : "<<key[0]-'a'<<" --\n";
    bool ans = cache[key[0]-'a'].read(key,buffer);
    // TODO db part remain
    pthread_mutex_unlock(&cache_lock[key[0]-'a']);
    return ans;
}


bool add_command(string &key, string &value)
{
    cout<<"logic : add_command\n";
    pthread_mutex_lock(&cache_lock[key[0]-'a']);
    cout<<" Bucket : "<<key[0]-'a'<<" --\n";
    bool ans = cache[key[0]-'a'].add(key,value);

    // TODO db part remain
    pthread_mutex_unlock(&cache_lock[key[0]-'a']);
    return ans;
}


bool update_command(string &key, string &value)
{
    cout<<"logic : update_command\n";
    pthread_mutex_lock(&cache_lock[key[0]-'a']);
    cout<<" Bucket : "<<key[0]-'a'<<" --\n";
    bool ans = cache[key[0]-'a'].update(key,value);

    // TODO db part remain
    pthread_mutex_unlock(&cache_lock[key[0]-'a']);
    return ans;
}

bool delete_command(string &key)
{
    cout<<"logic : delete_command\n";
    pthread_mutex_lock(&cache_lock[key[0]-'a']);
    cout<<" Bucket : "<<key[0]-'a'<<" --\n";
    bool ans = cache[key[0]-'a'].remove(key);

    // TODO db part remain
    pthread_mutex_unlock(&cache_lock[key[0]-'a']);
    return ans;
}


void *work(void *arg) // function that thread will be assigned
{
    cout<<"logic : work\n";
    // initial allocation 
    int client_fd = *(int *)arg;
    free((int *)arg);
    char buffer[BUFFSIZE];
    vector<string> token(MAXTOKENS);
    for(int i = 0;i<MAXTOKENS;i++)
    {
        token[i].reserve(TOKENSIZE);
    }
    int receive_count = 0;
    int send_count = 0;
    int total_token = 0;


    snprintf(buffer,BUFFSIZE,"START");
    // cout<<"First msg : start size : "<<sizeof(buffer)<<" --\n";
    custom_send(client_fd,buffer,strlen(buffer),0);
    
    // ------main work

    while (true)
    {
        receive_count = custom_recv(client_fd,buffer,BUFFSIZE,0);
        total_token = 0;
        if(tokeznizer(buffer,receive_count,token,total_token))
        {
            if(token[0].compare("READ")==0 && total_token == read_command_length)
            {
                memset(buffer,'\0',BUFFSIZE);
                if(read_command(token[1],buffer))
                {
                    cout<<" read buffer : "<<buffer<<" : "<<strlen(buffer)<<" --\n";
                    custom_send(client_fd,buffer,strlen(buffer),0);
                }
                else{
                    snprintf(buffer,BUFFSIZE,"KEY NOT PRESENT");
                    custom_send(client_fd,buffer,strlen(buffer),0);
                }
                
                continue;
            }
            else if(token[0].compare("UPDATE")==0 && total_token == update_command_length)
            {
                if(update_command(token[1],token[2]))
                {
                    snprintf(buffer,BUFFSIZE,"KEY UPDATED");
                }
                else{
                    snprintf(buffer,BUFFSIZE,"KEY NOT PRESENT");
                }
                custom_send(client_fd,buffer,strlen(buffer),0);
                continue;
            }
            else if(token[0].compare("DELETE")==0 && total_token == delete_command_length)
            {
                if(delete_command(token[1]))
                {
                    snprintf(buffer,BUFFSIZE,"KEY DELETED");
                }
                else{
                    snprintf(buffer,BUFFSIZE,"KEY NOT PRESENT");                   
                }
                custom_send(client_fd,buffer,strlen(buffer),0);
                continue;
            }
            else if(token[0].compare("ADD")==0 && total_token == add_command_length)
            {
                if(add_command(token[1],token[2]))
                {
                    snprintf(buffer,BUFFSIZE,"KEY ADDED");                    
                }
                else{
                    snprintf(buffer,BUFFSIZE,"KEY ALREADY PRESENT");
                }
                custom_send(client_fd,buffer,strlen(buffer),0);
                continue;
            }
            else if(token[0].compare("DISCONNECT")==0 && total_token == disscont_command_length)
            {
                cout<<"Dissconect  inside\n";
                snprintf(buffer,BUFFSIZE,"OK");
                custom_send(client_fd,buffer,strlen(buffer),0);
                close(client_fd);
                break;
            }
        }
        memset(buffer,'\0',BUFFSIZE);
        snprintf(buffer,BUFFSIZE,"INVALID COMMAND");
        custom_send(client_fd,buffer,strlen(buffer),0);
        
    }
    cout<<" Work finishe\n";
    sem_post(&client_count);
    return NULL;
}