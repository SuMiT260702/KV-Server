#ifndef DLL_H
#define DLL_H
#include <string>

using namespace std;
class dll{
    public:
    string key;
    dll*prev;
    dll*next;
};


#endif