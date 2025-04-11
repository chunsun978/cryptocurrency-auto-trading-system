#ifndef TRADESIMULATOR_HPP
#define TRADESIMULATOR_HPP

#include <string>

class TradeSimulator
{
public:
    TradeSimulator(double initialBalance);
    void executeTrade(const std::string &signal, double price);
    double getBalance() const;

private:
    double balance_;
    bool holdingCoin_;
    double coinAmount_;
};

#endif