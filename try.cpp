#include <iostream>
#include <vector>
#include <string>

using namespace std;

void tokeznizer(char *buffer,int size, vector<string>&v)
{
    int i=0,j=0;
    while(i<size)
    {
        if(buffer[i]=='[')
        {
            i++;
            while(i<size && buffer[i]!=']')
            {
                v[j].push_back(buffer[i]);
                i++;
            }
            j++;
        }
        i++;
    }
    return ;
}

int main()
{
    char buffer[1000] ;
    // vector<string> v(3);
    // tokeznizer(buffer,29,v);
    // for(int i=0;i<3;i++)
    // {
    //     cout<<v[i]<<" ----\n";
    // }
    // cin>> buffer;

    // cout<<" first : "<<buffer<<"\n";

    // cin>>buffer;

    // cout<<"second : "<<buffer<<"\n";

    // scanf("%[^\n]",buffer);
    // getchar();
    // cout<<buffer<<" "<<strlen(buffer)<<" \n";

    // scanf("%[^\n]",buffer);
    // getchar();
    // cout<<buffer<<" "<<strlen(buffer)<<" \n";



    snprintf(buffer,1000,"jsdfhakjsdhfkasdhf");
    cout<<buffer<<" "<<strlen(buffer)<<" \n";

    snprintf(buffer,1000,"sdfjs");
    cout<<buffer<<" "<<strlen(buffer)<<" \n";

    return 0;
}