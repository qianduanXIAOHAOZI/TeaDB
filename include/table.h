#ifndef TEADB_TABLE_H
#define TEADB_TABLE_H

#include <iostream>
#include <cstring>
#include <strstream>
#include <sstream>
#include <list>
using std::string;
using std::list;

namespace TeaDB {
    class table {
    public:
        typedef list<string> fields;
        table(string n, string d, string p = "/usr/local/TeaDB/") : name(n), dbName(d), path(p) { }
        void create();
        void insert(string);
        fields find(string, string, unsigned long long limit = 9223372036854775807);
        inline string nameOf() {
            return name;
        }
        inline string DBnameOf() {
            return dbName;
        }
        inline string pathOf() {
            return path;
        }
        static const unsigned short TABLE_SIZE = 1024;
        static const unsigned long long FLOAT_ACC = 10000000000;
    private:
        string name;
        string dbName;
        string path;
        string lltoString(unsigned long long t)
        {
            std::string result;
            std::strstream ss;
            ss <<  t;
            ss >> result;
            return result;
        }
        std::string& trim(std::string &s)
        {
            if (s.empty())
            {
                return s;
            }

            s.erase(0,s.find_first_not_of(" "));
            s.erase(0,s.find_first_not_of("\n"));
            s.erase(s.find_last_not_of(" ") + 1);
            s.erase(s.find_last_not_of("\n") + 1);
            return s;
        }
    };
}


#endif //TEADB_TABLE_H
