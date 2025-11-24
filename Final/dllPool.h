#ifndef DLLPOOL_H
#define DLLPOOL_H

#include "dll.h"

class dllPool{
    private:
        int noOfNode = 0;
        dll *head = NULL;
        dll *start = NULL;
        dll *node = NULL;
    public:
        void makePool(int num);
        dll* getNode();
        bool putNode(dll *curr);
        bool destroy();
};


#endif