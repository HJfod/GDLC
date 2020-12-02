#include <string>
#include <Windows.h>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <direct.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <conio.h>

#define METH_COPY_FROM_DOESNT_EXIST 0
#define METH_SUCCESS 1

namespace methods {
    std::string replace(std::string const& original, std::string const& from, std::string const& to) {
        std::string results;
        std::string::const_iterator end = original.end();
        std::string::const_iterator current = original.begin();
        std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
        while ( next != end ) {
            results.append( current, next );
            results.append( to );
            current = next + from.size();
            next = std::search( current, end, from.begin(), from.end() );
        }
        results.append( current, next );
        return results;
    }

    std::string lower(std::string const& _s) {
        std::string s = _s;
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return s;
    }

    bool fexists(std::string _path) {
        struct stat info;
        return !(stat(_path.c_str(), &info ) != 0);
    }

    std::string workdir() {
        char buff[FILENAME_MAX];
        _getcwd(buff, FILENAME_MAX);
        std::string current_working_dir(buff);
        return current_working_dir;
    }

    void fsave (std::string _path, std::string _cont) {
        std::ofstream file;
        file.open(_path);
        file << _cont;
        file.close();
    }

    std::string fread (std::string _path) {
        std::ifstream in(_path, std::ios::in | std::ios::binary);
        if (in) {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        } throw(errno);
    }

    int fcopy(std::string from, std::string to) {
        if (!fexists(from))
            return METH_COPY_FROM_DOESNT_EXIST;
        std::ifstream src(from, std::ios::binary);
        std::ofstream dst(to,   std::ios::binary);
        dst << src.rdbuf();

        return METH_SUCCESS;
    }

    bool ewith (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    std::string sanitize (std::string _str) {
        std::string ret = _str;
        while (ret._Starts_with("\""))
            ret = ret.substr(1);
        while (ewith(ret, "\""))
            ret = ret.substr(0, ret.length() - 1);
        return ret;
    }

    std::vector<std::string> split (std::string _str, std::string _split) {
        size_t pos = 0;
        std::string token;
        std::vector<std::string> res = {};
        while ((pos = _str.find(_split)) != std::string::npos) {
            token = _str.substr(0, pos);
            res.push_back(token);
            _str.erase(0, pos + _split.length());
        }
        return res;
    }

    std::wstring conv (std::string _str) {
        return std::wstring(_str.begin(), _str.end());
    }

    int count (std::string _s, char _c) {
        int count = 0;
        for (int i = 0; i < _s.size(); i++)
            if (_s[i] == _c) count++;
        return count;
    }

    std::string remove(std::string _str, std::string _sub, bool all = false) {
        if (all) {
            int s;
            while ((s = _str.find(_sub)) != std::string::npos)
                _str = _str.erase(s, _sub.length());
            return _str;
        }
        int s = _str.find(_sub);
        if (s != std::string::npos)
            return _str.erase(s, _sub.length());
        else return _str;
    }

    char* stc (std::string _str) {
        int sz = _str.size() + 1;
        char* cstr = new char[sz];
        int err = strcpy_s(cstr, sz, _str.c_str());
        return cstr;
    }
}