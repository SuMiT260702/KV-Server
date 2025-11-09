#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <iostream>
#include <string>

using namespace std;


class database
{
    private:
    pqxx::connection *C;
    pqxx:: result R;
    char query[5000];
    
    public:
        double count = 0;
        database();
        int insert(string &key,string &value); 
        /**
         * 
         * @brief try to insert key and value in db if not present, if presernt it ignore 
         * 
         * @note using ON CONFLICT DO NOTHING in Postgresql query
         * 
         * @return number of rows affected by query, if 0 means key already present;
         * 
         * @return -1 if error happen in running db
         * 
         */
        int remove(string &key);
        /**
         * @brief delete a key from db if present 
         * 
         * @note DELETE normally do nothing is key not present 
         * 
         * @return number of rows affected by query, 0 means key not present
         * 
         * @return -1 if error happen in running db
         */
        int update(string &key,string &value);
        /**
         * @brief update a key from db if present 
         * 
         * @note UPDATE and WHERE normally do nothing is key not present 
         * 
         * @return number of rows affected by query, 0 means key not present
         * 
         * @return -1 if error happen in running db
         */
        int get(string &key, string &return_value);
        /**
         * @brief get value of key from database and put in return_value variable
         * 
         * @note SELECT value FROM kvserver WHERE key = 'your_key'. format is used
         * 
         * @return number of rows affected by query, 0 means key not present
         * 
         * @return -1 if error happen in running db
         */

};

#endif