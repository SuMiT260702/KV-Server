#ifndef MEMORYCACHE_H
#define MEMORYCACHE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <array>
#include "valueStruct.h"
#include "valueStructPool.h"
#include "lru.h"

using namespace std;
class memoryCache
{
    private:
        int memory_size = 0;
        lru lru_cache;
        string lru_delete_key;
        unordered_map<string,struct valueStruct *> map; // we can use a custom allocator here to, not dynamically allocate memeory for map nodes
        struct valueStruct *new_value;
        struct valueStructPool pool;
        struct valueStruct *node;
        struct valueStruct *curr;
        bool remove_lru();
        bool check_present(string &key);
        void print_key(string &key);
    public:
        memoryCache();
        ~memoryCache();
        void initialize(int size);
        bool add(string &key, string &value);
        bool remove(string &key);
        bool update(string &key, string &value);
        bool read(string &key,char *buffer);
};

#endif