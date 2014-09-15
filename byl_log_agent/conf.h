#ifndef _CONF_H_
#define _CONF_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Conf {
public:
    Conf();
    ~Conf();
    
    // trim from both ends
    static inline std::string &trim(std::string &s)
    {
        return ltrim(rtrim(s));
    }
    
    // trim from start
    static inline std::string &ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }
    
    // trim from end
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }   

    static int parse(string &conf_file, map<string, string>& result);
};

#endif
