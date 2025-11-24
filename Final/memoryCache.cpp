#include "memoryCache.h"

#define ENDPOINT '\0'

memoryCache::memoryCache()
{

}

void memoryCache::initialize(int size)
{
    // cout<<"memoryCache:: initialize \n";
    int noOfNode = pool.makepool(size);
    lru_cache.lru_initial(noOfNode);
    new_value = NULL;
    memory_size = size;
    lru_delete_key.reserve(3000); /// this part remember it

}

memoryCache::~memoryCache()
{
    pool.destroy();
}

bool memoryCache:: add(string &key, string &value)
{
    // cout<<"memoryCache:: add ---- "<< key.size()<<" -\n";
    // check if present or not
    
    if(check_present(key))
    {
        return false;
    }

    // calculating required node
    int value_size = value.size();
    int requiredNode = value_size/N_Node_value_size + 1;// for end point
    // cout<<" require Node : "<<requiredNode<<" \n";
    // if(value_size%N_Node_value_size == 0) // for end point
    // {
    //     requiredNode++;
    // }
    new_value = NULL;
    // cout<<" let see ------\n";
    // making nodes for new value 
    valueStruct *curr = NULL, *node = NULL;
    while(true)
    {
        node = pool.getNode();
        if(node == NULL)
        {
            remove_lru();
            continue;
        }
        
        if(new_value == NULL)
        {
            new_value = node;
            curr = node;
        }
        else{
            curr->next = node;
            curr = node;
        }

        requiredNode--; // we get a node
        if(requiredNode==0) break;
    }
    curr->next = NULL;

    // cout<<" are we here \n";
    // copying value in node 
    curr = new_value; // back to starting node
    int node_index = 0,value_index = 0;
    while(true)
    {
        while(node_index<N_Node_value_size && value_index<value_size) // N_Node_value_size  buffer size in node
        {
            curr->value[node_index++] = value[value_index++];
            // node_index++;
            // value_index++;
        } 
        // cout<<" are we here 2\n";
        
        if(node_index<N_Node_value_size && value_index==value_size)
        {
            break;
        }

        // it should be after if statment otherwise it will give segmentation in line curr->value
        curr = curr->next; // this is always works as we assign a extra node when value_size%N_Node_value_size == 0 
        node_index = 0;
    }

    // putting endpoint
    // cout<<"are we here 3";
    curr->value[node_index] = ENDPOINT;

    // adding in hash map
    map[key] = new_value;
    lru_cache.touch(key);
    // Initiliazing for next operation
    new_value = NULL;

    // print_key(key);
    // cout<<"memoryCache:: add finish  -\n";
    return true;

}

void memoryCache::print_key(string &key)
{
    // cout<<"memoryCache:: print_key \n";
    // cout<<"key : "<<key<<" : ";
    node = map[key];
    while(node!=NULL)
    {
        cout<<node->value;
        node = node->next;
    }

    // cout<<"   ---\n";
    return ;
}

bool memoryCache::check_present(string &key)
{
    // cout<<"memoryCache:: check_present \n";
    try
    {
        if(map.find(key)==map.end())
        {
            // cout<<" returning false check_present--\n";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << " ---- "<<e.what() << '\n';
    }
    
    
    // cout<<" returning --\n";
    return true;
}

bool memoryCache::remove(string &key)
{
    // cout<<"memoryCache:: remove \n";
    // check if present or not
    if(!check_present(key))
    {
        return false;
    }
    node = map[key];
    pool.putNode(node);
    node = NULL;
    map.erase(key);
    lru_cache.delete_key(key);
    return true;
}

void memoryCache::update(string &key,string &value)
{
    // cout<<"memoryCache:: update \n";
    // check if present or not
    if(!check_present(key))
    {
        add(key,value);// update lru 
        return;
    }
    remove(key);
    add(key,value);// update lru 
    return;
}


bool memoryCache::remove_lru()
{
    // cout<<"memoryCache:: remove_lru \n";
    if(lru_cache.remove(lru_delete_key)==false)
    {
        // cout<<"head null of lru, some problem\n";
        exit(0);
    }
    // if(!check_present(lru_delete_key)) if its in lru, it will always be in cache
    // {
    //     return false;
    // }
    node = map[lru_delete_key];
    pool.putNode(node);
    map.erase(lru_delete_key);
    node = NULL;
    return true;
}

bool memoryCache::read(string &key,char *buffer)
{
    // cout<<"memoryCache:: read \n";
    if(!check_present(key))
    {
        return false;
    }
    node = map[key];
    int node_index = 0;
    int i = 0;
    while(true)
    {
        while(node_index < N_Node_value_size && node->value[node_index]!='\0')
        {
            buffer[i++] = node->value[node_index++];
        }
        if(node_index < N_Node_value_size && node->value[node_index]=='\0')
        {
            buffer[i++] = '\0';
            break;
        }
        node_index = 0;
        node=node->next;
    }
    // print_key(key);
    // cout<<"inside cache read -- buffer : "<<buffer<<" \n";
    
    return true;
}