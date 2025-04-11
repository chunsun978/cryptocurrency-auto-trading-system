#ifndef DATAFETCHER_HPP
#define DATAFETCHER_HPP
#include <string>
#include <vector>
#include <utility>

class DataFetcher
{
public:
    DataFetcher();
    std::vector<std::pair<double, int64_t>> getPriceHistory(const std::string &coin, int days); // Return price and timestamp
    double getCurrentPrice(const std::string &coin);
};
#endif