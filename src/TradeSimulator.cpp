#include "TradeSimulator.hpp"

TradeSimulator::TradeSimulator(double initialBalance)
    : balance_(initialBalance), holdingCoin_(false), coinAmount_(0.0) {}

void TradeSimulator::executeTrade(const std::string &signal, double price)
{
    if (signal == "buy" && !holdingCoin_ && balance_ > 0)
    {
        coinAmount_ = balance_ / price; // Buy fractional amount
        balance_ = 0.0;
        holdingCoin_ = true;
    }
    else if (signal == "sell" && holdingCoin_)
    {
        balance_ = coinAmount_ * price;
        coinAmount_ = 0.0;
        holdingCoin_ = false;
    }
}

double TradeSimulator::getBalance() const
{
    return balance_;
}