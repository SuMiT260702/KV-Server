#include "lru.h"


void lru::lru_initial(int numofNode)
{
    cout<<"lru::lru_initial \n";
    head = NULL;
    tail = NULL;
    pool.makePool(numofNode);
}

bool lru::touch(string &key) // if we have a key then make it recent access, if not add key and make it recent access
{
    cout<<"lru::touch \n";
    if(map.find(key)==map.end())
    {
        node = pool.getNode(); //always have node
        if(node == NULL)
        {
            cout<<"Some issue with Pool\n";
            exit(3);
        }
        node->key.assign(key);
        map[key] = node;
        if(head == NULL)
        {
            head = node;
            tail = node;
        }
        else{
            tail->next = node;
            node->prev = tail;
            node->next = NULL;
            tail = node;
        }
        return true;
    }
    else{
        node = map[key];
        if(head == node && tail == node)
        {
            return true;
        }
        else if(tail == node)
        {
            return true;
        }
        else if(head == node)
        {
            head = node->next;
            head->prev = NULL;
            tail->next = node;
            node->prev = tail;
            tail = node;
            node->next = NULL;
        }
        else{
            node->prev->next = node->next;
            node->next->prev = node->prev;
            tail->next = node;
            node->next = NULL;
            node->prev = tail;
            tail = node;
        }

        return true;
    }
}

bool lru::remove(string &delete_key)
{
    cout<<"lru::remove \n";
    if(head == NULL)
    {
        return false;
    }
    if(head == tail)
    {
        node = head;
        head = NULL;
        tail = NULL;
    }
    else{
        node = head;
        head = head->next;
        head->prev = NULL;
    }
    delete_key.assign(node->key);
    map.erase(node->key);
    pool.putNode(node);
    node = NULL;
    return true;
}


bool lru::delete_key(string &key)
{
    cout<<"lru::delete_key \n";
    if(map.find(key)==map.end())
    {
        cout<<"key not present in lru\n";
        exit(4);
    }
    node = map[key];
    map.erase(key);
    if(node == head && node == tail)
    {
        head = NULL;
        tail = NULL;
    }
    else if(tail == node)
    {
        tail = tail->prev;
        tail->next = NULL;
    }
    else if(head == node)
    {
        head = head->next;
        head->prev = NULL;
    }
    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    pool.putNode(node);
    node = NULL;
    return true;
}

