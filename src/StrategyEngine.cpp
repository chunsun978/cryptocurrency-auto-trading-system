#include "StrategyEngine.hpp"

StrategyEngine::StrategyEngine(int shortPeriod, int longPeriod)
    : shortPeriod_(shortPeriod), longPeriod_(longPeriod) {}

double StrategyEngine::calculateMA(int period, int endIndex, const std::vector<double> &prices)
{
    if (endIndex < period - 1)
        return 0.0;
    double sum = 0.0;
    for (int i = endIndex - period + 1; i <= endIndex; ++i)
    {
        sum += prices[i];
    }
    return sum / period;
}

std::string StrategyEngine::analyze(const std::vector<double> &prices)
{
    if (prices.size() < longPeriod_)
        return "hold";
    double shortMA = calculateMA(shortPeriod_, prices.size() - 1, prices);
    double longMA = calculateMA(longPeriod_, prices.size() - 1, prices);
    if (shortMA == 0.0 || longMA == 0.0)
        return "hold";
    if (shortMA > longMA)
        return "buy";
    if (shortMA < longMA)
        return "sell";
    return "hold";
}