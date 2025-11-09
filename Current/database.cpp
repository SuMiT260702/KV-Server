#include "database.hxx"

using namespace std;


database::database()
{
    try{
        C = new pqxx::connection("user=halo password=halo host=localhost port=5432 dbname=halo target_session_attrs=read-write");
        if(C->is_open())
        {
            cout<<"Connected to database successfully: dbname = "<<C->dbname()<<"\n";
        }
        else{
            cout<<"database : Connection fail\n";
        }
    }
    catch(const exception &e){
        cerr<< e.what() << "\n";
    }
}

int database::insert(string &key, string &value)
{
    try
    {
        pqxx::work W(*C);
        snprintf(query,5000,"INSERT INTO kvserver (key, value) VALUES ('%s', '%s') ON CONFLICT (key) DO NOTHING;",key.c_str(),value.c_str());
        // cout<<query<<" -\n";
        R = W.exec(query);
        W.commit();
        if(R.affected_rows())
        {
            count++;
        }
        // else{
        //     cout<<query<<" -\n";
        // }
        return R.affected_rows();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return -1;
    
}

int database::update(string &key, string &value)
{
    try
    {
        pqxx::work W(*C);
        snprintf(query,5000,"UPDATE kvserver SET value = '%s' WHERE key = '%s';",value.c_str(),key.c_str());
        R = W.exec(query);
        W.commit();
        return R.affected_rows();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return -1;
    
}

int database::remove(string &key)
{
    try
    {
        pqxx::work W(*C);
        snprintf(query,5000,"DELETE FROM kvserver WHERE key = '%s';",key.c_str());
        R = W.exec(query);
        W.commit();
        return R.affected_rows();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return -1;
    
}

int database::get(string &key, string &return_value)
{
    try
    {
        pqxx::work W(*C);
        snprintf(query,5000,"SELECT value FROM kvserver WHERE key = '%s';",key.c_str());
        R = W.exec(query);
        W.commit();
        if(!R.empty()) 
        {
            // snprintf(return_value,5000,"%s",R[0][0].c_str());
            return_value.assign(R[0][0].c_str());
            return R.size();
        }
        else 
        {
            return 0;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return -1;
}


// create table kvserver (key TEXT PRIMARY KEY,value TEXT NOT NULL);
