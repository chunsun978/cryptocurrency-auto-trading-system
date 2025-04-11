

#ifndef STRATEGYENGINE_HPP
#define STRATEGYENGINE_HPP
#include <string>
#include <vector>

class StrategyEngine
{
public:
    StrategyEngine(int shortPeriod, int longPeriod);
    std::string analyze(const std::vector<double> &prices);

private:
    double calculateMA(int period, int endIndex, const std::vector<double> &prices);
    int shortPeriod_;
    int longPeriod_;
};
#endif