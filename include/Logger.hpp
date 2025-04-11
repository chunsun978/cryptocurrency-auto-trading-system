#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

class Logger
{
public:
    Logger(const std::string &filename);
    void log(const std::string &message);

private:
    std::string filename_;
};

#endif