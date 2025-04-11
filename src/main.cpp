#include "DataFetcher.hpp"
#include "StrategyEngine.hpp"
#include "TradeSimulator.hpp"
#include "Logger.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    try
    {
        DataFetcher fetcher;
        StrategyEngine strategy(5, 20);
        TradeSimulator trader(1000.0);
        Logger logger("trade_log.txt");

        while (true)
        {
            // auto prices = fetcher.getPriceHistory("bitcoin", 30);
            auto prices = fetcher.getPriceHistory("bitcoin", 91);
            if (prices.empty())
            {
                logger.log("Error: Failed to fetch price data");
                std::cerr << "No prices fetched, retrying in 5 minutes..." << std::endl;
                std::this_thread::sleep_for(std::chrono::minutes(5));
                continue;
            }

            std::string signal = strategy.analyze(prices);
            trader.executeTrade(signal, prices.back());
            logger.log("Signal: " + signal + ", Price: " + std::to_string(prices.back()) +
                       ", Balance: " + std::to_string(trader.getBalance()));
            std::cout << "Signal: " << signal << ", Price: " << prices.back()
                      << ", Balance: " << trader.getBalance() << std::endl;
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}