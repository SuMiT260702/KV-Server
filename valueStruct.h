#ifndef VALUESTRUCT_H
#define VALUESTRUCT_H

#define N 100

// class that our memory cache will use to store value and create pool
class valueStruct
{
    public:
        char value[N];
        valueStruct *next;
        valueStruct()
        {
            next=nullptr;
        }
};

#endif