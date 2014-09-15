#include "split.h"
#include "conf.h"

Conf::Conf()
{

}

Conf::~Conf()
{

}

int Conf::parse(string &conf_file, map<string, string>& result)
{
    string line;
	vector<string> tmp;
    
    ifstream in(conf_file.c_str()); 
    if (in) {
        while (getline(in, line)) {
            string str = trim(line);
            if (str.c_str()[0] == '#') 
                continue;
            if (str.length() < 3)
                continue;
            split(str, '=', tmp);
			if (tmp.size() != 2)
				continue;
            string key = trim(tmp[0]);
            string value = trim(tmp[1]);
            result[key] = value;
            // printf("[%s]=[%s]\n", key.c_str(), value.c_str());
        }
        in.close();
        return 0; 
    }
    
    return -1;  
}
