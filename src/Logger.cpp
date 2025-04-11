// Logger.cpp
#include "Logger.hpp"
#include <fstream>

Logger::Logger(const std::string &filename) : filename_(filename)
{
    // Constructor sets the filename
}

void Logger::log(const std::string &message)
{
    std::ofstream file(filename_, std::ios::app);
    file << message << "\n";
}