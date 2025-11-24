#ifndef VALUESTRUCT_H
#define VALUESTRUCT_H

#define N_Node_value_size 100

// class that our memory cache will use to store value and create pool
class valueStruct
{
    public:
        char value[N_Node_value_size];
        valueStruct *next;
        valueStruct()
        {
            next=nullptr;
        }
};

#endif