// this is basically cache memory pool
#ifndef VALUESTRUCTPOOL_H
#define VALUESTRUCTPOOL_H

#include <iostream>
#include "valueStruct.h"


class valueStructPool
{
    private:
        valueStruct *head = NULL;
        valueStruct *start = NULL;
        valueStruct *top = NULL;
        valueStruct *node = NULL;
    public:
        valueStruct *getNode();
        int makepool(int size);
        void putNode(valueStruct *node);
        void destroy();

};
#endif