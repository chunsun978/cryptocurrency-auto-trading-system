#include "DataFetcher.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

DataFetcher::DataFetcher() {}

std::vector<std::pair<double, int64_t>> DataFetcher::getPriceHistory(const std::string &coin, int days)
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
        std::vector<std::pair<double, int64_t>> hourlyData;
        for (auto &price : j["prices"])
        {
            double priceValue = price[1].get<double>();
            int64_t timestamp = price[0].get<int64_t>();
            hourlyData.emplace_back(priceValue, timestamp);
        }
        // Downsample to daily data (average every 24 hours)
        std::vector<std::pair<double, int64_t>> dailyData;
        for (size_t i = 0; i < hourlyData.size(); i += 24)
        {
            double sum = 0.0;
            int64_t lastTimestamp = 0;
            size_t count = 0;
            for (size_t j = i; j < i + 24 && j < hourlyData.size(); ++j)
            {
                sum += hourlyData[j].first;
                lastTimestamp = hourlyData[j].second; // Use the last timestamp of the day
                count++;
            }
            if (count > 0)
            {
                dailyData.emplace_back(sum / count, lastTimestamp);
            }
        }
        std::cout << "Fetched " << dailyData.size() << " daily prices for " << coin << std::endl;
        return dailyData;
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
}