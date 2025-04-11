#ifndef STRATEGYENGINE_HPP
#define STRATEGYENGINE_HPP
#include <string>
#include <vector>
#include <utility>

class StrategyEngine
{
public:
    StrategyEngine(int shortPeriod, int longPeriod);
    std::string analyze(const std::vector<std::pair<double, int64_t>> &priceData);

private:
    double calculateMA(int period, int endIndex, const std::vector<std::pair<double, int64_t>> &priceData);
    int shortPeriod_;
    int longPeriod_;
};
#endif