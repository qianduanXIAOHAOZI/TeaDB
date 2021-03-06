#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <sstream>
#include "../include/table.h"
#include "../base64-master/base64.h"
using std::fstream;
using std::ofstream;
using std::cout;
using std::endl;


void TeaDB::table::create() {
//    std::cout << path << ", " << dbName << ", " << name << std::endl;

    // 创建表文件夹
    string command("mkdir " + path + dbName + "/" + name);
    system(command.c_str());

    // 存储系统最大_id字段
    ofstream _idFile(path + dbName + "/" + name + "/_id.txt");
//    cout << path + dbName + "/" + name + "/_id.txt" << endl;
    _idFile << "0";
    _idFile.close();
}

void TeaDB::table::insert(string line) {
    line += " ";// 防止一些错误，所以在这个后面加上一个空格防止此类错误的产生
    unsigned long long maxId;
    fstream _idFileIn(path + dbName + "/" + name + "/_id.txt");

    _idFileIn >> maxId;
    _idFileIn.close();
    maxId++;

//    line = line;
    unsigned long long _IdFileID = maxId / TABLE_SIZE;
    string maxIdString = lltoString(maxId);
    fstream _idFileSaver(path + dbName + "/" + name + "/_id-" + lltoString(_IdFileID), std::ios::app);
    _idFileSaver << "_id:" + maxIdString + "," + line << endl;
    _idFileSaver.close();

    string token(""), value("");
    bool stringT = false;
    bool gettingToken = true;
    bool escape = false;
    bool isString = false;
    long lineLen = line.length();
    long lineLastIndex = line.length() - 1;

    for (long i = 0; i < lineLen; i++) {
        if (gettingToken) {// 获取字段名
            if (line[i] == ':') {
                gettingToken = false;
            } else {
                token += line[i];
            }
        } else {// 获取字段值
            if (line[i] == ',' || i == lineLastIndex) {
                if (!stringT) {
                    token = trim(token);
                    value = trim(value);
                    if (token != "_id") {
                        // 写入链接表
                        if (isString) {
                            value.erase(0, 1);
                            value.erase(value.end() - 1);
                            string tmp("");
                            Base64::Encode(value, &tmp);
                            value = tmp;
                            unsigned long long n = 0;
                            while (true) {
//                                std::cout << n << std::endl;
                                fstream file(path + dbName + "/" + name + "/string-" + token + "-" + lltoString(n));
                                if (!file) {
                                    file.close();
                                    fstream writer(
                                            path + dbName + "/" + name + "/string-" + token + "-" + lltoString(n),
                                            std::ios::out);
                                    writer << value << endl << maxId;
                                    writer.close();
                                    break;
                                }
                                string content("");
//                            file >> content;
                                getline(file, content);
                                if (content == value) {
                                    file.close();
                                    fstream writer(
                                            path + dbName + "/" + name + "/string-" + token + "-" + lltoString(n),
                                            std::ios::app);
                                    writer << endl << maxId;
                                    writer.close();
                                    break;
                                }
                                if (content < value) n = 2 * n + 1;
                                if (content > value) n = 2 * n + 2;
                                file.close();
                            }
                        } else {
                            unsigned long long v = (unsigned long long) ((double)(FLOAT_ACC) * atof(value.c_str()));
//                            std::cout << v << ", " << atof(value.c_str()) << std::endl;
                            unsigned long long fileId = v / TABLE_SIZE;
                            fstream fileSaver(path + dbName + "/" + name + "/" + token + "-" + lltoString(fileId), std::ios::app);
                            fileSaver << lltoString(v) << " " << maxIdString << endl;
                            fileSaver.close();
                        }
                    }

                    // 重置
                    token = "";
                    value = "";
                    stringT = false;
                    gettingToken = true;
                    escape = false;
                    isString = false;
                } else {
                    value += line[i];
                }
            } else {
                value += line[i];
            }
            if (line[i] == '\\') {
                escape = !escape;
            }
            if (line[i] == '\"') {
                if (!escape) {
                    isString = true;
                    stringT = !stringT;
                }
            }
        }
    }

    fstream _idFileOut(path + dbName + "/" + name + "/_id.txt", std::ios::in | std::ios::out | std::ios::trunc);
    _idFileOut.write(maxIdString.c_str(), maxIdString.length());
    _idFileOut.close();
}

