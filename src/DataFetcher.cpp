#include "DataFetcher.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

DataFetcher::DataFetcher() {}

std::vector<double> DataFetcher::getPriceHistory(const std::string &coin, int days)
{
    httplib::Client cli("https://api.coingecko.com");
    auto res = cli.Get(("/api/v3/coins/" + coin + "/market_chart?vs_currency=usd&days=" + std::to_string(days)).c_str());

    if (!res || res->status != 200)
    {
        std::cerr << "Failed to fetch historical data for " << coin << ". Status: " << (res ? res->status : -1) << std::endl;
        if (res)
            std::cerr << "Response: " << res->body << std::endl;
        return {};
    }

    try
    {
        nlohmann::json j = nlohmann::json::parse(res->body);
        std::vector<double> prices;
        for (auto &price : j["prices"])
        {
            prices.push_back(price[1].get<double>());
        }
        std::cout << "Fetched " << prices.size() << " historical prices for " << coin << std::endl;
        return prices;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "JSON parse error for historical data: " << e.what() << std::endl;
        std::cerr << "Response: " << res->body << std::endl;
        return {};
    }
}

double DataFetcher::getCurrentPrice(const std::string &coin)
{
    httplib::Client cli("https://api.coingecko.com");
    auto res = cli.Get("/api/v3/simple/price?ids=" + coin + "&vs_currencies=usd");

    if (!res || res->status != 200)
    {
        std::cerr << "Failed to fetch current price for " << coin << ". Status: " << (res ? res->status : -1) << std::endl;
        if (res)
            std::cerr << "Response: " << res->body << std::endl;
        return 0.0;
    }

    try
    {
        nlohmann::json j = nlohmann::json::parse(res->body);
        double price = j[coin]["usd"].get<double>();
        std::cout << "Fetched current price for " << coin << ": $" << price << std::endl;
        return price;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "JSON parse error for current price: " << e.what() << std::endl;
        std::cerr << "Response: " << res->body << std::endl;
        return 0.0;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range error for current price: " << e.what() << std::endl;
        std::cerr << "Response: " << res->body << std::endl;
        return 0.0;
    }
}