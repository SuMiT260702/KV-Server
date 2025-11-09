#include "dllPool.h"
#include <iostream>

using namespace std;


void dllPool::makePool(int num)
{
    // cout<<"dllPool::makePool \n";
    noOfNode = num;
    dll *raw = (dll *)malloc(sizeof(dll) * noOfNode);
    if(raw == NULL)
    {
        cout<<"DLL pool fail\n";
        exit(2);
    }
    head = raw;
    start = head;
    for(int i = 0;i<noOfNode-1;i++)
    {
        raw[i].prev=NULL;
        raw[i].next = &raw[i+1];
    }
    raw[noOfNode-1].prev = NULL;
    raw[noOfNode-1].next = NULL;
}


dll* dllPool::getNode()
{
    // cout<<"dllPool::getNode \n";
    if(head == NULL)
    {
        return NULL;
    }
    node = head;
    head = head->next;
    node->next = NULL;
    node->prev = NULL;
    return node;
}


bool dllPool::putNode(dll *curr)
{
    // cout<<"dllPool::putNode \n";
    if(curr==NULL)
    {
        return false;
    }
    if(head == NULL)
    {
        head = curr;
        curr->prev = NULL;
        curr->next = NULL;
        return true;
    }
    curr->prev = NULL;
    curr->next = head;
    head = curr;
    return true;
}

bool dllPool::destroy()
{
    // cout<<"dllPool::destroy \n";
    free(start);
    head = NULL;
    start = NULL;
    return true;
}