TeaDB::table::fields TeaDB::table::find(string token, string v, unsigned long long limit) {
    bool isString = false;
    fields result;
    if (token == "_id") {
        unsigned long long id = atoll(v.c_str());
        fstream idTable(path + dbName + "/" + name + "/_id-" + lltoString(id / TABLE_SIZE));
//                _field res;
        string tableLine;
        string _idValue;
        while (getline(idTable, tableLine)) {
            _idValue = "";
            int i = 0;
            for (; i < tableLine.length() && tableLine[i] != ':'; i++) {}
            i++;
            for (; i < tableLine.length() && tableLine[i] != ','; i++) {
                _idValue += tableLine[i];
            }
            _idValue = trim(_idValue);
            if (v == _idValue) {// 找到结果
                if (result.size() >= limit) break;// 个数达成
                result.push_back(tableLine);
            }
        }
        idTable.close();
        return result;
    }
    if (v[0] == '\"') isString = true;
    if (isString) {
        v.erase(0, 1);
        v.erase(v.end() - 1);
        unsigned long long s = 0;
        string tmp;
        Base64::Encode(v, &tmp);
        v = tmp;
        while (true) {
//            std::cout << path + dbName + "/" + name + "/string-" + token + "-" + lltoString(s) << std::endl;
            fstream file(path + dbName + "/" + name + "/string-" + token + "-" + lltoString(s));
            if (!file) {
                break;
            }
            string content("");
//            std::cout << "[" << content  << "]\n[" << v << "]" << std::endl;

            bool exitLoop(false);
            getline(file, content);
            if (content == v) {
                string id_str;
                while (getline(file, id_str)) {
                    unsigned long long id;
                    std::istringstream is(id_str);
                    is >> id;
                    fstream idFile(path + dbName + "/" + name + "/_id-" + lltoString(id / TABLE_SIZE), std::ios::in);
                    string tableLine;
                    string _idValue;
//                std::cout << id << std::endl;
                    while (getline(idFile, tableLine)) {
//                    std::cout << tableLine << std::endl;
                        _idValue = "";
                        int i = 0;
                        for (; i < tableLine.length() && tableLine[i] != ':'; i++) {}
                        i++;
                        for (; i < tableLine.length() && tableLine[i] != ','; i++) {
                            _idValue += tableLine[i];
                        }
                        _idValue = trim(_idValue);
                        if (lltoString(id) == _idValue) {// 找到结果
//                        std::cout << id << ", " << _idValue << ", " << (lltoString(id) == _idValue) << std::endl;
                            result.push_back(tableLine);
                            if (result.size() >= limit) {
                                exitLoop = true;
                                break;
                            }
//                        std::cout << tableLine << std::endl;
                        }
//                    std::cout << _idValue << ", " << id << std::endl;
                    }
                    idFile.close();
                    if (exitLoop) break;
                }
                if (exitLoop) break;
//                std::cout << id << std::endl;
            }
            if (content <= v) s = 2 * s + 1;
            if (content > v) s = 2 * s + 2;
            file.close();
        }
    } else {
        unsigned long long tmp = ((double)(FLOAT_ACC) * atof(v.c_str()));
        unsigned long long tableId = tmp / TABLE_SIZE;
        fstream tableFile(path + dbName + "/" + name + "/" + token + "-" + lltoString(tableId));
        string line;
        while (getline(tableFile, line)) {
            std::strstream ss;
//            std::cout << line << std::endl;
            ss << line;
            bool exit = false;
            unsigned long long value, id;
            ss >> value >> id;
            if (value == tmp) {
                fstream idTable(path + dbName + "/" + name + "/_id-" + lltoString(id / TABLE_SIZE));
//                _field res;
                string tableLine;
                string _idValue;
                while (getline(idTable, tableLine)) {
                    _idValue = "";
                    int i = 0;
                    for (; i < tableLine.length() && tableLine[i] != ':'; i++) {}
                    i++;
                    for (; i < tableLine.length() && tableLine[i] != ','; i++) {
                        _idValue += tableLine[i];
                    }
                    _idValue = trim(_idValue);
                    if (lltoString(id) == _idValue) {// 找到结果
                        if (result.size() >= limit) {// 个数达成
                            exit = true;
                            break;
                        }
//                        std::cout << id << ", " << _idValue << ", " << (lltoString(id) == _idValue) << std::endl;
                        result.push_back(tableLine);
//                        std::cout << tableLine << std::endl;
                    }
//                    std::cout << _idValue << ", " << id << std::endl;
                }
                idTable.close();
                if (exit) break;
            }
        }
        tableFile.close();
    }
    return result;
}