// DataFetcher.hpp
#ifndef DATAFETCHER_HPP
#define DATAFETCHER_HPP

// #pragma once
#include <string>
#include <vector>

class DataFetcher
{
public:
    DataFetcher();
    std::vector<double> getPriceHistory(const std::string &coin, int days);
    double getCurrentPrice(const std::string &coin);

private:
    // Using cpp-httplib for HTTP requests
};

#endif