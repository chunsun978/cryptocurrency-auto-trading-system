cmake_minimum_required(VERSION 3.10)
project(CryptoTrader CXX)
set(CMAKE_CXX_STANDARD 17)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

add_executable(CryptoTrader 
    src/main.cpp 
    src/DataFetcher.cpp 
    src/StrategyEngine.cpp 
    src/TradeSimulator.cpp 
    src/Logger.cpp
)
target_include_directories(CryptoTrader PRIVATE include)
target_compile_definitions(CryptoTrader PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
target_link_libraries(CryptoTrader PRIVATE OpenSSL::SSL OpenSSL::Crypto)