#include "valueStructPool.h"

using namespace std;

int valueStructPool::makepool(int size)
{
    // cout<< "valueStructPool::makepool \n";
    int nodeSize = sizeof(valueStruct);
    int noOfNode = size/nodeSize;
    valueStruct *raw = (valueStruct *)malloc(noOfNode *nodeSize);
    if(!raw)
    {
        cout<<"Pool Creation Fail\n";
        exit(1);
    }

    start = raw;
    head = raw;
    for(int i = 0; i<noOfNode-1;i++)
    {
        memset(raw[i].value,0,sizeof(raw[i].value));
        raw[i].next = &raw[i+1];
    }
    memset(raw[noOfNode-1].value,0,sizeof(raw[noOfNode-1].value));
    raw[noOfNode-1].next = NULL;

    return noOfNode;
}

valueStruct * valueStructPool::getNode()
{
    // cout<< "valueStructPool::getNode \n";
    if(head == NULL)
    {
        return NULL;
    }
    // cout <<" lets check again --\n";
    node = head;
    head = head->next;
    node->next = NULL;
    return node;
}


void valueStructPool::putNode(valueStruct *node1)
{
    // cout<< "valueStructPool::putNode \n";
    if(node1 == NULL) return ;
    if(head == NULL)
    {
        head = node1; // node can be a list
        return ;
    }
    top = node1;
    while(node1->next!=NULL)
    {
        node1 = node1->next;
    }
    node1->next = head;
    head = top;
    return ;
}


void valueStructPool::destroy()
{
    // cout<< "valueStructPool::destroy \n";
    free(start);
    start = NULL;
    head = NULL;
    return;
}