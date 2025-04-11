#include "StrategyEngine.hpp"

StrategyEngine::StrategyEngine(int shortPeriod, int longPeriod)
    : shortPeriod_(shortPeriod), longPeriod_(longPeriod) {}

double StrategyEngine::calculateMA(int period, int endIndex, const std::vector<std::pair<double, int64_t>> &priceData)
{
    if (endIndex < period - 1)
        return 0.0;
    double sum = 0.0;
    for (int i = endIndex - period + 1; i <= endIndex; ++i)
    {
        sum += priceData[i].first; // Extract price from pair
    }
    return sum / period;
}

std::string StrategyEngine::analyze(const std::vector<std::pair<double, int64_t>> &priceData)
{
    if (priceData.size() < longPeriod_)
        return "hold";
    double shortMA = calculateMA(shortPeriod_, priceData.size() - 1, priceData);
    double longMA = calculateMA(longPeriod_, priceData.size() - 1, priceData);
    if (shortMA == 0.0 || longMA == 0.0)
        return "hold";
    if (shortMA > longMA)
        return "buy";
    if (shortMA < longMA)
        return "sell";
    return "hold";
}