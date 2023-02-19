#ifndef _LOG_H_
#define _LOG_H_

#include <string>

class Log {
private:
    Log();
    ~Log();
public:

    static Log & getInstance();

    void addEntry(const std::string & entry);

};

#endif