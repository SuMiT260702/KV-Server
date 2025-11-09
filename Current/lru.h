#ifndef LRU_H
#define LRU_H


#include <iostream>
#include <gssapi.h>


#include "dll.h"
#include "dllPool.h"


class lru{
    dllPool pool;
    dll *head;
    dll *tail;
    unordered_map<string,dll *>map;
    dll *node;
    public:
    // string key_return;
    void lru_initial(int numOfNode);
    bool touch(string &key);
    bool remove(string &deleted_key);
    bool delete_key(string &key);
};

#endif