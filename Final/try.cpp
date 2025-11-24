#include <iostream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
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
    try{
        pqxx::connection *C;
        C = new pqxx::connection("user=halo password=halo host=localhost port=5432 dbname=halo target_session_attrs=read-write");
        if(C->is_open())
        {
            cout<<"Connected to database successfully: dbname = "<<C->dbname()<<"\n";
        }
        else{
            cout<<"database : Connection fail\n";
        }
        pqxx::work *W;
        W = new pqxx::work(*C);
        pqxx::result r = W->exec("INSERT INTO kvserver (key, value) VALUES ('usernamess', 'alice'),('emails', 'alice@example.com'),('city', 'New York') ON CONFLICT (key) DO NOTHING;");
        cout<<r.size() <<" insert operation \n";
        W->commit();
    }
    catch(const exception &e){
        cerr<< e.what() << "\n";

    }
    return 0;
}