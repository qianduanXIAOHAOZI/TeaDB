#include <iostream>
#include <string>
#include <ctime>
#include <sys/time.h>
#include "include/db.h"
using std::string;

int64_t getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    time_t start = getCurrentTime();
    TeaDB::db db("test");
//    db.create();
    db.insert("test", false);

    auto table = db.tab("test");

//    for (int i = 0; i < 10000; i++) {
//        table->insert("number: " + std::to_string(i));
//    }

    for (int i = 0; i < 100000; i++) {
        auto res = table->find("number", "1234", 1);
        for (auto iter = res.begin(); iter != res.end(); iter++) {
//            std::cout << *iter << std::endl;
        }
    }

////    std::cout << table << std::endl;
//    table->insert("name: \"TeaDB\", language: \"c++\", level: 5");
//    table->insert("name: \"test\", language: \"test\", level: 5.5");
//    table->insert("best_number: 5");
    time_t end = getCurrentTime();
    std::cout << "start: " << start << "ms, end: " << end << "ms, used " << (end - start) << "ms" << std::endl;
    return 0;
}