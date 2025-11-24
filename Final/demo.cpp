#include <iostream>
#include <string>
using namespace std;

int main()
{
    char buffer[5000];
    string key,value;
    snprintf(buffer,5000,"INSERT INTO kvserver (key, value) VALUES ('%s', '%s') ON CONFLICT (key) DO NOTHING;",key,value);
    return 0;
}