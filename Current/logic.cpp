#include "logic.h"


bool tokeznizer(char *buffer,int size, vector<string>&v,int &total_token)
{
    // cout<<"logic : tokeznizer\n";
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
    return true;
}





bool read_command(string &key, char *buffer, string &string_buffer,database &db)
{
    // cout<<"logic : read_command\n";

    pthread_mutex_lock(&cache_lock[key[0]-'a']);

        int ans = cache[key[0]-'a'].read(key,buffer);

    pthread_mutex_unlock(&cache_lock[key[0]-'a']);

    if(!ans)
    {
        // row_affected
        ans = db.get(key,string_buffer);

        if(ans == 0) // if result_size (db.get())  = 0 mean , key not present 
        {
            return false;
        }
        else if(ans == 1)
        {
            pthread_mutex_lock(&cache_lock[key[0]-'a']);

                cache[key[0]-'a'].add(key,string_buffer);

            pthread_mutex_unlock(&cache_lock[key[0]-'a']);

            snprintf(buffer,BUFFSIZE,"%s",string_buffer.c_str());
            
            return true;
        }
        else{ //ans == -1
            // FIXME
            cout<<"Some error while read in db\n";
        }


        // return true;
    }
    return true;
}


bool add_command(string &key, string &value,database &db)
{
    // cout<<"logic : add_command\n";

    pthread_mutex_lock(&cache_lock[key[0]-'a']);

        int ans = cache[key[0]-'a'].check_present(key); // return true when key present in cache 

    pthread_mutex_unlock(&cache_lock[key[0]-'a']);

    if(!ans) 
    {
        ans = db.insert(key,value);
        if(ans==1) 
        {
            pthread_mutex_lock(&cache_lock[key[0]-'a']);

                cache[key[0]-'a'].add(key,value); 

            pthread_mutex_unlock(&cache_lock[key[0]-'a']);

            return true;
        }
        else if(ans == 0){ // key already present
            return false;
        }
        else{ 
            // ans == -1 
            // FIXME
            cout<<"Some error while inserting in db\n";
        }
    }
    return true;
}


bool update_command(string &key, string &value, database &db)
{
    // cout<<"logic : update_command\n";
    int ans = db.update(key,value);
    if(ans==1) 
    {
        pthread_mutex_lock(&cache_lock[key[0]-'a']);

            cache[key[0]-'a'].update(key,value);

        pthread_mutex_unlock(&cache_lock[key[0]-'a']);

        return true;
    }
    else if(ans == 0) // key not present 
    {
        return false;
    }
    else{
        // FIXME
        cout<<"Some error while updating in db\n";
    }
    return true;
}

bool delete_command(string &key, database &db)
{
    // cout<<"logic : delete_command\n";

    int ans = db.remove(key);

    pthread_mutex_lock(&cache_lock[key[0]-'a']);

        cache[key[0]-'a'].remove(key);

    pthread_mutex_unlock(&cache_lock[key[0]-'a']);

    if(ans == 0)
    {
        return false;
    }
    else if(ans == -1)
    {
        cout<<"Some error while deleting in db\n";
    }
    return true;
}


void *work(void *arg) // function that thread will be assigned
{
    // cout<<"logic : work\n";
    // initial allocation 
    int client_fd = *(int *)arg;
    free((int *)arg);

    char buffer[BUFFSIZE];

    string string_buffer;
    string_buffer.reserve(BUFFSIZE);

    vector<string> token(MAXTOKENS);
    for(int i = 0;i<MAXTOKENS;i++)
    {
        token[i].reserve(TOKENSIZE);
    }

    int receive_count = 0;
    int send_count = 0;
    int total_token = 0;
    database db;

    // ------------

    snprintf(buffer,BUFFSIZE,"START");
    // cout<<"First msg : start size : "<<sizeof(buffer)<<" --\n"; --- reason for abort 
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
                if(read_command(token[1],buffer,string_buffer,db))
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
                if(update_command(token[1],token[2],db))
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
                if(delete_command(token[1],db))
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
                if(add_command(token[1],token[2],db))
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
                // cout<<"Dissconect  inside\n";
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

    cout<<" db count : "<<db.count<<" --\n";

    // cout<<" Work finishe\n";
    // sem_post(&client_count);
    client_count.release();
    return NULL;
}